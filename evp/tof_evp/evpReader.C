#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>
#include <pwd.h>

#ifdef __linux__
#include <sched.h>
// some older Linuxes (notably 6.2) crash and burn
// because of not having madvise so...
// Redhat EL3 crashes the kernel! with madvise... christ...
#define madvise(x,y,z)

#else
#include <procfs.h>
#endif

// MUST BE INCLUDED BEFORE ANY OTHER RTS INCLUDE!
#include <evpReader.hh>

#include <rtsLog.h>
#include <daqFormats.h>
#include <iccp.h>
#include <msgNQLib.h>



#ifndef MADV_DONTNEED
#define madvise(x,y,z)
#endif


u_int evp_daqbits ;



static int parseLRHD(struct LOGREC *lr) ;
static int evtwait(int task, ic_msg *m) ;
static int ask(int desc, ic_msg *m) ;


//static int thisNode = EVP_NODE ;
//static int evpDesc ;
static char *getCommand(void) ;




// CONSTRUCTOR!
evpReader::evpReader(char *name) 
{

//	printf("-%s-\n",'EVP_VERSION') ;

//	rtsLogLevel(WARN) ;
//	rtsLogOutput(RTS_LOG_NET) ;
#ifdef RTS_PROJECT_PP
	rtsLogAddDest("130.199.91.18",RTS_LOG_PORT_READER) ;
#else
	rtsLogAddDest("130.199.60.86",RTS_LOG_PORT_READER) ;	// reader.log to daqman
#endif


	if(name == NULL) {	// use the ACTIVE run...
		LOG(DBG,"Constructor for the ACTIVE run...",0,0,0,0,0) ;

	}
	else {
		strncpy(fname,name,sizeof(fname)-1) ;
		LOG(DBG,"Constructor %s",(uint)fname,0,0,0,0) ;
	}

	// setup...
	do_open = 1 ;
	do_mmap = 1 ;
	strcpy(evp_disk,"") ;



	desc = -1 ;
	mem_mapped = NULL ;
	bytes_mapped = 0 ;

	// need this for mmap....
	page_size = sysconf(_SC_PAGESIZE) ;

	issued = 0 ;
	last_issued = time(NULL) ;
	status = EVP_STAT_CRIT ;
	tot_bytes = 0 ;
	isdir = 0 ;	// not a directory by default...
	isfile = 0 ;	// not even a standalone file
	isevp = 0 ;	// not even the current run???

	event_number = 0 ;	// last known event...
	total_events = 0 ;
	bytes = 0 ;

	mem = NULL ;



	file_size = 0 ;		// only if it's a real big file...


	if(name == NULL) {	// EVP
		fname[0] = 0 ;	// mark as "from pool"
		reconnect() ;	// will loop until success or Cntrl-C I guess...
		return ;	// that's it....
	}



	// This code is reached only if the argument was non-NULL
	// file or directory?

	if(stat(fname, &stat_buf) < 0) {	// error
		LOG(ERR,"Can't stat \"%s\" [%s]",(int)fname,(int)strerror(errno),0,0,0) ;
//		fprintf(stderr,"CRITICAL: Can't stat \"%s\" [%s]\n",fname,strerror(errno)) ;
		sleep(1) ;
		return ;
	}

	
	LOG(INFO,"Using file \"%s\"...",(int)fname,0,0,0,0) ;

	// directory?
	if(stat_buf.st_mode & S_IFDIR) {
		LOG(DBG,"Running through a directory %s...",(uint)fname,0,0,0,0) ;

		// change to that directory...
		if(chdir(fname) < 0) {
			LOG(ERR,"Can't chdir %s [%s]",(int)fname,(int)strerror(errno),0,0,0) ;
			sleep(1) ;
			return ;
		}

		status = EVP_STAT_OK ;
		isdir = 1 ;
		return ;
	}	
	else {	// assume it's a file
		file_size = stat_buf.st_size ;
	}


	LOG(DBG,"Running through a file %s of %d bytes",(int)fname,file_size,0,0,0) ;

	desc = open(fname,O_RDONLY,0444) ;
	if(desc < 0) {	// error
		LOG(ERR,"Can't open %s [%s]",(int)fname,(int)strerror(errno),0,0,0) ;
		sleep(1) ;
		return ;
	}

	isfile = 1 ;	// must be a file...
	strcpy(file_name,fname) ;

	// all OK
	status = EVP_STAT_OK ;

	return ;
}

int evpReader::setOpen(int flg)
{	
	int ret ;

	ret = do_open ;
	do_open = flg ;

	return ret ;
}

int evpReader::setMmap(int flg)
{	
	int ret ;

	ret = do_mmap ;
	do_mmap = flg ;

	return ret ;
}

int evpReader::setLog(int flg)
{	

	if(flg) {
		rtsLogOutput(RTS_LOG_STDERR|RTS_LOG_NET) ;
	}
	else {
		rtsLogOutput(RTS_LOG_NET) ;
	}

	return 0 ;
}


char *evpReader::setEvpDisk(char *name)
{
	static char saved[256] ;

	strcpy(saved, evp_disk) ;
	strncpy(evp_disk,name,sizeof(evp_disk)-1) ;

	return saved ;
}

evpReader::~evpReader(void)
{
	LOG(DBG,"Destructor %s",(uint)fname,0,0,0,0) ;

	// clean-up input file...
	if(desc >= 0) close(desc) ;

	// clean up mem mapped stuff...
	if(mem_mapped && bytes_mapped) {
		madvise(mem_mapped, bytes_mapped, MADV_DONTNEED) ;
		munmap(mem_mapped,bytes_mapped) ;
	}
	
	// clean up input queues
	if(isevp) {
		msgNQDelete(evpDesc) ;
	}



	return ;
}

/*
	Args
	=====
	If	num == 0	Next event
		num > 0		That particula event (if it makes sense...)


	Output
	======
	Returns a valid pointer on success or NULL on error...


*/
char *evpReader::get(int num, int type)
{
	static int crit_cou = 30 ;
	static char evname[256] ;
	static struct LOGREC lrhd ;
	int ret ;
	int data_bytes ;
	int leftover, tmp ;
	u_int offset ;

	LOG(DBG,"get event %d of type %d; file %d, dir %d, evp %d",num,type,isfile,isdir,isevp) ;

	// nix some variables which may have no meaning for all streams...
	evb_type = 0 ;
	evb_type_cou = 0 ;
	evb_cou = 0 ;
	run = 0 ;

	// check status of the previous command...
	switch(status) {
	case EVP_STAT_EVT :	// something wrong with last event...
		usleep(500000) ;
		break ;
	case EVP_STAT_EOR :	// EndOfRun was the last status and yet we are asked again...
		usleep(500000) ;
		break ;
	case EVP_STAT_CRIT :
		if(crit_cou==30) LOG(WARN,"Previous error is unrecoverable! Why are you asking me again?",0,0,0,0,0) ;
		sleep(1) ;
		crit_cou-- ;
		if(crit_cou < 10) {
			if(crit_cou==0) {
				LOG(ERR,"CRITICAL ERROR: That's IT! Bye...",0,0,0,0,0);
				sleep(1) ;	// linger...
				exit(-1) ;
			}
			LOG(ERR,"Anybody there??? Exiting in %2d seconds...",crit_cou,0,0,0,0) ;
		}
		return NULL ;

	default :	// all OK...
		break ;
	}

	// unmap previous
	if(mem_mapped && bytes_mapped) {
		LOG(DBG,"Unmapping previous 0x%X, bytes %d",(uint)mem_mapped,bytes_mapped,0,0,0) ;
		madvise(mem_mapped, bytes_mapped, MADV_DONTNEED) ;
		ret = munmap(mem_mapped,bytes_mapped) ;
		mem_mapped = NULL ;
		bytes_mapped = 0 ;

		if(ret < 0) {
			LOG(ERR,"Error in munmap (%s)",(int)strerror(errno),0,0,0,0) ;
			status = EVP_STAT_CRIT ;
			return NULL ;
		}
	}

	
	if(isdir) {	// we are running in a directory and we are already chdir-ed in the constructor

		if((event_number != 0) && (token == 0)) {	// we read at least one event and it was token==0 thus this is it...
			LOG(NOTE,"Previous event was Token 0 in directory - stopping...",0,0,0,0,0) ;
			status = EVP_STAT_EOR ;
			return NULL ;
		}

		if(num==0) {	// next event
			num = ++event_number ;	// advance event counter

			int max_num = num+10 ;

			ret = 0 ;
			for(;num<max_num;num++) {
				sprintf(evname,"%d",num) ;	// make a file name...
				sprintf(file_name,"%s/%d",fname,num) ;

				errno = 0 ;
				ret = stat(evname,&stat_buf) ;
				if(ret == 0) break ;	// file OK
				else {
					LOG(WARN,"File %s has problems [%s]...",(int)evname,(int)strerror(errno),0,0,0) ;
				}
			}

			if(ret) {	// no success, report end-of0run
				LOG(WARN,"Couldn't find events around this %u... Returning EOR.",max_num,0,0,0,0) ;
				status = EVP_STAT_EOR ;
				return NULL ;
			}

		}
		else {

			sprintf(evname,"%d",num) ;	// make a file name...
			sprintf(file_name,"%s/%d",fname,num) ;

		}

		event_number = num ;

	}
	else if(isevp) {	// ACTIVE RUN
		static ic_msg m ;


		// issue get event only if not issued before
		if(issued) {
			if((time(NULL) - last_issued) > 10) {	// reissue
				LOG(DBG,"Re-issueing request...",0,0,0,0,0) ;
				issued = 0 ;
			}
		}

		if(!issued) {

			m.ld.dword[0] = htonl(type) ;	// event type...
			ret = ask(evpDesc,&m) ;
			if(ret != STAT_OK) {	// some error...

				LOG(ERR,"Queue error %d - recheck EVP...",ret,0,0,0,0) ;
				reconnect() ;
				status = EVP_STAT_EVT ;	
				return NULL ;
			}
			issued = 1 ;
			last_issued = time(NULL) ;
		}

		// wait for the event
		ret = evtwait(evpDesc, &m) ;


		LOG(DBG,"evtwait returned %d [%d], evt status %d",ret,STAT_OK,m.head.status,0,0) ;

		if(ret == STAT_TIMED_OUT) {	// retry ...
#ifdef __linux
			sched_yield() ;
#else
			yield() ;		// give up time slice?
#endif
			usleep(1000) ;		// 1 ms?
			status = EVP_STAT_OK ;	// no error...
			return NULL ;		// but also noevent - only on wait!
		}


		// got some reply here so nix issued
		issued = 0 ;

		if(ret != STAT_OK) {
			reconnect() ;
			LOG(ERR,"Queue error %d - recheck EVP...",ret,0,0,0,0) ;
			status = EVP_STAT_EVT ;	// treat is as temporary...
			return NULL ;
		}


		// check misc. 
		if(m.head.status == STAT_SEQ) {	// end of run!
			LOG(DBG,"End of Run!",0,0,0,0,0) ;
			status = EVP_STAT_EOR ;
			return NULL ;
		}


		if(m.head.status != STAT_OK) {		// some event failure
			LOG(WARN,"Event in error - not stored...",0,0,0,0,0) ;
			status = EVP_STAT_EVT ;			// repeat
			return NULL ;
		}


		sprintf(evname,"%s%s/%d/%d",evp_disk,(char *)&m.ld.dword[5],ntohl(m.ld.dword[0]),ntohl(m.ld.dword[1])) ;
		strcpy(file_name,evname) ;

		evb_type = ntohl(m.ld.dword[2]) ;
		evb_type_cou = ntohl(m.ld.dword[3]) ;
		evb_cou = ntohl(m.ld.dword[4]) ;
		run = ntohl(m.ld.dword[0]) ;
		event_number = ntohl(m.ld.dword[1]) ;

		LOG(DBG,"Returned event - write it to -%s-",(int)evname,0,0,0,0) ;

	}

	if(isevp || isdir) {
		// single file - full name in evname!
		if(desc > 0) {
			close(desc) ;
			desc = -1 ;
		}

		errno = 0 ;

		if(do_open) {
			desc = open(evname,O_RDONLY,0666) ;
			if(desc < 0) {	
				LOG(ERR,"Error opening file %s [%s] - skipping...",(int)evname,(int)strerror(errno),0,0,0) ;
				status = EVP_STAT_EVT ;
				return NULL ;
			}

			// get the file_size ;
			ret = stat(evname,&stat_buf) ;
			if(ret < 0) {	//
				LOG(ERR,"Can't stat %s",(int)evname,0,0,0,0) ;
				status = EVP_STAT_EVT ;
				close(desc) ;
				desc = -1 ;
				return NULL ;
			}


			file_size = stat_buf.st_size ;
			tot_bytes = 0 ;
			bytes = 0 ;
		}
		else {
			file_size = 0 ;
			tot_bytes = 0 ;
			bytes = 0 ;
			status = EVP_STAT_OK ;
			total_events++ ;
			return NULL ;
		}
	}
		
	// at this point the file is opened...
	if(isfile) event_number++ ;


	do {
		// read the LRHD first
		errno = 0 ;
		ret = read(desc,(char *)&lrhd, sizeof(lrhd)) ;
		if(ret != sizeof(lrhd)) {	// error
			mem = NULL ;

			if(ret == 0) {	// EOF
				LOG(NOTE,"EOF reading LRHD...",0,0,0,0,0) ;
			}
			else {
				LOG(ERR,"Error reading LRHD [%s], desc %d, got %d, expect %d",(int)strerror(errno),desc,ret,sizeof(lrhd),0) ;
			}


			if(isfile) {
				status = EVP_STAT_EOR ;
			}
			else {
				status = EVP_STAT_EVT ;
			}

			return NULL ;
		}

		tot_bytes += ret ;

		// this also swaps the data...
		data_bytes = parseLRHD(&lrhd) ;

		if(data_bytes == -1) {	// error
			LOG(ERR,"Bad LRHD - skipping...",0,0,0,0,0) ;
			status = EVP_STAT_EVT ;
			return NULL ;
		}
		else if((data_bytes == -2) && (isdir || isfile)) {	// ENDR in LRHD
			LOG(NOTE,"ENDR record in LRHD in directory - ending run...",0,0,0,0,0) ;
			status = EVP_STAT_EOR ;
			return NULL ;
		}

	} while(data_bytes == 0) ;	// until we get to the DATA...

	if(isevp) {
		if(run != lrhd.lh.run) {
			LOG(ERR,"Run number mismatch: EVP %d vs. data %d",run,lrhd.lh.run,0,0,0) ;
		}
	}

	run = lrhd.lh.run ;		// set the run number to the one in data!

	// at this point we just read the LRHD corresponding to DATA
	// and we are positioned at DATAP....


	// map the event...
	// ...but we must do %^&*()_ acrobatics because the offset must be alligned on a page
	// boundary arrghhhhhhhhhhhhhhh
	tmp = tot_bytes / page_size ;
	leftover = tot_bytes % page_size ;

	offset = page_size * tmp ;
	bytes_mapped = data_bytes + leftover ;

	// sanity check for reads after EOF
	if((offset + bytes_mapped) > file_size) {




		if(isfile) {
			LOG(WARN,"This event is truncated... Good events %d [%d+%d > %d]...", total_events,offset,bytes_mapped,file_size,0) ;
			status = EVP_STAT_EOR ;
		}
		else {
			LOG(ERR,"This event is truncated... Good events %d [%d+%d > %d]...", total_events,offset,bytes_mapped,file_size,0) ;
			status = EVP_STAT_EVT ;
		}

		return NULL ;
	}

	if(do_mmap) {

		mem = (char *) mmap(NULL, bytes_mapped, PROT_READ,MAP_SHARED|MAP_NORESERVE,desc,offset) ;
		if(((void *) mem) == MAP_FAILED) {
			LOG(ERR,"Error in mmap (%s)",(int)strerror(errno),0,0,0,0) ;
			status = EVP_STAT_CRIT ;
			mem = NULL ;
			mem_mapped = NULL ;

			close(desc) ;
			desc = -1 ;

			return NULL ;
		}

		// need to save this for munmap
		mem_mapped = mem ;

		// the memory will most likely be used only once... hmmm...
		madvise(mem_mapped, bytes_mapped, MADV_SEQUENTIAL) ;
		

		// adjust mem
		mem += leftover ;
	}
	else {
		mem_mapped = NULL ;
		bytes_mapped = 0 ;
		mem = NULL ;
	}



	// at this point mem points to DATAP so let's see...
	struct DATAP *datap = (struct DATAP *) mem ;

	if(checkBank(datap->bh.bank_type,"DATAP") < 0) {	// something very wrong!
		status = EVP_STAT_EVT ;
		return NULL ;
	}

	// endianess mess
	// ext. system
	u_int off = datap->det[EXT_SYSTEM].off ;
	u_int len = datap->det[EXT_SYSTEM].len ;

	token = datap->bh.token ;	
	trgword = datap->trg_word ;
	evt_time = datap->time ;	// UNIX time
	detectors = datap->detector ;
	seq = datap->seq ;
	daqbits = datap->L3_Summary[0] ;

	// swaps
	if(datap->bh.byte_order != DAQ_RAW_FORMAT_ORDER) {	// swap

		off = swap32(off) ;
		len = swap32(len) ;

		token = swap32(token) ;
		trgword = swap32(trgword) ;
		evt_time = swap32(evt_time) ;
		detectors = swap32(detectors) ;
		seq = swap32(seq) ;
		daqbits = swap32(daqbits) ;

		trgcmd = (swap32(datap->trg_in_word) >> 12) & 0xF ;	// _just_ the trigger command
		daqcmd = (swap32(datap->trg_in_word) >> 8) & 0xF ;	// DAQ command

	}
	else {

		trgcmd = ((datap->trg_in_word) >> 12) & 0xF ;	// _just_ the trigger command
		daqcmd = ((datap->trg_in_word) >> 8) & 0xF ;	// DAQ command

	}

	evp_daqbits = daqbits ;


	if(len) {	// extended should be there
		struct DATAPX *dx ;

		dx = (struct DATAPX *)(mem + off*4) ;

		if(checkBank(dx->bh.bank_type,CHAR_DATAPX) < 0) {
			status = EVP_STAT_EVT ;
			return NULL ;
		}

		if(dx->bh.byte_order != DAQ_RAW_FORMAT_ORDER) {	// swap

			int i ;
			for(i=0;i<22;i++) {
				LOG(DBG,"   detx %2d: len %d, off %d",i+10, swap32(dx->det[i].len),swap32(dx->det[i].off),0,0) ;
			}
		}
		else {
			int i ;
			for(i=0;i<22;i++) {
				LOG(DBG,"   detx %2d: len %d, off %d",i+10, (dx->det[i].len), (dx->det[i].off),0,0) ;
			}


		}
	}



	// all done - all OK
	status = EVP_STAT_OK ;
	bytes = data_bytes ;	// size of this event...
	tot_bytes += bytes ;
	total_events++ ;


	// move to next event although it may make no sense...
	ret = lseek(desc, bytes, SEEK_CUR) ;
	if(ret < 0) {
		LOG(ERR,"lseek error (%s)",(int)strerror(errno),0,0,0,0) ;
	}

	
	LOG(DBG,"Curr offset %d, bytes %d, tot_bytes %d, file size %d",ret,bytes,tot_bytes,file_size,0) ;


	// return the pointer at the beginning of DATAP!
	// at this point we return the pointer to READ_ONLY DATAP
	// the event size is "bytes"
	// 
	return (char *)mem ;
}


static int parseLRHD(struct LOGREC *lr)
{


	if(memcmp(lr->lh.bank_type,"LRHD",4) != 0) {
		lr->lh.bank_type[7] = 0 ;
		LOG(ERR,"Bank is not LRHD but is [%s]!",(uint)lr->lh.bank_type,0,0,0,0) ;
		return -1 ;
	}


	if(lr->lh.byte_order == DAQ_RAW_FORMAT_ORDER) {	// no swapping necessary
		;
	}
	else {
		// now that I swapped it
		lr->lh.byte_order = swap32(lr->lh.byte_order) ;
		lr->lh.length = swap32(lr->lh.length) ;
		lr->lh.run = swap32(lr->lh.run) ;
		lr->lh.format_ver = swap32(lr->lh.format_ver) ;
		lr->lh.crc = swap32(lr->lh.crc) ;
		
		lr->length = swap32(lr->length) ; 
		lr->blocking = swap32(lr->blocking) ;
		lr->crc = swap32(lr->crc) ;
	}


	if(memcmp(lr->record_type,"BEGR",4) == 0) return 0 ;
	if(memcmp(lr->record_type,"DATA",4) == 0) return (lr->length - lr->lh.length)*4 ;	// size in bytes!
	if(memcmp(lr->record_type,"ENDR",4) == 0) return -2 ;	// END of RUN

	// unknown bank
	lr->record_type[7] = 0 ;
	LOG(CAUTION,"Unknown bank type [%s]!",(uint)lr->record_type,0,0,0,0) ;

	return -1 ;
}


static int evtwait(int desc, ic_msg *m)
{
	int ret ;
	static int counter = 0 ;
	
	// wait with no timeout - handled by higher level code...
	ret = msgNQReceive(desc,(char *)m, sizeof(ic_msg),NO_WAIT) ;

	LOG(DBG,"msgNQReceive returned %d",ret,0,0,0,0) ;

	if(ret == MSG_Q_TIMEOUT) {
		counter++ ;
		if(counter >= 100) {
			counter = 0 ;
			if(msgNQCheck(desc)) return STAT_TIMED_OUT ;
			else {
				LOG(DBG,"EVP_TASK died",0,0,0,0,0) ;
				return STAT_ERROR ;
			}
		}
		return STAT_TIMED_OUT ;
	}

	counter = 0 ;

	if(ret > 0) {
		int i, *intp ;
		intp = (int *) m ;
		LOG(DBG,"0x%08X 0x%08X 0x%08X; %d %d",*intp,*(intp+1),*(intp+2),m->head.daq_cmd,m->head.status) ;

		for(i=0;i<3;i++) {
			*(intp+i) = ntohl(*(intp+i)) ;
		}
		LOG(DBG,"0x%08X 0x%08X 0x%08X; %d %d",*intp,*(intp+1),*(intp+2),m->head.daq_cmd,m->head.status) ;
		return STAT_OK ;
	}

	return STAT_ERROR ;	// critical - please reboot

}

int evpReader::reconnect(void)
{
	int ret ;
	int retries ;
	ic_msg msg ;


	evpDesc = -1 ;	// mark as disconnected

	retries = 0 ;

for(;;) {	// until success...


	evpDesc = msgNQCreate(EVP_HOSTNAME,EVP_PORT,120) ;

	if(evpDesc < 0) {
		LOG(ERR,"Can't create connection to %s:%d [%s] - will retry...",(u_int)EVP_HOSTNAME,EVP_PORT,
		    (u_int)strerror(errno),0,0) ;
		fprintf(stderr,"CRITICAL: Can't create connection to %s:%d [%s] - will retry...\n",EVP_HOSTNAME,EVP_PORT,
			strerror(errno)) ;
		sleep(10) ;
		retries++ ;
		continue ;
	}


	if(retries) {
		LOG(WARN,"Connection suceeded!",0,0,0,0,0) ;
	}


	LOG(DBG,"Opened connection to %s, port %d on descriptor %d",(u_int)EVP_HOSTNAME, EVP_PORT,evpDesc,0,0) ;

	msg.head.daq_cmd = RTS_ETHDOOR_ANNOUNCE ;
	msg.head.status = 0 ;
	msg.ld.dword[0] = htonl(getpid()) ;

	char *user ;

	struct passwd *passwd = getpwuid(getuid()) ;
	if(passwd == NULL) {
		LOG(WARN,"User doesn't exist?",0,0,0,0,0) ;
		user = "????" ;
	}
	else {
		user = passwd->pw_name ;
	}


	strncpy((char *)&msg.ld.dword[1],user,12) ;
	strncpy((char *)&msg.ld.dword[4],getCommand(),12) ;
	msg.head.valid_words = 1+7 ;

#define BABABA
#ifdef BABABA
		{
			int jj ;
			int *intp = (int *) &msg ;
			for(jj=0;jj<3;jj++) {
				*(intp+jj) = htonl(*(intp+jj)) ;
			}
		}
#endif

	ret = msgNQSend(evpDesc,(char *)&msg,120,60) ;
	if(ret < 0) {
		LOG(ERR,"Can't send data to %s! - will reconnect...",(u_int)EVP_HOSTNAME,0,0,0,0) ;
		msgNQDelete(evpDesc) ;
		evpDesc = -1 ;
		continue ;
		//return ;
	}

	// all OK...
	isevp = 2  ;	// mark first
	status = EVP_STAT_OK ;
	LOG(DBG,"Returning to caller, status %d",status,0,0,0,0) ;
	break  ;	// that's it....
	}




	return 0 ;
}

static int ask(int desc, ic_msg *m)
{
	int ret ;
	time_t tm ;
	int jj ;
	int *intp = (int *) m ;

	m->head.daq_cmd = EVP_REQ_EVENT ;
	m->head.status = STAT_OK ;
	m->head.dst_task = EVP_TASK ;
	m->head.valid_words = 1+1 ;	// reserve one for type...
	m->head.source_id = EVP_NODE ;
	m->head.src_task = EVP_TASK_READER ;

	LOG(DBG,"Sending request to EVP_TASK",0,0,0,0,0) ;
	tm = time(NULL) ;


	for(jj=0;jj<3;jj++) {
		*(intp+jj) = htonl(*(intp+jj)) ;
	}

	ret = msgNQSend(desc, (char *)m, 120,10) ;

	LOG(DBG,"msgNQSend returned %d in %d seconds",ret,time(NULL)-tm,0,0,0) ;

	if(ret < 0) {	// communication error
		return STAT_ERROR ;
	}
	else {	// OK
		return STAT_OK ;
	}
}


static char *getCommand(void)
{


        static char *str = "(no-name)" ;
#ifdef __linux__
        FILE *file ;
        static char name[128] ;
        int dummy ;

        file = fopen("/proc/self/stat","r") ;
        if(file==NULL) return str ;

        fscanf(file,"%d %s",&dummy,name) ;
        fclose(file) ;
        *(name+strlen(name)-1) = 0 ;
        return name+1 ;
#else   // solaris
        int fd, ret ;
        static struct psinfo ps ;

        fd = open("/proc/self/psinfo",O_RDONLY,0666) ;
        if(fd < 0) return str ;

        ret = read(fd,(char *)&ps,sizeof(ps)) ;
        close(fd) ;

        if(ret != sizeof(ps)) {
                return str ;
	}

        return ps.pr_fname ;
#endif
}

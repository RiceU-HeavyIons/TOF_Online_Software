#ifndef _EVP_READER_HH_
#define _EVP_READER_HH_


#include <sys/types.h>
#include <sys/stat.h>

#include <evp.h>		// for some constants

#include <evpSupport.h>

#ifndef RTS_PROJECT_PP	// STAR specific...

#include <scReader.h>
#include <trgReader.h>
#include <tpcReader.h>
#include <svtReader.h>
#include <ftpReader.h>
#include <ricReader.h>
#include <tofReader.h>
#include <fpdReader.h>
#include <pmdReader.h>
#include <emcReader.h>
#include <l3Reader.h>
#include <ssdReader.h>

#endif

#include <pp2ppReader.h>

class evpReader {
public:
	evpReader(char *fname) ;
	~evpReader(void) ;

	char *get(int which, int type=EVP_TYPE_ANY) ;	


	int setMmap(int flag) ;		// flag=1 - enable memory mapping
	int setOpen(int flag) ;		// flag=1 - enable file open
	int setLog(int flag) ;		// flag=1 - enable logging...
	char *setEvpDisk(char *fs) ;	// sets the local directory name where the evp.star disks
					// are mounted

	int status ;		// 0 OK, all others should disregard the event!

	// all the variables below are valid ONLY if the status is 0!!!

	u_int event_number ;	// current event in the evp or file 
	u_int total_events ;	// total number of events seen by this object so far

	u_int bytes ;		// size of the current event
	

	char file_name[256] ;	// fully qualified file name (on evp.star)
	u_int file_size ;	// size of the file in bytes


	u_int run ;		// current run number

	u_int evb_type ;	// event type	(only from ACTIVE run)
	u_int evb_cou ;		// total events in this run (only from ACTIVE run)
	u_int evb_type_cou ;	// total events of the above type in this run (only from ACTIVE run)

	u_int token ;		// current token
	u_int trgcmd ;		// current trigger command
	u_int daqcmd ;		// current DAQ command
	u_int trgword ;		// the Trigger Word
	u_int phyword ;		// the Physics Word
	u_int daqbits ;		// "offline" bits aka L3 summary...


	u_int evt_time ;	// time in UNIX seconds
	u_int seq ;		// event sequence from EVB
	u_int detectors ;	// detectors present bit mask according to DAQ!

	int isdir ;		// are we running through a directory?
	int isfile ;		// ... or a file?
	int isevp ;		// ... or the active run from EVP?

private:	// one shouldn't care...
	int reconnect(void) ;

	char *mem ;	


	int do_mmap ;
	int do_open ;
	int do_log ;
	char evp_disk[256]; 

	int issued ;
	int last_issued ;	// time

	int task ;
	int desc ;		// file descriptor

	int evpDesc ;		// message queue desc.
	// file variables
	char fname[256] ;
	struct stat stat_buf ;


	u_int tot_bytes ;

	// mmap variables
	int bytes_mapped ;
	char *mem_mapped ;
	int page_size ;
} ;


#endif

// JS: This is a tof reader program based on Tonko's "special.C" program, which allows to read .daq files and to decode them.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#include <evpReader.hh>

#include <rtsSystems.h>	// for detector ids...

extern int sanityCheck(char *datap) ;



int main(int argc, char *argv[])
{
  char *mem ;
  int ret ;
  extern char *optarg ;
  extern int optind ;
  int c ;
  char *datap ;
  class evpReader *evp = NULL ;
  int good = 0, bad = 0 ;
  static char mountpoint[32] ;
  int fcount ;
  char *fnames[1000] ;	// up to 1000 files, hope that's enough...
  int i ;
  int dump_det ;
  int check_only ;
  // parse command line arguments
  // but set the defaults before...

  int evtype = EVP_TYPE_ANY ;	// any
  strcpy(mountpoint,"") ;


  check_only = 0 ;	// don't _just_ check the headers...

  dump_det = -1 ;	// default is no dump

  while((c = getopt(argc,argv,"Ct:d:m:w:D:")) != EOF)
    switch(c) {
    case 't' :
      evtype = atoi(optarg) ;		// request type
      break ;
    case 'm' :				// mountpoint for data on evp i.e. /evp
      strncpy(mountpoint,optarg,sizeof(mountpoint)-1) ;
      break ;
    case 'D' :				// dump data for this detector
      dump_det = atoi(optarg) ;	
      break ;
    case 'C' :				// only run sanity checks
      check_only = 1 ;
      break ;
    case '?' :
      fprintf(stdout,"Usage: %s [-t type ] [-C] [-m mountpoint] [-D det_id] <files...>\n",argv[0]) ;
      return -1 ;
      break ;
    }



  // repack pointers to filenames
  fcount = 0 ;
  if(optind < argc) {
    while(optind < argc) {
      fnames[fcount] = argv[optind] ;
      optind++ ;
      fcount++ ;
    }
  }
  else {
    fnames[fcount] = NULL ;
    fcount = 1 ;
  }


  good = bad = 0 ;

  for(i=0;i<fcount;i++) {
    // One must first call the constructor of the data
    // stream.
    // This is very lightweight and doesn't take too much
    // time or space.
    // One can have multiple streams but keep in mind
    // that each detector reader has one buffer!
    // The argument is either a string in which case
    // the call will determine if it's a directory
    // or a file and proceed accordingly.
    // If the argument is NULL it will connect to the
    // current LIVE! run
	

    if(fnames[i]) fprintf(stdout,"Calling constructor [%d/%d], with filename %s\n",(i+1),(fcount+1),fnames[i]) ;
    else fprintf(stdout,"Calling constructor [%d/%d], with filename NULL",(i+1),(fcount+1)) ;

    evp = new evpReader(fnames[i]) ;

    // MUST CHECK THE STATUS!!!
    // as well as the variable itself!
    if(evp == NULL) {
      char *fl = "NULL" ;
      if(fnames[i]) fl = fnames[i] ;
			
      fprintf(stdout,"Constructor for the evpReader failed for file %s!?\n",fl) ;
      return -1 ;
    }


    if(evp->status) {	// error in constructor
      char *fl = "NULL" ;
      if(fnames[i]) fl = fnames[i] ;

      fprintf(stdout,"Error initializing reader for file %s!\n",fl);
      return -1 ;
    }


    // set the evp.star mountpoint i.e. if evp.star is mounted in your local
    // /net/evp.star then mountpoint="/net/evp.star"
    evp->setEvpDisk(mountpoint) ;


    // The EVENT LOOP!!!!

    for(;;) {
      int end_of_file ;

      mem = evp->get(0,evtype) ;	// get the next event!

			
      end_of_file = 0 ;
      if(mem == NULL) {	// event not valid
	switch(evp->status) {
	case EVP_STAT_OK :	// should retry as fast as possible...
	  continue ;
	case EVP_STAT_EOR :	// EOR or EOR - might contain token 0!
	  if(evp->isevp) {	// keep going until the next run...
	    sleep(5) ;	// ...but let's be friendly...
	    continue ;
	  }
	  else {
	    end_of_file = 1 ;
	    break ;
	  }
	case EVP_STAT_EVT :
	  bad++ ;
	  fprintf(stdout,"Problem getting event - skipping [good %d, bad %d]\n",good,bad) ;
	  sleep(5) ;	// wait a sec...
	  continue ;
	case EVP_STAT_CRIT :
	  fprintf(stdout,"Critical error - halting...\n") ;
	  return -1 ;
	}
				
      }

      if(end_of_file) break ;	// go to next file

      if(sanityCheck(mem)) {
	fprintf(stdout,"Bad event - skipping...\n") ;
	continue ;
      }


      // once evp->get returns a valid pointer the event is available
      good++ ;
      if((good%1000)==0) {
	fprintf(stdout,"Events processed %d...\n",good) ;
      }

      fprintf(stdout,"**** Event %d: bytes %d, token %d, trg_cmd %d, FILE %s\n",evp->event_number,evp->bytes,
	      evp->token,evp->trgcmd,evp->file_name) ;

      // make humanly readable time from the UNIX time...
      char *stime ;
      stime = ctime((long int *)&evp->evt_time) ;
      *(stime+strlen(stime)-1) = 0 ;

      fprintf(stdout, "     Trigger Word 0x%02X, time %u (%s), daqbits 0x%04X\n",
	      evp->trgword,evp->evt_time,stime,evp->daqbits) ;

      if(check_only) continue ;

      //if(evp->token != 0) continue ;

      // the return value of the "get" method points to datap so one may
      // use it of one knows what one is doing (unlikely)

      datap = mem ;

      // everything beyond this point is up to the user
      // and all the calls are optional
      // i.e. the FTPC guys would just call the ftpReader(datap)


      ret = tofReader(datap) ;
      if(ret <= 0) {
	if(ret < 0) {
	  fprintf(stdout, "TOF: problems in data (%d) - continuing...\n",ret) ;
	}
	else {
	  fprintf(stdout,"TOF: not present...\n") ;
	}
      }
      else {
	fprintf(stdout,"TOF: %d bytes\n",ret) ;
	// now do something:

	for (int ii = 0; ii<4; ii++) {
	  fprintf(stdout,"TOFDDLR %d: length %d words\n",ii+1, tof.ddl_words[ii]);
	  if (tof.ddl_words[ii] != 0) {
	    for(int jj=0; jj<tof.ddl_words[ii]; jj++) {
	      fprintf(stdout, "DDLR %d: %3d: 0x%08X\n",ii+1,i,l2h32(tof.ddl_word_p[ii][jj])) ;
	    }
	  }
	}

      } // else

    }	// end of EVENT LOOP


    fprintf(stdout,"Processed %s: %d good, %d bad events total...\n",fnames[i],good,bad) ;

    delete evp ;
  }	// end of input file loop

  return 0 ;
}

// $Id$

// JS: This is a tof reader program based on Tonko's "special.C" program, 
// which allows to read .daq files and to decode them.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#include <evpReader.hh>

#include <rtsSystems.h>	// for detector ids...

// Root stuff:
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TMapFile.h"

extern int sanityCheck(char *datap) ;

const bool DEBUG = false;


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
  int 	tofbad1 = 0, // separators missing
    tofbad2 = 0, // trigger counter out of order
    tofbad3 = 0, // too many separators 
    tofbad4 = 0, // DEADFACE wrong 
    tofbad5 = 0, // trigger ctr diff != event ctr diff 
    tofbad6 = 0, // trigger ctr in TDC header and trailer out of sync
    tofbad7 = 0, // token mismatch
    tofbad8 = 0; // tokens don't match evp token
  bool trgCtrOutofsync;

  static char mountpoint[32] ;
  static char rootfile[132];
  int fcount ;
  char *fnames[1000] ;	// up to 1000 files, hope that's enough...
  int i ;
  int dump_det ;
  int check_only ;
  // for TOF:
  u_int expectedMask = 0x3f3;
  int nExpected = 8;
  int oldEventNo;

  // a Root structure for the events:
  const int MAX_TRAY = 256;
  const int MAX_PVPD = 64;
  struct event_t {
    Int_t	nTrayLE;
    Int_t	nTrayTE;
    Int_t	nEastLE;
    Int_t	nEastTE;
    Int_t	nWestLE;
    Int_t	nWestTE;

    UInt_t	trayLE[MAX_TRAY];
    UInt_t	trayTE[MAX_TRAY];
    UInt_t	eastLE[MAX_PVPD];
    UInt_t	eastTE[MAX_PVPD];
    UInt_t	westLE[MAX_PVPD];
    UInt_t	westTE[MAX_PVPD];

    Int_t	trayLEch[MAX_TRAY];
    Int_t	trayTEch[MAX_TRAY];
    Int_t	eastLEch[MAX_PVPD];
    Int_t	eastTEch[MAX_PVPD];
    Int_t	westLEch[MAX_PVPD];
    Int_t	westTEch[MAX_PVPD];
  };

  event_t event;

    

  // parse command line arguments
  // but set the defaults before...

  int evtype = EVP_TYPE_ANY ;	// any
  strcpy(mountpoint,"") ;
  strcpy(rootfile, "/tmp/out.root");

  check_only = 0 ;	// don't _just_ check the headers...

  dump_det = -1 ;	// default is no dump

  bool doRoot = true;

  while((c = getopt(argc,argv,"CRt:m:r:D:s:n:")) != EOF)
    switch(c) {
    case 't' :
      evtype = atoi(optarg) ;		// request type
      break ;
    case 'm' :				// mountpoint for data on evp i.e. /evp
      strncpy(mountpoint,optarg,sizeof(mountpoint)-1) ;
      break ;
    case 'r' :				// mountpoint for data on evp i.e. /evp
      strncpy(rootfile,optarg,sizeof(rootfile)-1) ;
      break ;
    case 'D' :				// dump data for this detector
      dump_det = atoi(optarg) ;	
      break ;
    case 'C' :				// only run sanity checks
      check_only = 1 ;
      break ;
    case 's':
      expectedMask = strtoul(optarg, NULL, 16);
      break;
    case 'n':
      nExpected = atoi(optarg);
      break;
    case 'R':
      doRoot = false;
      break;

    case '?' :
      fprintf(stdout,"Usage: %s [-t type ] [-C] [-R] [-r rootfile ] [-m mountpoint] [-D det_id] [-s separatorMask] [-n nSeparators] <files...>\n",
	      argv[0]) ;
      return -1 ;
      break ;
    }

  printf("sepMask = 0x%x, nsep = %d\n", expectedMask, nExpected);
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
	

    if (DEBUG)
      if(fnames[i]) fprintf(stdout,"Calling constructor [%d/%d], with filename %s\n",(i+1),fcount,fnames[i]) ;
      else fprintf(stdout,"Calling constructor [%d/%d], with filename NULL",(i+1),(fcount+1)) ;

    tofbad1 = tofbad2 = tofbad3 = tofbad4 = tofbad5 = tofbad6 = tofbad7 = tofbad8 = 0;
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



    // for TOF checks:
    bool firstTime = true;
    trgCtrOutofsync = false;
    u_int triggerCtr[10] = {0,0,0,0,0,0,0,0,0,0};
    u_int triggerWrd[3] = {0,0,0};
    int nSeparators;
      

    TFile hfile;
    TTree *tree;

    if (doRoot) {
      // open Root file:
      //TFile hfile(rootfile, "RECREATE","TOF DAQ ROOT file");
      hfile.Open(rootfile, "RECREATE","TOF DAQ ROOT file");
      
      // create a tree:
      //TTree *tree = new TTree("Event", "Event");
      tree = new TTree("Event", "Event");
      tree->Branch("nEvent", &event.nTrayLE, "nTrayLE/i:nTrayTE:nEastLE:nEastTE:nWestLE:nWestTE");
      
      tree->Branch("trayLE", event.trayLE, "trayLE[nTrayLE]/i");
      tree->Branch("trayTE", event.trayTE, "trayTE[nTrayTE]/i");
      tree->Branch("eastLE", event.eastLE, "eastLE[nEastLE]/i");
      tree->Branch("eastTE", event.eastTE, "eastTE[nEastTE]/i");
      tree->Branch("westLE", event.westLE, "eastLE[nWestLE]/i");
      tree->Branch("westTE", event.westTE, "eastTE[nWestTE]/i");
      
      tree->Branch("trayLEch", event.trayLEch, "trayLEch[nTrayLE]/I");
      tree->Branch("trayTEch", event.trayTEch, "trayTEch[nTrayTE]/I");
      tree->Branch("eastLEch", event.eastLEch, "eastLEch[nEastLE]/I");
      tree->Branch("eastTEch", event.eastTEch, "eastTEch[nEastTE]/I");
      tree->Branch("westLEch", event.westLEch, "eastLEch[nWestLE]/I");
      tree->Branch("westTEch", event.westTEch, "eastTEch[nWestTE]/I");
      
    }

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
      if (DEBUG)
	if((good%1000)==0) {
	  fprintf(stdout,"Events processed %d...\n",good) ;
	}

      fprintf(stdout,"\n**** Event %d (seq %d): bytes %d, token %d, trg_cmd %d, FILE %s\n",
	      evp->event_number,evp->seq, evp->bytes,evp->token,evp->trgcmd,evp->file_name) ;

      // make humanly readable time from the UNIX time...
      char *stime ;
      stime = ctime((long int *)&evp->evt_time) ;
      *(stime+strlen(stime)-1) = 0 ;

      if (DEBUG) fprintf(stdout, "     Trigger Word 0x%02X, time %u (%s), daqbits 0x%04X\n",
			 evp->trgword,evp->evt_time,stime,evp->daqbits) ;

      if(check_only) continue ;

      // the return value of the "get" method points to datap so one may
      // use it if one knows what one is doing (unlikely)

      datap = mem ;

      // here the TOF specific stuff starts

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
	if (DEBUG) fprintf(stdout,"TOF: %d bytes\n",ret) ;
	// now do something:
	u_int sep_mask;
	int channel, halftray;
	int tdig, tdc;
	u_int tdcVal;
	bool jsErr = false;
	bool jsErr2 = false;
	bool tokenErr = false;
	bool tokenMismatch = false;
	sep_mask = 0;
	nSeparators = 0;
	bool firstTDCHeader = true;
	bool tdcHeaderError = false;
	u_int TDCHeader;

	// root structure zero-ing:
	if(doRoot) {
	  event.nTrayLE = event.nTrayTE = event.nEastLE = 
	    event.nEastTE = event.nWestLE = event.nWestTE = 0;
	}

	for (int ii = 0; ii<4; ii++) { // 4 DDLR banks
	  halftray = 0;
	  if (DEBUG) fprintf(stdout,"TOFDDLR %d: length %d words\n",ii+1, tof.ddl_words[ii]);
	  if (tof.ddl_words[ii] != 0) {
	    for(int jj=0; jj<tof.ddl_words[ii]; jj++) {
	      u_int data_word = l2h32(tof.ddl_word_p[ii][jj]); 	// tof.ddl_word_p[ii] points to the DDLR ii data
	      if (jj == 2) { // this should be deadface
		if (dump_det == 2) fprintf(stdout, "DDLR %d: 0x%08X\n",ii+1,data_word) ;
		if (data_word != 0xdeadface) { 
		  fprintf(stdout, "\t*****JS4:ERROR: 0x%08x should be 0xDEADFACE\n", data_word); 
		  tofbad4++;
		}
	      }
	      else if ((data_word & 0xFFF00000) == 0xA0000000) {	// header trigger word
		fprintf(stdout, "DDLR %d: 0x%08X\n",ii+1,data_word) ;
		triggerWrd[ii] = data_word;
		if (ii != 0) {
		  if (data_word != triggerWrd[ii-1]) tokenErr = true;
		}
		if ((data_word & 0x00000fff) !=  evp->token) tokenMismatch = true;
	      }
	      else if ((data_word & 0xFF000000) == 0xB0000000)	// header DEBUG word
		fprintf(stdout, "DDLR %d: 0x%08X\n",ii+1,data_word) ;
	      else if ((data_word & 0xF8FF0000) == 0xE0000000) {	// TDIG separator word
		nSeparators++;
		fprintf(stdout, "DDLR %d: 0x%08X\n",ii+1,data_word) ;
		if (ii == 1) tdig = 8;
		else if (ii == 2) tdig = 9;
		else tdig = (data_word & 0x0F000000) >> 24;
		sep_mask |= (1<<tdig);
		data_word &= 0x000000FF;
		if(DEBUG) fprintf(stdout, "\ttrgCtr new = 0x%x, old = 0x%x\n", data_word, triggerCtr[tdig]);
#ifdef NOTNOW // don't look for this right now, events might be out of order
		if (!firstTime) {
		  if (data_word != ((triggerCtr[tdig]+1)&0xFF)) {
		    fprintf(stdout, "\t*****JS2:ERROR: trgCtr new = 0x%x, old = 0x%x ******\n", 
			    data_word, triggerCtr[tdig]);
		    jsErr = true;
		  }
		}
#endif
		//if (evp->event_number == 1) { // the first event analyzed!
		if (!trgCtrOutofsync) { // counters not yet out of sync
		  if (tdig != 1) {
		    if (data_word != triggerCtr[1]) {
		      fprintf(stdout, "\t*****JSSYNC:ERROR: trgCtr out of sync: 0x%x, TDIG 1 = 0x%x ******\n", 
			      data_word, triggerCtr[1]);
		      trgCtrOutofsync = true;
		    }
		  }
		}
#ifdef NOTNOW	// don't look for this now, since we are running with aborts
		if (!firstTime) {
		  if (data_word != ((triggerCtr[tdig]+evp->seq-oldEventNo)&0xFF)) {
		    fprintf(stdout, "\t*****JS5:ERROR: trgCtr new = 0x%x, old = 0x%x  evnt_diff 0x%x ******\n", 
			    data_word, triggerCtr[tdig], evp->seq-oldEventNo);
		    jsErr2 = true;
		  }
		}
#endif

		triggerCtr[tdig] = data_word;
	      }
	      else if ((data_word & 0xFFFFFF00) == 0xC0000000) {	// geographical word
		halftray = data_word & 0x1;
		if (dump_det == 2) fprintf(stdout, "DDLR %d: 0x%08X\n",ii+1,data_word) ;
	      }
	      else if (data_word == 0xEA000000) {	// end-of-event separator
		if (dump_det == 2) fprintf(stdout, "DDLR %d: 0x%08X\n",ii+1,data_word) ;
	      }
	      else if ((data_word & 0x80000000) == 0x80000000) {	
		// this word has the MSB=1, but fits none of the above defined words, 
		// and is also not an HPTDC word
		if (dump_det == 2) fprintf(stdout, "DDLR %d: 0x%08X *** unknown ID ***\n",ii+1,data_word) ;
	      }
	      else if ((data_word & 0xE0000000) == 0x20000000) {	// TDC header or trailer
		fprintf(stdout, "DDLR %d: 0x%08X\n",ii+1,data_word) ;
		if (firstTDCHeader) {
		  TDCHeader = data_word & 0x00FFF000;
		  firstTDCHeader = false;
		}
		else {
		  if ( (data_word & 0x00FFF000) != TDCHeader) { // trigger counter in TDC Header or trailer is wrong
		    fprintf(stdout, "\t*****JS6:ERROR: trgCtr in TDC header/trailer out of sync ******\n");
		    tdcHeaderError = true;
		  }
		}
	      }
	      else {	// this should be all the HPTDC words (anything with the MSB=0)
		if (dump_det == 2) {
		  fprintf(stdout, "DDLR %d: 0x%08X", ii+1,data_word) ;
		  tdc = (data_word & 0x0F000000) >> 24;
		  if ((data_word & 0xF0000000) == 0x40000000) {	// leading edge, 25ps
		    channel = ((data_word & 0x00E00000) >> 21) + (tdc&0x3)*8 + (tdc>>2)*24 + halftray*96;
		    tdcVal =  ((data_word & 0x00180000) >> 19) + ((data_word & 0x0007FFFF) << 2);
		    fprintf(stdout, " le: ch %3d t 0x%08x", channel, tdcVal);
		    if (doRoot) {
		      if (ii == 0) {
			if (event.nTrayLE < MAX_TRAY) {
			  event.trayLE[event.nTrayLE] = tdcVal;
			  event.trayLEch[event.nTrayLE++] = channel;
			}
		      }
		      else if (ii == 2) { // West
			if (event.nWestLE < MAX_PVPD) {
			  event.westLE[event.nWestLE] = tdcVal;
			  event.westLEch[event.nWestLE++] = channel;
			}
		      }
		      else if (ii == 1) { // East
			if (event.nEastLE < MAX_PVPD) {
			  event.eastLE[event.nEastLE] = tdcVal;
			  event.eastLEch[event.nEastLE++] = channel;
			}
		      }
		    } // if doRoot
		  }
		  else if ((data_word & 0xF0000000) == 0x50000000) {	// trailing edge, 100ps
		    channel = ((data_word & 0x00F80000) >> 19) + (tdc>>2)*24 + halftray*96;
		    tdcVal =  (data_word & 0x0007FFFF)<<2;
		    fprintf(stdout, " te: ch %3d t 0x%08x", channel, tdcVal);
		    if (doRoot) {
		      if (ii == 0) {
			if (event.nTrayTE < MAX_TRAY) {
			  event.trayTE[event.nTrayTE] = tdcVal;
			  event.trayTEch[event.nTrayTE++] = channel;
			}
		      }
		      else if (ii == 2) { // West
			if (event.nWestLE < MAX_PVPD) {
			  event.westTE[event.nWestTE] = tdcVal;
			  event.westTEch[event.nWestTE++] = channel;
			}
		      }
		      else if (ii == 1) { // East
			if (event.nEastLE < MAX_PVPD) {
			  event.eastTE[event.nEastTE] = tdcVal;
			  event.eastTEch[event.nEastTE++] = channel;
			}
		      }
		    } // if doRoot
		  }
		  fprintf(stdout, "\n");
		}
	      }
	    }
	  } // if (tof.ddl_words[ii] != 0)
	} // for (int ii = ....


	if (tokenErr) {
	  fprintf(stdout, "\t***JS7:ERROR: tokens don't match: 0x%x, 0x%x, 0x%x\n",
		  triggerWrd[0], triggerWrd[1], triggerWrd[2]);
	  tofbad7++;
	}
	if (tokenMismatch) {
	  fprintf(stdout, "\t***JS8:ERROR: token(s) don't match event token: evp 0x%x, TOF 0x%x, 0x%x, 0x%x\n",
		  evp->token, triggerWrd[0], triggerWrd[1], triggerWrd[2]);
	  tofbad8++;
	}
	oldEventNo =  evp->seq;
	if (firstTime) firstTime = false;
	if (tdcHeaderError) tofbad6++;
	if (sep_mask != expectedMask) {
	  fprintf(stdout, "\t*****JS1:ERROR: Separator(s) missing (0x%03x); size DDLR 1: %d, 3: %d\n", 
		  sep_mask, tof.ddl_words[0], tof.ddl_words[2]);
	  firstTime = true;
	  tofbad1++;
	  continue;
	}
	if (jsErr) {
	  firstTime = true;
	  tofbad2++;
	  continue;
	}
	if (jsErr2) {
	  firstTime = true;
	  tofbad5++;
	  continue;
	}
	if (nSeparators > nExpected) {
	  fprintf(stdout, "\t***JS3:ERROR: number of separators: %d\n", nSeparators);
	  tofbad3++;
	  firstTime = true;
	  continue;
	}
	// print out the structure for tray leading edge:
	//printf("nTrayLE %d nTrayTE %d\n", event.nTrayLE, event.nTrayTE);
	//for (int kk = 0; kk<event.nTrayLE; kk++)
	//  printf("ch %d tdc 0x%x\n", event.trayLEch[kk], event.trayLE[kk]);

	if (doRoot) {
	  tree->Fill();
	}
	
      } // else
      
    }	// end of EVENT LOOP


    fprintf(stdout,"Processed %s: %d good, %d bad events total...\n",fnames[i],good,bad) ;
    fprintf(stdout,"%d bad1, %d bad2, %d bad3 %d bad4 %d bad5 %d bad6 %d bad7 %d bad8 events with TOF problems\n", 
	    tofbad1, tofbad2, tofbad3, tofbad4, tofbad5, tofbad6, tofbad7, tofbad8); 
    if (trgCtrOutofsync) fprintf(stdout, "Trigger Counters in separators out of sync\n");

    delete evp ;
    
    // close Root file:
    if (doRoot) {
      hfile.Write();
      hfile.Close();
    }

  }	// end of input file loop

  return 0 ;
}

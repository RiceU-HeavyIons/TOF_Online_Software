#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <stdlib.h>

#include <rtsLog.h>	// for my LOG() call
#include <rtsSystems.h>

// this needs to be always included
#include <DAQ_READER/daqReader.h>
#include <DAQ_READER/daq_dta.h>

// only the detectors we will use need to be included
// for their structure definitions...
#include <DAQ_BSMD/daq_bsmd.h>
#include <DAQ_BTOW/daq_btow.h>
#include <DAQ_EMC/daq_emc.h>
#include <DAQ_ESMD/daq_esmd.h>
#include <DAQ_ETOW/daq_etow.h>
#include <DAQ_FPD/daq_fpd.h>
#include <DAQ_FTP/daq_ftp.h>
#include <DAQ_L3/daq_l3.h>
#include <DAQ_PMD/daq_pmd.h>
#include <DAQ_PP2PP/daq_pp2pp.h>
#include <DAQ_RIC/daq_ric.h>
#include <DAQ_SC/daq_sc.h>
#include <DAQ_SSD/daq_ssd.h>
#include <DAQ_SVT/daq_svt.h>
#include <DAQ_TOF/daq_tof.h>
#include <DAQ_TPC/daq_tpc.h>
#include <DAQ_TPX/daq_tpx.h>
#include <DAQ_TRG/daq_trg.h>
#include <DAQ_HLT/daq_hlt.h>
#include <DAQ_FGT/daq_fgt.h>
#include <DAQ_MTD/daq_mtd.h>

#define RTS_DISABLE_LOG

// I wrapped more complicated detectors inside their own functions
// for this example
static int trg_doer(daqReader *rdr, const char *do_print) ;
static int mtd_doer(daqReader *rdr, const char *do_print) ;

int main(int argc, char *argv[])
{
  extern char *optarg ;
  extern int optind ;
  int c ;
  const char *print_det = "" ;
  char _mountpoint[256];
  char *mountpoint = NULL;

  printf("Jo's version\n");
  //rtsLogOutput(RTS_LOG_STDERR) ;
  //rtsLogLevel((char *)WARN) ;


  while((c = getopt(argc, argv, "D:d:m:h")) != EOF) {
    switch(c) {
    case 'd' :
      rtsLogLevel(optarg) ;
      break ;
    case 'D' :
      print_det = optarg ;
      break ;
    case 'm' :
      mountpoint = _mountpoint;
      strcpy(mountpoint, optarg);
      break;
		  
    default :
      break ;
    }
  }

  class daqReader *evp ;			// tha main guy
  evp = new daqReader(argv[optind]) ;	// create it with the filename argument..
  if(mountpoint) {
    evp->setEvpDisk(mountpoint);
  }

  int good=0;
  int bad=0;
  int tofevt=0;
	
  for(;;) {
    char *ret = evp->get(0,EVP_TYPE_ANY);
       
    if(ret) {
      if(evp->status) {
	//LOG(ERR,"evp status is non-null [0x08X, %d dec]",evp->status,evp->status) ;
	printf("evp status is non-null [0x%08X, %d dec]\n",evp->status,evp->status) ;
	continue ;
      }
      good++;
    }
    else {    // something other than an event, check what.
      switch(evp->status) {
      case EVP_STAT_OK:   // just a burp!
	continue;
      case EVP_STAT_EOR:
	LOG(DBG, "End of Run/File");
	if(evp->IsEvp()) {   // but event pool, keep trying...
	  LOG(DBG, "Wait a second...");
	  sleep(1);
	  continue;
	}
	break;        // file, we're done...
      case EVP_STAT_EVT:
	bad++;
	//LOG(WARN, "Problem getting event - skipping [good %d, bad %d]",good,bad);
	printf( "Problem getting event - skipping [good %d, bad %d]\n",good,bad);
	sleep(1);
	continue;
      case EVP_STAT_CRIT:
	//LOG(CRIT,"evp->status CRITICAL (?)") ;
	printf("evp->status CRITICAL (?)\n") ;
	return -1;
      }
    }

		
    if(evp->status == EVP_STAT_EOR) {
      LOG(INFO,"End of File reached...") ;
      break ;	// of the for() loop...
    }

    daq_dta *dd ;	// generic data pointer; reused all the time


    //    LOG(INFO,"sequence %d: token %4d, trgcmd %2d, daqcmd %2d, time %u, detectors 0x%08X (status 0x%X)",
    //    evp->seq, evp->token, evp->trgcmd, evp->daqcmd,
    //evp->evt_time, evp->detectors, evp->status) ;
    printf("File name \"%s\": sequence %d: token %4d, trgcmd 0x%X, daqcmd 0x%X (evp status 0x%X)\n",
	   evp->file_name, evp->seq, evp->token, evp->trgcmd, evp->daqcmd,evp->status) ;


    //if(print_det[0]) printf("***** Seq #%d, token %d\n",evp->seq,evp->token) ;
    /***************** let's do simple detectors; the ones which only have legacy *****/

    if(print_det[0]) {
      if(strcmp(print_det, "tinfo") == 0) {		    
	printf("trginfo: seq = #%d  token = %d detectors = 0x%x triggers = 0x%x  evptriggers=0x%x/0x%x\n",
	       evp->seq,
	       evp->token,
	       evp->detectors,
	       evp->daqbits,
	       evp->evpgroups,
	       evp->evpgroupsinrun);
      }
    }

    if(print_det[0]) {
      if(strcmp(print_det, "readahead") == 0) {		    
	SummaryInfo nsummary;
	int ret = evp->readNextFutureSummaryInfo(&nsummary);
		   
		   
	if(ret <= 0) {
	  printf("Event #%d, token %d triggers 0x%x  ---->   No Next Event...",
		 evp->seq,evp->token,evp->daqbits);
	}
	else {
	  printf("Event #%d, token %d triggers 0x%x   ---->   Next Event:  #%d, token %d triggers 0x%x\n",
		 evp->seq,evp->token,evp->daqbits,
		 nsummary.seq, nsummary.token, nsummary.daqbits);
	}
      }
    }

#ifdef NOTNOW
    dd = evp->det("tof")->get("raw",0); //"sector" = 0, ignored
    if(dd) {
      printf("TOF found\n") ;
    
      while(dd->iterate()) { // iterates over "rows" (RDOs) from 1 to 4
	printf("TOF: sec %d, RDO %d, %4d bytes\n",dd->sec,dd->row,dd->ncontent) ;
	for (int ii=0; ii<(int)(dd->ncontent/4); ii++) {
	  printf("\t%4d: 0x%08X\n",ii,dd->Int32[ii]) ;
	}
      }
    }
#endif

    dd = evp->det("tof")->get("raw") ;
    if(dd) {
      printf("TOF found\n") ;
      tofevt++;
      if(strcasecmp(print_det,"tof")==0) {
	//JS: start TOF analysis here
	unsigned int parity = 0;
	unsigned int phaseNW = 0;
	while(dd->iterate()) {
	  int tray = 0;
	  int r = dd->row;
	  bool find0x2 = false;
	  bool find0xe = false;
	  bool firstHeader = true;
	  unsigned int firstPhase = 0;
	  printf("RDO %d: words %d:\n",r,dd->ncontent/4);
	  unsigned int prevWord = 0xffffffff;
	  unsigned int item_count = 0;
	  int trayhalf_count = 0;
	  for(u_int i=0;i<(dd->ncontent/4);i++) {
	    item_count++;
	    printf("\t%4d: 0x%08X",i,dd->Int32[i]);

	    // ****************** repeated word ***************************
	    if (dd->Int32[i] == prevWord) {
	      printf(" *** repeat word?");
	    }
	    prevWord = dd->Int32[i];

	    // ****************** geographical word ***********************
	    if((dd->Int32[i] & 0xF0000000) == 0xC0000000) {
	      item_count = 0;
	      printf(" - tray %d-%d", 
		     (dd->Int32[i] & 0xFF)/2, 
		     ((dd->Int32[i] & 0x1) == 1) ? 1 : 0);
	      if((dd->Int32[i] & 0xFFFFFF00) != 0xC0000000) {
		printf(" *** bit error");
	      }
	      if((dd->Int32[i] & 0x1) == parity) {
		printf(" *** tray half missing?");
	      }
	      parity = dd->Int32[i] & 0x1;
	      if (find0xe) {
		printf(" *** separator missing");
	      }
	      find0x2 = true;
	      find0xe = true;
	      tray = (int)(dd->Int32[i] & 0xFF)/2;
	      trayhalf_count++; 
	    }

	    // ****************** TDC local header word ***************************
	    else if((dd->Int32[i] & 0xF0000000) == 0x20000000) {
	      if (firstHeader) {
		if(r == 1) {
		  firstPhase  = (dd->Int32[i]) & 0xFFF;
		  phaseNW = firstPhase + 0x1000;
		}
		else {
		  firstPhase  = (dd->Int32[i]) & 0xFFF;
		  printf(" - phase%d %d", r, (phaseNW - firstPhase) & 0xFFF);
		}
		firstHeader = false;
	      }
	      else if ((tray != 121) && (tray != 122)) {
		// start detectors have different phase, don't check for now
		if (((dd->Int32[i]) & 0xFFF) != firstPhase)  {
		  printf(" *** tray %d wrong phase 0x%x (should be 0x%x)", tray,
			 dd->Int32[i] & 0xFFF, 
			 firstPhase);

		}
	      }
	      if (!find0x2 && (dd->Int32[i] & 0xFF000000) == 0x20000000) // first header 
		printf(" *** geographical word missing");
	      find0x2 = false;
	    }

	    // *********************** separator word *****************************
	    else if((dd->Int32[i] & 0xF0000000) == 0xe0000000) {
	      find0xe = false;
	      if (find0x2) {
		printf(" *** header  missing");
		find0x2 = false;
	      }
	      else {
		unsigned int reported_count;
		reported_count = (dd->Int32[i] & 0x3ff00) >> 8;
		if ((reported_count == 1022) && (item_count == 1021)) {
		}
		else if (reported_count != --item_count) {
		  printf(" *** item count incorrect: reported %d, actual count %d",
			 reported_count, item_count);
		}
	      }
	      // reset item_count
	      item_count = 0;
	    }

	    // *************************** trigger word ***************************
	    else if((dd->Int32[i] & 0xF0000000) == 0xA0000000) {
	      item_count = 0;
	      if(parity == 1) {
		printf(" *** tray half missing?");
	      }
	      parity = 0;
	      if (find0xe) {
		printf(" *** separator missing");
	      }
	      if (find0x2) {
		printf(" *** header missing");
	      }
	      find0xe = find0x2 = false;
	    }

	    // ***************************** tag word *****************************
	    else if(dd->Int32[i] == 0xDEADFACE) {
	      item_count = 0;
	    }

	    // ************************* HPTDC data word *************************
	    else if(((dd->Int32[i] & 0xF0000000) == 0x40000000) ||
		    ((dd->Int32[i] & 0xF0000000) == 0x50000000)) {
	    }

	    // ******************* HPTDC Error word ******************************
	    else if((dd->Int32[i] & 0xF0000000) == 0x60000000) {
	      printf(" *** HPTDC Error");
	    }

	    // ********************* unexpected data word ************************
	    else {
	      printf(" *** unexpected data word!!!!");
	    }
	    printf("\n");
	  }
	  printf("\t");
	  if ( (r == 1) && (trayhalf_count != 62))
	    printf("*** ");
	  else if ( (r == 2) && (trayhalf_count != 62))
	    printf("*** ");
	  else if ( (r == 3) && (trayhalf_count != 60))
	    printf("*** ");
	    //else if ( (r == 4) && (trayhalf_count != 58)) // run 12 has tray 102 disabled
	  else if ( (r == 4) && (trayhalf_count != 60))
	    printf("*** ");
// 	  else if ( (r != 4) && (trayhalf_count != 60) && (trayhalf_count != 62) )
// 	    printf("*** ");
	  printf("RDO %d tray half count = %d\n", r, trayhalf_count);
	}
	//JSend: end of TOF analysis
	  

	//} // while (dd->iterate)
      }
    } // if (dd)

    if(trg_doer(evp, print_det)) LOG(INFO,"TRG found") ;
		
    if(mtd_doer(evp,print_det)) LOG(INFO,"MTD found") ;
		

  }

  delete evp ;	// cleanup i.e. if running through a set of files.

  printf("\n\n\n");
  printf("*** Found %d good events, %d of these with TOF\n", good, tofevt);

  return 0 ;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////// TRIGGER /////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
static int trg_doer(daqReader *rdr, const char  *do_print)
{
  int found = 0 ;
  daq_dta *dd ;

  if(strcasestr(do_print,"trg")) ;
  else do_print = 0 ;

  // If i want decoded data ala old evpReader I call "legacy" ;

  dd = rdr->det("trg")->get("legacy") ;
  if(dd) {
    if(dd->iterate()) {
      trg_t *trg_p = (trg_t *) dd->Void ;

      if(do_print) {	// print something...
	printf("Trigger: tcubits 0x%08X, trg_word 0x%04X\n",trg_p->tcubits,trg_p->trg_word) ;
      }

    }
  }


  // if you need the void black box of untouched trigger data:
  dd = rdr->det("trg")->get("raw") ;
  if(dd) {
    if(dd->iterate()) {
      found = 1 ;


      u_char *trg_raw = dd->Byte;
			
      if(do_print) {	// I have no clue but let me print first few words...


	// simple start of trig desc; the way it should be...
	struct simple_desc {
	  short len ;
	  char evt_desc ;
	  char ver ;
	} *desc ;

	desc = (simple_desc *) trg_raw ;


	printf("Trigger: raw bank has %d bytes: ver 0x%02X, desc %d, len %d\n",
	       dd->ncontent,desc->ver, desc->evt_desc, desc->len) ;

	// dump first 10 ints...
	u_int *i32 = (u_int *) trg_raw ;
	for(int i=0;i<10;i++) {
	  printf("Trigger: word %d: 0x%08X\n",i,i32[i]) ;
	}
      }
    }
  }

  return found ;
}

///////////////////////////////////////////////////////////
///////////////////// MTD /////////////////////////////////
///////////////////////////////////////////////////////////
static int mtd_doer(daqReader *rdr, const char *do_print)
{
  int found = 0 ;
  daq_dta *dd ;

  if(strcasestr(do_print,"mtd")) ;	// leave as is...
  else do_print = 0 ;


  // right now only the "raw" pointer is available/known
  dd = rdr->det("mtd")->get("raw") ;
  if(dd) {
    printf("MTD found\n");
    while(dd->iterate()) {
      found = 1 ;

      // point to the start of the DDL raw data
      u_int *d = (u_int *) dd->Int32 ;	

#ifdef NOTNOW
      if(do_print) {
	printf("MTD: RDO %d: %d bytes\n", dd->rdo, dd->ncontent) ;
	// dump all
	for(int i=0;i<(int)(dd->ncontent/4);i++) {
	  printf(" %2d: 0x%08X\n",i,d[i]) ;
	}
      }
#endif

      if(do_print) {
	//JS: start MTD analysis here (same as TOF for now)
	unsigned int parity = 0;
	bool find0x2 = false;
	bool find0xe = false;
	//bool firstHeader = true;
	unsigned int prevWord = 0xffffffff;
	//unsigned int firstPhase = 0;
	unsigned int item_count = 0;
	for(int i=0;i<(int)(dd->ncontent/4);i++) {
	  item_count++;
	  printf("\t%4d: 0x%08X",i,d[i]) ;

	  // ****************** repeated word ***************************
	  if (d[i] == prevWord) {
	    printf(" *** repeat word?");
	  }
	  prevWord = d[i];

	  // ****************** geographical word ***********************
	  if((d[i] & 0xF0000000) == 0xC0000000) {
	    item_count = 0;
	    printf(" - tray %d-%d", 
		   (d[i] & 0xFF)/2, 
		   ((d[i] & 0x1) == 1) ? 1 : 0);
	    if((d[i] & 0xFFFFFF00) != 0xC0000000) {
	      printf(" *** bit error");
	    }
	    if((d[i] & 0x1) == parity) {
	      printf(" *** tray half missing?");
	    }
	    parity = d[i] & 0x1;
	    if (find0xe) {
	      printf(" *** separator missing");
	    }
	    find0x2 = true;
	    find0xe = true;
	  }

	  // ****************** TDC local header word ***************************
	  else if((d[i] & 0xF0000000) == 0x20000000) {
	    //if (firstHeader) {
	    //if(r == 0) {
	    //firstPhase  = (d[i]) & 0xFFF;
	    //phaseNW = firstPhase + 0x1000;
	    //}
	    //else {
	    //firstPhase  = (d[i]) & 0xFFF;
	    //printf(" - phase%d %d", r, (phaseNW - firstPhase) & 0xFFF);
	    //}
	    //firstHeader = false;
	    //}
	    //else if (((d[i]) & 0xFFF) != firstPhase)  {
	    //  printf(" *** wrong phase 0x%x (should be 0x%x)", d[i] & 0xFFF, 
	    //firstPhase);
	    //
	    //}
			      
	    if (!find0x2 && (d[i] & 0xFF000000) == 0x20000000) // first header 
	      printf(" *** geographical word missing");
	    find0x2 = false;
	  }

	  // *********************** separator word *****************************
	  else if((d[i] & 0xF0000000) == 0xe0000000) {
	    find0xe = false;
	    if (find0x2) {
	      printf(" *** header  missing");
	      find0x2 = false;
	    }
	    else {
	      unsigned int reported_count;
	      reported_count = (d[i] & 0x3ff00) >> 8;
	      if ((reported_count == 1022) && (item_count == 1021)) {
	      }
	      else if (reported_count != --item_count) {
		printf(" *** item count incorrect: reported %d, actual count %d",
		       reported_count, item_count);
	      }
	    }
	    // reset item_count
	    item_count = 0;
	  }

	  // *************************** trigger word ***************************
	  else if((d[i] & 0xF0000000) == 0xA0000000) {
	    item_count = 0;
	    if(parity == 1) {
	      printf(" *** tray half missing?");
	    }
	    parity = 0;
	    if (find0xe) {
	      printf(" *** separator missing");
	    }
	    if (find0x2) {
	      printf(" *** header missing");
	    }
	    find0xe = find0x2 = false;
	  }

	  // ***************************** tag word *****************************
	  else if(d[i] == 0xDEADFACE) {
	    item_count = 0;
	  }

	  // ************************* HPTDC data word *************************
	  else if(((d[i] & 0xF0000000) == 0x40000000) ||
		  ((d[i] & 0xF0000000) == 0x50000000)) {
	  }

	  // ******************* HPTDC Error word ******************************
	  else if((d[i] & 0xF0000000) == 0x60000000) {
	    printf(" *** HPTDC Error");
	  }

	  // ********************* unexpected data word ************************
	  else {
	    printf(" *** unexpected data word!!!!");
	  }
	  printf("\n");
	}
	//JSend: end MTD analysis

      } //if(do_print)


#ifdef NOTNOW
      //JS: start MTD analysis (for later, once the MTD pointer is known)
      mtd_t *mtd = (mtd_t *)dd->Void;

      for(int r=0;r<2;r++) {
	printf("RDO %d: words %d:\n",r+1,mtd->ddl_words[r]) ;
      }
      //JSend: end MTD analysis

#endif // NOTNOW
      
    }
  }

  return found ;
}


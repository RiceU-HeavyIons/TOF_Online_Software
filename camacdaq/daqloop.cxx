#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: daqloop.cxx,v 1.1.1.1 2003-07-03 18:34:51 jschamba Exp $";
#endif /* lint */

/* File name     : daqloop.cxx
 * Creation date : 4/15/02
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

int hdaq();

#ifndef __CINT__
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TMapFile.h"

/* System headers */
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream.h>
#include <time.h>
#include <sys/stat.h>

/* Local headers */
#include "camlib.h"

/* Macros */
#define FIFO_FILE "/tmp/daqfifo"

#define  ADC_2249 1
#define  MASK_2249 (1<<(ADC_2249-1))

#define TDC_2228 6

const int N_ADC = 1;
const int N_TDC = 1;
const int N_ADC_CHANNEL = N_ADC*12;
const int N_TDC_CHANNEL = N_TDC*8;

/* Functions */
void error_exit( int status )
{
    perror(strerror(status));
    _exit(status);
}

// set default actions
char *filename="tofr.root";
long numberofevents=10;
bool verbose=false;
bool doBlockRead=false;
bool globalPAW=true;

//______________________________________________________________________________
int main(int argc, char *argv[])
{

  // process commandline
  if (argc>1){
    for (int i=1;i<(argc);i++){
      if (!strcmp(argv[i],"-h") || !strcmp(argv[i],"--help")){
        cout << "Usage:  " << argv[0] <<
          "  [-h|--help] [-v] [-b|-B] [-g|-G] [-f filename] [-n nevents] " << endl;
        return 0;
      }
      if (!strcmp(argv[i],"-v")) verbose=true;
      if (!strcmp(argv[i],"-f")) filename=argv[++i];
      if (!strcmp(argv[i],"-n")) numberofevents=atol(argv[++i]);
      if (!strcmp(argv[i],"-b")) doBlockRead=true;
      if (!strcmp(argv[i],"-B")) doBlockRead=false;
      if (!strcmp(argv[i],"-g")) globalPAW=true;
      if (!strcmp(argv[i],"-G")) globalPAW=false;
    }
  }

  
  return hdaq();
}
#endif

int hdaq()
{
  int fifofd;
  FILE *fifofp;
  char tempstr[80], tempstr2[80];
  int status, lamstatus, data, q, x;
  short unsigned int sdata;
  struct event_t {
    UShort_t fadc[N_ADC_CHANNEL];
    UShort_t ftdc[N_TDC_CHANNEL];
    Int_t evno;
    Int_t timestamp;
  };
  event_t event;
  time_t *thetime = new time_t;
  struct tm *tm;

  /* Initialization part */
  if( (status = CAM_Open( )) != 0 ) 	error_exit(status);
  if( (status = CSETBR(0)) != 0 ) 	error_exit(status);
  if( (status = CSETCR(0)) != 0 ) 	error_exit(status);
  if( (status = CGENZ()) != 0 ) 	error_exit(status);
  if( (status = CGENC()) != 0 ) 	error_exit(status);
  if( (status = CSETI()) != 0 ) 	error_exit(status);

  data = 0;

  /* Initialize CAMAC modules */

  /* 2249: Clear module and LAM */
  if( (status = CAMAC(NAF(ADC_2249, 0, 9), &data, &q, &x)) ) error_exit(status);
  printf("ADC F9: q=%d, x=%d\n", q, x);  
  /* 2249: Enable LAM */
  if( (status = CAMAC(NAF(ADC_2249, 0, 26), &data, &q, &x)) ) error_exit(status);
  printf("ADC F26: q=%d, x=%d\n", q, x);  
  
  /* 2228: Disable LAM */
  if( (status = CAMAC(NAF(TDC_2228, 0, 24), &data, &q, &x)) ) error_exit(status);
  printf("TDC F24: q=%d, x=%d\n", q, x);  
  /* 2228: Clear module and LAM */
  if( (status = CAMAC(NAF(TDC_2228, 0, 9), &data, &q, &x)) ) error_exit(status);
  printf("TDC F9: q=%d, x=%d\n", q, x);  

  // enable crate controller LAM on ADC2249 module slot
  if( (status = CENLAM (MASK_2249)) ) error_exit(status);
  printf("enabled lam, will now wait for LAM...\n");


  TMapFile::SetMapAddress(0x411aa000);
  TMapFile *mfile = TMapFile::Create("hsimple.map","RECREATE", 100000,
                            "Memory mapped file with histograms");
  // Create some histograms and an ntuple
  TH1F *hadc[N_ADC_CHANNEL]; // ADC histos
  for (int i=0; i<N_ADC_CHANNEL; i++) {
    sprintf(tempstr, "hadc%d", i);
    sprintf(tempstr2, "ADC %d distribution", i);
    hadc[i] = new TH1F(tempstr, tempstr2, 1024,-0.5,1023.5);
  }

  TH1F *htdc[N_TDC_CHANNEL]; // TDC histos
  for (int i=0; i<N_TDC_CHANNEL; i++) {
    sprintf(tempstr, "htdc%d", i);
    sprintf(tempstr2, "TDC %d distribution", i);
    htdc[i] = new TH1F(tempstr, tempstr2, 2048,-0.5,2047.5);
  }

  mfile->Print();

  // Create a new ROOT binary machine independent file.
  // This file is now becoming the current directory.
  TFile hfile(filename,"RECREATE","DAQ ROOT file with histograms");

  //TNtuple *ntuple = new TNtuple("ntuple","Demo ntuple","px:py:pz:random:i");
  TTree *tree = new TTree("Event", "Event");
  sprintf(tempstr, "fadc[%d]/s:ftdc[%d]:evno/I:timestamp", N_ADC_CHANNEL, N_TDC_CHANNEL);
  tree->Branch("nEvent", &event, tempstr);

  // control FIFO
  umask(0);
  mknod(FIFO_FILE, S_IFIFO|0666, 0);
  fifofd = open(FIFO_FILE, O_RDONLY | O_NONBLOCK);
  fifofp = fdopen(fifofd, "r");

  for ( Int_t ev=0; ev<numberofevents; ev++) {
    char *result = fgets(tempstr, 80, fifofp);
    if (result != NULL) {
      if (strncmp(tempstr, "e", 1) == 0) {
	printf("End command received\n");
	fclose(fifofp);
	break;
      }
    }
    /* Remove Inhibit */
    if( (status = CREMI()) ) 	error_exit(status);
    // wait for trigger
    while( (lamstatus = CWTLAM(1000)) != 0 ) {
      printf("CWTLAM returned with status = %d: timeout\n", status);
      result = fgets(tempstr, 80, fifofp);
      if (result != NULL) {
	if (strncmp(tempstr, "e", 1) == 0) {
	  printf("End command received\n");
	  fclose(fifofp);
	  break;
	}
      }
    }
    
    // set inhibit to prevent further triggers coming in
    if( (status = CSETI()) != 0 ) 	error_exit(status);

    // check that we actually got a LAM
    if (lamstatus != 0) break;

    //printf("Received LAM, now reading out...\n");
    printf("------ event %d ---------\r", ev);
    fflush(stdout);

    // record the timestamp...
    time(thetime);
    tm = localtime(thetime);
    event.timestamp = 86400*tm->tm_yday + 3600*tm->tm_hour 
      + 60*tm->tm_min + tm->tm_sec;

    // ... and event number
    event.evno = ev;

    // readout ADC's
    for (int i=0; i<N_ADC; i++) {
      int N = ADC_2249+i;
      for (int A=0; A<12; A++) {
	CAMACW(NAF(N, A, 2), &sdata, &q, &x); // A=11 should clear module & LAM
	//printf("Q=%d, X=%d, data = %d\n", q, x, sdata);
	event.fadc[i*12+A] = (int)sdata;
	// fill histo and tree with structure data
	hadc[i*12+A]->Fill(sdata);
      }
    }
    // finished reading ADC's
    

    // readout TDC's
    for (int i=0; i<N_TDC; i++) {
      int N = TDC_2228+i;
      for (int A=0; A<8; A++) {
	CAMACW(NAF(N, A, 2), &sdata, &q, &x); // A=11 should clear module & LAM
	//printf("Q=%d, X=%d, data = %d\n", q, x, sdata);
	event.ftdc[i*8+A] = (int)sdata;
	htdc[i*8+A]->Fill(sdata);
      }
    }
    // finished reading TDC's

    mfile->Update();
    //mfile->ls();

    tree->Fill();
  }

  // Save all objects in this file
  printf("Closing root binary file...\n");
  hfile.Write();

  // Close the file. Note that this is automatically done when you leave
  // the application.
  hfile.Close();

  // close CAMAC device
  CCLOSE();

  return 0;
}

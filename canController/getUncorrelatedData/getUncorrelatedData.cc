/* File name     : getUncorrelatedData.cc
 * Creation date : 8/20/04
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: getUncorrelatedData.cc,v 1.2 2004-08-23 16:17:05 jschamba Exp $";
#endif /* lint */

//#define LOCAL_DEBUG

//****************************************************************************
// INCLUDES
// C++ header file
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

// other headers
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <libpcan.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>

//****************************************************************************
// DEFINES
#define TDIG

//****************************************************************************
// GLOBALS
HANDLE h = NULL;
FILE *fp[24];

ofstream dataFile[24];

#define LOCAL_STRING_LEN 64       // length of internal used strings
typedef struct
{
  char szVersionString[LOCAL_STRING_LEN];
  char szDevicePath[LOCAL_STRING_LEN];
  int  nFileNo;
} PCAN_DESCRIPTOR;


//****************************************************************************
// LOCALS

//****************************************************************************
// CODE 

// centralized entry point for all exits
static void my_private_exit(int error)
{
  if (h)
  {
    CAN_Close(h); 
  }
  if (fp[0] != (FILE *)NULL)
    fclose(fp[0]);

  for (int i=0; i<24; i++)
    if (dataFile[i].is_open())
      dataFile[i].close();
  
#ifdef LOCAL_DEBUG
  cout << "getUncorrelatedData: finished (" << error << ")\n";
#endif
  exit(error);
}

// handles CTRL-C user interrupt
static void signal_handler(int signal)
{
  my_private_exit(0);
}

//**********************************************
// here all is done
int getUncorrelatedData(unsigned int nodeID, 
			int numWordsRequested,
			const char *dirName,
			const char *dataFileName, 
			int chanList[])
{
#ifdef LOCAL_DEBUG
#endif
  //__u32 dwPort = 0;
  //__u16 wIrq = 0;
  //__u16 wBTR0BTR1 = CAN_BAUD_250K;
  int i, stat;
  char txt[255]; // temporary string storage
  unsigned int buffer[2];
  unsigned char *uc_ptr =  (unsigned char *)buffer;

  int numWordsRead = 0;
  int chan, TDC;
  int timeout = 5000;
  time_t acqTimeout = 86400000;	// 24 hours
  time_t startTime;

#ifdef LOCAL_DEBUG
  cout << "getUncorrelatedData:  nodeID = " << hex << nodeID << ", numWordsRequested = " 
       << dec << numWordsRequested << ", directory " << dirName
       << ", dataFileName = " << dataFileName 
       << "\nchanList = {";
  for(i=0; i<24; i++) {
    if(chanList[i]) {
      cout << dec << i << " ";
    }
  }
  cout << "}" << endl;
#endif

  // create top directory
  errno = 0;
  stat = mkdir (dirName, 0755);
  //cout << "mkdir returned " << stat << " errno = " << errno << endl;

  if (stat != 0) {
    if (errno != EEXIST) {
      cout << "mkdir " << dirName << " returned " << stat << " errno = " << errno << endl;
      perror("getUncorrelatedData: mkdir");
      return 1;
    }
  }

  // create channel directories
  errno = 0;
  string filename;
  for(i=0; i<24; i++) {
    if(chanList[i]) {
      stringstream ss;
      ss << dirName << "/Ch" << setw(2) << setfill('0') << i;
      filename = ss.str();
      stat = mkdir (filename.c_str(), 0755);
      //cout << "mkdir " << filename << " returned " << stat << " errno = " << errno << endl;
      if (stat != 0) {
	if (errno != EEXIST) {
	  cout << "mkdir " << filename << " returned " << stat << " errno = " << errno << endl;
	  perror("getUncorrelatedData: mkdir");
	  my_private_exit(errno);
	}
      }
      ss << "/" << dataFileName;
      filename = ss.str();
#ifdef LOCAL_DEBUG
      cout << "Opening data file: " << filename << endl;
#endif
      dataFile[i].open( filename.c_str(), ofstream::out | ofstream::app );
      if ( !(dataFile[i].good()) ) {
	cout << "ERROR: " << filename << ": file error\n";
	perror("getUncorrelatedData:file.open");
	my_private_exit(errno);
      }
    }
  }

  errno = 0;
  
  // install signal handler for manual break
  signal(SIGINT, signal_handler);
  
  // open the CAN port
  // please use what is appropriate  
  // HW_DONGLE_SJA 
  // HW_DONGLE_SJA_EPP 
  // HW_ISA_SJA 
  // HW_PCI 
  // HW_USB  -- this is the one we are using

  //h = CAN_Open(HW_USB, dwPort, wIrq);
  h = LINUX_CAN_Open("/dev/pcan32", O_RDWR|O_NONBLOCK);
  
  PCAN_DESCRIPTOR *pcd = (PCAN_DESCRIPTOR *)h;
  
  if (!h) {
    errno = nGetLastError();
    perror("getUncorrelatedData: LINUX_CAN_Open()");
    my_private_exit(errno);
  }

    
  // get version info
  errno = CAN_VersionInfo(h, txt);
  if (!errno) {
#ifdef LOCAL_DEBUG
    cout << "getUncorrelatedData: driver version = "<< txt << endl;
#endif
  }
  else {
    perror("getUncorrelatedData: CAN_VersionInfo()");
    my_private_exit(errno);
  }
  
#ifdef TDIG
  // open CAN Port for TDIG, init PCAN-USB
  errno = CAN_Init(h, CAN_BAUD_1M,  CAN_INIT_TYPE_ST);
  if (errno) {
    perror("getUncorrelatedData: CAN_Init()");
    my_private_exit(errno);
  } 
#else
  // open CAN Port for Jalapeno, init PCAN-USB
  errno = CAN_Init(h, CAN_BAUD_250K,  CAN_INIT_TYPE_ST);
  if (errno) {
    perror("getUncorrelatedData: CAN_Init()");
    my_private_exit(errno);
  } 
#endif

  cout << "Starting getUncorrelatedData Procedure. Hit Ctrl-C to abort ...\n";

  TPCANMsg m;
  __u32 status;
    
  struct pollfd pfd;
  pfd.fd = pcd->nFileNo;
  pfd.events = POLLIN;


  startTime = time(0);
  while (1) { // loop forever
    if (numWordsRead >= numWordsRequested) break;
    if ( (time(0) - startTime) > acqTimeout) break; 


    errno = poll(&pfd, 1, timeout);
    
    if (errno == 1) { // data received
      if ((errno = CAN_Read(h, &m))) {
	perror("getUncorrelatedData: CAN_Read()");
	my_private_exit(errno);
      }
      else { // data read
#ifdef LOCAL_DEBUG
	printf("getUncorrelatedData: message received : %c %c 0x%08x %1d  ", 
	       (m.MSGTYPE & MSGTYPE_RTR)      ? 'r' : 'm',
	       (m.MSGTYPE & MSGTYPE_EXTENDED) ? 'e' : 's',
	       m.ID, 
	       m.LEN); 
	
	for (i = 0; i < m.LEN; i++)
	  printf("0x%02x ", m.DATA[i]);
	printf("\n");
#endif
	
	// check if a CAN status is pending	     
	if (m.MSGTYPE & MSGTYPE_STATUS) {
	  status = CAN_Status(h);
	  if ((int)status < 0) {
	    errno = nGetLastError();
	    perror("getUncorrelatedData: CAN_Status()");
	    my_private_exit(errno);
	  }
	  else
	    printf("getUncorrelatedData: pending CAN status 0x%04x read.\n", (__u16)status);
	} 
	else if (m.MSGTYPE == MSGTYPE_STANDARD) {
	  // now interprete the received message:
	  if (m.ID != (0x000 | nodeID)) { // check if it's a getData response
	    cout << "getUncorrelatedData: Got smething other than getUncorrelatedData response: ID 0x" 
		 << hex << m.ID 
		 << ", expected 0x" << (0x000 | nodeID) << endl;
	    continue;
	  }
	  if ( (m.LEN != 4) && (m.LEN != 8) ) { // check for correct length
	    cout << "ERROR: getUncorrelatedData: Got msg with incorrect data length " 
		 << dec << m.LEN << ", should be 4 or 8" << endl;
	    continue;
	  }
	  // Message is okay, so log the data
	  for (i=0; i<4; i++)
	    uc_ptr[3-i] = m.DATA[i];
	  if ( (buffer[0] & 0xF0000000) == 0x60000000 ) {
#ifdef LOCAL_DEBUG
	    cout << "Error word: 0x" << hex << buffer[0] << endl;
#endif
	  }
	  else if ( (buffer[0] & 0xF0000000) == 0xE0000000 ) {
#ifdef LOCAL_DEBUG
	    cout << "separator word: 0x" << hex << buffer[0] << endl;
#endif
	  }
	  else { // good data word, analyze and save
	    TDC = (buffer[0] & 0x0f000000) >> 24;
	    if ((TDC % 4) == 3) // low res TDC has IDs 3, 7, 11, or 15
	      chan = (buffer[0] & 0x00f80000) >> 19; // low res TDC
	    else {
	      chan = (buffer[0] & 0x00e00000) >> 21; // hi res TDC
	      chan += (TDC % 4) * 8; // TDC with channel 0 has IDs 0, 4, 8, or 12
	    }
#ifdef LOCAL_DEBUG
	    cout << "Got data from " 
		 << ( ((TDC % 4) == 3) ? "lo res" : "hi res" ) 
		 << " TDC " << dec << TDC
		 << ", chan " << chan << endl;
#endif
	    if (!chanList[chan]) {
	      cout << "ERROR: Get Uncorrelated Data: Data coming from unexpected channel "
		   << dec << chan << ". Check data collection settings.\n";
	      my_private_exit(-1);
	    }
	    else {
	      numWordsRead++;
	      dataFile[chan] << hex << showbase << buffer[0] << "\n";
	      //fprintf(fp[0], "0x%08x\n", buffer[0]);
	    }
	  }
	    
	  if (m.LEN == 8) {
	    for (i=4; i<8; i++)
	      uc_ptr[11-i] = m.DATA[i];
	    if ( (buffer[1] & 0xF0000000) == 0x60000000 ) {
#ifdef LOCAL_DEBUG
	      cout << "Error word: 0x" << hex << buffer[1] << endl;
#endif
	      continue;
	    }
	    else { // good data word, analyze and save
	      
	      TDC = (buffer[1] & 0x0f000000) >> 24;
	      if ((TDC % 4) == 3) // low res TDC has IDs 3, 7, 11, or 15
		chan = (buffer[1] & 0x00f80000) >> 19; // low res TDC
	      else {
		chan = (buffer[1] & 0x00e00000) >> 21; // hi res TDC
		chan += (TDC % 4) * 8; // TDC with channel 0 has IDs 0, 4, 8, or 12
	      }
#ifdef LOCAL_DEBUG
	      cout << "Got data from " 
		   << ( ((TDC % 4) == 3) ? "lo res" : "hi res" ) 
		   << " TDC " << dec << TDC
		   << ", chan " << chan << endl;
#endif
	      if (!chanList[chan]) {
		cout << "ERROR: Get Uncorrelated Data: Data coming from unexpected channel "
		     << dec << chan << ". Check data collection settings.\n";
		my_private_exit(-1);
	      }
	      else {
		numWordsRead++;
		dataFile[chan] << hex << showbase << buffer[1] << "\n";
		//fprintf(fp[0], "0x%08x\n", buffer[1]);
	      }
	    }
	  } // ... if (m.LEN == 8)
	  
	} // ... if (m.MSGTYPE == MSGTYPE_STANDARD) 
	
      } // data read
    } // data received
    else {	
      cout << "ERROR: timeout: did not receive response within "
	   << dec << timeout << " msec" << endl;
      break;
    }
  } // ... while (1) loop end
    

  cout << "Collected " << dec << numWordsRead 
       << " data words. Elapsed acquisition time: "
       << (time(0) - startTime) << " seconds\n";
  my_private_exit(errno);
  
  return errno;
}


int main(int argc, char *argv[])
{
  unsigned int nodeID;	
  int chan;
  int chanList[24];
  
  // initialize chanList to all 0
  for (int i=0; i<24; i++) {
    chanList[i] = 0;
  }
  
#ifdef LOCAL_DEBUG
  cout << "getUncorrelatedData, last modified August 20, 2004" << endl;
#endif
  
  if (argc < 6) {
    cout << "USAGE: " << argv[0] 
	 << " <nodeID> <number of words> <directory name> <filename> ?<channel A>? ?<channel B>?...\n";
    return 1;
  } 

  int numChan = argc - 5;
  if (numChan > 24) {
    cerr << "Maximum number of channels is 24.\n";
    return 1;
  }
  for (int i=5; i<argc; i++) {
    chan = atoi(argv[i]);
    if ((chan < 0)||(chan > 23)) {
      cout << "ERROR: Channel "
	   << chan 
	   << " is not a valid channel.  Must be between 0 and 23.\n";
      return 1;
    }
    chanList[chan] = 1;
  }
  
  nodeID = atoi(argv[1]);
  
  if ((nodeID < 1) || (nodeID > 8)) {
    cerr << "nodeID = " << nodeID 
	 << " is an invalid entry.  Use a value between 1 and 8 instead."  << endl;
    return 1;
  }
  
  int numWordsRequested = atoi(argv[2]);
  
  return getUncorrelatedData(nodeID, numWordsRequested, argv[3], argv[4], chanList);
}

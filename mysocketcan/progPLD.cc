/* File name     : progPLD.cc
 * Creation date : 7/3/2012
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id$";
#endif /* lint */

// #define LOCAL_DEBUG

//****************************************************************************
// INCLUDES
// C++ header file
#include <iostream>
#include <fstream>
using namespace std;

// other headers
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
//#include <time.h>

// pcan include file
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

// local utilities
#include "can_utils.h"

//****************************************************************************
// DEFINES
#define LINE_UP "[1A[80D[0J"

//****************************************************************************
// GLOBALS
int h = -1;


//****************************************************************************
// LOCALS

//****************************************************************************

//****************************************************************************
// CODE 
//****************************************************************************

// centralized entry point for all exits
static void my_private_exit(int error)
{
  if (h > 0)
  {
    close(h); 
  }
#ifdef LOCAL_DEBUG
  cout << "progPLD: finished (" << error << ")\n";
#endif
  exit(error);
}

// handles CTRL-C user interrupt
static void signal_handler(int signal)
{
  my_private_exit(0);
}


// everything happens here
int p_progPLD(const char *filename, int pldNum, int nodeID, int devID)
{
  ifstream conf;
  unsigned char confByte[256];
  int fileSize, noPages;
  
  // struct timespec timesp;
  
  cout << "Configuring PLD " << pldNum 
       << " on nodeID 0x" << hex << nodeID
       << " with filename " << filename << "...\n";
  
  errno = 0;
  
  
  conf.open(filename, ios_base::binary); // "in" is default 
  if ( !conf.good() ) {
    cerr << filename << ": file error\n";
    return -1;
  }
  
  conf.seekg(0,ios::end);
  fileSize = conf.tellg();
  noPages = fileSize/256;
  
  // timesp.tv_sec = 0;
  // timesp.tv_nsec = 1000;	// 1 ms
  
  cout << "Filesize = " << dec << fileSize << " bytes, " << fileSize/1024 << " kbytes, "
       << noPages << " pages\n";
  
  conf.seekg(0,ios::beg);    // move file pointer to beginning of file
  
  
  cout << "Starting Configuration Procedure....\n";
  
  struct can_frame ms;
  struct can_frame mr;
  
  // swallow any packets that might be present first
  errno = CAN_Read_Timeout(h, &mr, 10000); // timeout = 10 mseconds
  
  // ***************************************************************************
  
  // ************** progPLD:Start ****************************************
  // this is a write message (msgID = 0x002)
  
  ms.can_id = 0x002 | (nodeID << 4);
  ms.can_dlc = 2;
  
  ms.data[0] = 0x20;
  ms.data[1] = pldNum;
  
  cout << LINE_UP << "Starting Bulk Erase...\n"; cout.flush();
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "progPLD: Sending progPLD:Start command:");
#endif
  
  if ( sendCAN_and_Compare(h, ms, "progPLD: progPLD:Start", 60000000) != 0) // timeout = 60 sec
    my_private_exit(errno);
  
  cout << LINE_UP << "Bulk Erase finished...\nStarting page programming...\n";
  cout.flush();
  sleep(2);
  for (int page=0; page<noPages/2; page++) {
    int EPCS_Address = page*256;
    // read 256 bytes
    conf.read((char *)confByte, 256);
    
    bool allFF = true;
    for (int i=0; i<256; i++) {
      if(confByte[i] != 0xff) {
	allFF = false;
	break;
      }
    }
    
    if (allFF) continue;
    
    // ************** progPLD:WriteAddress ****************************************
    
    ms.data[0] = 0x21;	// write Address, lowest to highest byte
    ms.data[1] = (EPCS_Address & 0x0000FF);
    ms.data[2] = ((EPCS_Address & 0x00FF00) >> 8);
    ms.data[3] = ((EPCS_Address & 0xFF0000) >> 16);
    ms.can_dlc = 4;
#ifdef LOCAL_DEBUG
    printCANMsg(ms, "progPLD: Sending progPLD:WriteAddress command:");
#endif
    
    // do this without response:
    if (write(h, &ms, sizeof(ms)) != sizeof(ms)) {
      perror("progPLD:write()");
      return -1;
    }
    errno = CAN_Read_Timeout(h, &mr, 1000000); // timeout = 1 second
    if (errno < 0) {
      cout << "CAN_Read_Timeout returned " << errno 
	   << " during progPLD:WriteAddress\n";
      return -1;
    }
    else if (errno == 0)
      cout << "Timeout during progPLD:WriteAddress\n";

    ms.data[0] = 0x22;
    ms.can_dlc = 8;
    unsigned char *tmpPtr = confByte;
    for (int i=0; i<36; i++) {
      // ************** progPLD:WriteDataBytes ************************************
      
      
      for (int j=1; j<8; j++) {
	ms.data[j] = *tmpPtr;
	tmpPtr++;
      }
#ifdef LOCAL_DEBUG
      printCANMsg(ms, "progPLD: Sending progPLD:WriteDataBytes command:");
#endif
      
      // do this without response:
      if (write(h, &ms, sizeof(ms)) != sizeof(ms)) {
	perror("progPLD:write()");
	return -1;
      }
      // waste some time, so packets aren't sent too fast
      //nanosleep(&timesp, NULL);
      // for (int j=0; j<4300000; j++) ;
      
      errno = CAN_Read_Timeout(h, &mr, 1000000); // timeout = 1 second
      const char *RED_ON_WHITE = "\033[47m\033[1;31m";
      const char *NORMAL_COLORS = "\033[0m";
      if (errno < 0) {
	cout << RED_ON_WHITE;
	cout << "CAN_Read_Timeout returned " << errno 
	     << " during progPLD:WriteDataBytes\n";
	cout << NORMAL_COLORS;
	return -1;
      }
      else if (errno == 0) {
	cout << RED_ON_WHITE;
	cout << "Timeout during progPLD:WriteDataBytes\n";
	cout << NORMAL_COLORS;
      }    
      
    }
    
    // ************** progPLD:Program256 ****************************************
    
    ms.data[0] = 0x23;
    ms.can_dlc = 5;
    for (int j=1; j<5; j++) {
      ms.data[j] = *tmpPtr;
      tmpPtr++;
    }
#ifdef LOCAL_DEBUG
    printCANMsg(ms, "progPLD: Sending progPLD:Program256 command:");
#endif
    
    if ( sendCAN_and_Compare(h, ms, "progPLD:Program256", 1000000) != 0) { // timeout = 1sec
      cout << "page = " << dec << page << endl;
      my_private_exit(errno);
    }
    
    //#ifdef LOCAL_DEBUG
    if(page<11) {cout << LINE_UP << "Page " << dec << page << "...\n"; flush(cout);}
    else if((page%100) == 0) {cout << LINE_UP << "Page " << dec << page << "...\n"; flush(cout);}
    //#endif
    
    
  } // for (int page
  
  cout << "Page Programming finished...\nSending asDone...\n";
  ms.data[0] = 0x24;
  ms.can_dlc = 1;
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "progPLD: Sending progPLD:asDone command:");
#endif
  
  if ( sendCAN_and_Compare(h, ms, "progPLD:asDone", 1000000) != 0) // timeout = 1 sec
    my_private_exit(errno);
  
  
  // ************************* finished !!!! ****************************************
  // ********************************************************************************
  
  conf.close();
  cout << "... Configuration finished successfully.\n";
  
  return errno;
}


// *********************** main function *********************************************
int main(int argc, char *argv[])
{
  int devID = 0;

  cout << vcid << endl;
  cout.flush();
  
  if ( argc < 4 ) {
    cout << "USAGE: " << argv[0] << " <fileName> <PLD # (0=m, 1-8=s, 9=all)> <nodeID> [<devID>]\n";
    return 1;
  }
  
  // 1 <= pldNum <= 8: Serdes FPGA <pldNum>
  // pldNum = 0: Master FPGA
  // pldNum = 9: all Serdes FPGAs
  int pldNum = atoi(argv[2]);
  int nodeID = strtol(argv[3], (char **)NULL, 0);
  if (argc >= 5) {
    devID = strtol(argv[4],(char **)NULL, 0);
    if (devID > 7) {
      printf("Invalid Device ID 0x%x. Use a device ID between 0 and 7\n", devID);
      return -1;
    }
  }

  // install signal handler for manual break
  signal(SIGINT, signal_handler);

  if((h = CAN_Open(devID)) < 0) {
    my_private_exit(-1);
  }

  if ((pldNum < 9) && (pldNum >= 0))
    p_progPLD(argv[1], pldNum, nodeID, devID);
  else if (pldNum == 9)
    for (int i=1; i<9; i++)
      p_progPLD(argv[1], i, nodeID, devID);
  else 
    cerr << "Invalid pldNum " << pldNum << "; exiting ...\n";

  my_private_exit(errno);
  return 0;
}

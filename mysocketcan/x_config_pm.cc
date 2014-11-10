/* File name     : x_config_pm.cc
 * Creation date : 6/26/2012
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id$";
#endif /* lint */

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
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>


// pcan include file
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

// local utilities
#include "can_utils.h"

//****************************************************************************
// DEFINES
//#define LOCAL_DEBUG

//****************************************************************************
// GLOBALS
int cansock = 0;

//****************************************************************************
// LOCALS

//****************************************************************************
// CODE 

// centralized entry point for all exits
static void my_private_exit(int error)
{
  if (cansock > 0)
  {
    close(cansock); 
  }
#ifdef LOCAL_DEBUG
  cout << "x_config_pm: finished (" << error << ")\n";
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
//*********************************************
int p_config(const char *filename, unsigned int tdigNodeID, unsigned int tcpuNodeID, int TDC, int devID)
{
  int i,j;

  bool isHexEntry = false;

  ifstream conf;
  unsigned char confByte[81];
  unsigned int tempval;
  char buffer[255];

  struct can_frame ms, mr;

  cout << "Storing configuration in PM for TDC " << TDC
       << " at TDIG NodeID 0x" << hex << tdigNodeID
       << " through TCPU NodeID 0x" << tcpuNodeID
       << "\n with filename " << filename 
       << " on CANbus ID 0x" << devID << "...\n";

  errno = 0;
  
  // install signal handler for manual break
  signal(SIGINT, signal_handler);
  
  // open file "filename" and extract control bits
  conf.open(filename); // "in" is default 
  if ( !conf.good() ) {
    cerr << filename << ": file error\n";
    return -1;
  }
  
  conf.seekg(0,ios::beg);    //move file pointer to beginning of file
  conf.getline(buffer, 80);
  // check if lines contain hex values (with the letter "x" in it)
  if( strchr(buffer, 'x') != NULL) isHexEntry = true;
  conf.seekg(0,ios::beg);    //move file pointer to beginning of file again
  
  conf >> hex;
  
  // clear buffer
  buffer[0]='\0';
  for (i=0; (i<81 && conf.good()); i++) { 
    conf >> tempval;
    if ( !conf.good() ) {
      cout << "ERROR: " << filename << ": error reading value for control byte " << dec << i << endl;
      conf.close();
      return -1;
    }
    
    if (isHexEntry)
      confByte[i] = (unsigned char)tempval;
    else
      confByte[i] =	(tempval & 0x00000001) |
	((tempval & 0x00000010)>>3) |
	((tempval & 0x00000100)>>6) |
	((tempval & 0x00001000)>>9) |
	((tempval & 0x00010000)>>12) |
	((tempval & 0x00100000)>>15) |
	((tempval & 0x01000000)>>18) |
	((tempval & 0x10000000)>>21);
  }

  conf.close();

  // open socket
  if ((cansock =  CAN_Open(devID)) < 0) {
    my_private_exit(-1);
  }

  cout << "Starting Configuration Procedure....\n";

  // swallow any packets that might be present first
  errno = CAN_Read_Timeout(cansock, &mr, 100000); // timeout = 100 mseconds
  
  int nodeIDVal = (((tdigNodeID << 4) | 0x002) << 18) | tcpuNodeID;

  ms.can_id = nodeIDVal | CAN_EFF_FLAG;

  /* 
   * The data in the configuration file for HLP 3 and higher is ordered with the LSB as the first byte.
   * The 81st byte is filled with an additional 0 at the highest (8th) bit. Data is sent with the LSB
   * first.
   */

  // ************** CONFIGURE_TDC:Write Block Start ****************************************
  // move the TDIG msg ID up 18 bits, make it a "Write" message,
  // and add the TCPU nodeID in the extended part
  ms.can_dlc = 1;

  // "CONFIGURE_TDC:Block Start"
  ms.data[0] = 0x10;

#ifdef LOCAL_DEBUG
  printCANMsg(ms, "x_config_pm: Sending CONFIGURE_TDC:Block Start command:");
#endif

  if ( sendCAN_and_Compare(cansock, ms, "x_config_pm: CONFIGURE_TDC:Write Block Start", 4000000,2, true)
       != 0) // timeout = 4 sec
    my_private_exit(-1);

  
  // *************** "CONFIGURE_TDC:Block Data", 11 messages with 7 bytes each ***********
  ms.can_dlc = 8;

  for (i=0; i<11; i++) {
    ms.data[0] = 0x20;
    for (j=0; j<7; j++) ms.data[j+1] = confByte[i*7+j];
    
#ifdef LOCAL_DEBUG
    printCANMsg(ms, "x_config_pm: Sending CONFIGURE_TDC:Block DATA packet:");
#endif
    
    
    if ( sendCAN_and_Compare(cansock, ms, "x_config_pm: CONFIGURE_TDC:Block DATA", 4000000, 2, true)
	 != 0) // timeout = 4 sec
      my_private_exit(-1);

  } // end "for(i=0, ... " loop


  // **************** "CONFIGURE_TDC:Block Data", ... and 1 last message with 4 bytes ******
  ms.can_dlc = 5;

  for (j=0; j<4; j++) ms.data[j+1] = confByte[77+j];
  
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "x_config_pm: Sending CONFIGURE_TDC:Block DATA packet 12:");
#endif
  
  
  if ( sendCAN_and_Compare(cansock, ms, "x_config_pm: CONFIGURE_TDC:Block DATA 12:", 4000000, 2, true) 
       != 0) // timeout = 4 sec
    my_private_exit(-1);

  // *************************** CONFIGURE_TDC:Write Block End *************************
  ms.can_dlc = 1;
  ms.data[0] = 0x30;
  
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "x_config_pm: Sending CONFIGURE_TDC:Write Block End packet:");
#endif


  if ( sendCAN_and_Compare(cansock, ms, "x_config_pm: CONFIGURE_TDC:Write Block End:", 4000000, 8, true) 
       != 0) // timeout = 4 sec
    my_private_exit(-1);
  

  // ****************************** "CONFIGURE_TDC:Write Block Target Config PM" **********************
  ms.data[0] = 0x44 | (TDC&0x3);
  
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "x_config_pm: Sending CONFIGURE_TDC:Write Block Target Config PM packet:");
#endif
  
  if ( sendCAN_and_Compare(cansock, ms, "x_config_pm: CONFIGURE_TDC:Write Block Target Config PM:", 4000000, 2, true) 
       != 0) // timeout = 4 sec
    my_private_exit(-1);


  // ************************* finished !!!! ****************************************
  // ********************************************************************************

  cout << "... Configuration finished successfully.\n";
  my_private_exit(errno);
  
  return errno;
}


int main(int argc, char *argv[])
{
  unsigned int tdigNodeID, tcpuNodeID;
  int devID = 0;

  cout << vcid << endl;
  cout.flush();
  
  if ( argc < 5 ) {
    cout << "USAGE: " << argv[0] << " <TDC ID> <TDIG node ID> <TCPU node ID> <fileName> [<devID>]\n";
    return 1;
  }
  
  int TDC = atoi(argv[1]);
  if ((TDC < 0) || (TDC > 3)) { 
    cerr << "TDC = " << TDC << " invalid entry. Use 0-3 instead." << endl;
    return 1;
  }

  tdigNodeID =  strtol(argv[2], (char **)NULL, 0);
  if ((tdigNodeID < 1) || (tdigNodeID > 0x3f)) { 
    cerr << "tdigNodeID = " << tdigNodeID 
	 << " is an invalid entry.  Use a value between 1 and 0x3f (63) instead.\n";
    return 1;
  }
  
  tcpuNodeID =  strtol(argv[3], (char **)NULL, 0);
  if ((tcpuNodeID < 1) || (tcpuNodeID > 0x3f)) { 
    cerr << "tcpuNodeID = " << tcpuNodeID 
	 << " is an invalid entry.  Use a value between 1 and 0x3f (63) instead.\n";
    return 1;
  }
  
  if (argc == 6) {
    devID = atoi(argv[5]);
    if (devID > 10) {
      printf("Invalid Device ID 0x%x. Use a device ID between 0 and 10\n", devID);
      return -1;
    }
  }
  
  return p_config(argv[4], tdigNodeID, tcpuNodeID, TDC, devID);
}

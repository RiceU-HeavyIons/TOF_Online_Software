/* File name     : x_config_scripted.cc
 * Creation date : 7/3/2012
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: x_config_scripted.cc 756 2012-07-05 20:44:12Z jschamba $";
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

// #define LOCAL_DEBUG

// use this define to power down the HPTDC before configuring
#define POWERDOWN_FIRST

//****************************************************************************
// GLOBALS
int h = -1;

//****************************************************************************
// LOCALS

//****************************************************************************
// CODE 

// centralized entry point for all exits
static void my_private_exit(int error)
{
  if (h > 0)
  {
    close(h); 
  }
#ifdef LOCAL_DEBUG
  cout << "x_config_scripted: finished (" << error << ")\n";
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

  cout << "Configuring TDC " << TDC
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
  

  cout << "Starting Configuration Procedure....\n";

  struct can_frame ms;
  struct can_frame mr;


  // swallow any packets that might be present first
  errno = CAN_Read_Timeout(h, &mr, 100000); // timeout = 100 mseconds
  
  int nodeIDVal = (((tdigNodeID << 4) | 0x002) << 18) | tcpuNodeID;

  ms.can_id = nodeIDVal | CAN_EFF_FLAG;


#ifdef POWERDOWN_FIRST
  // send a "power down" control word to the HTPDC first:
  ms.can_dlc = 6;
  ms.data[0] = 0x4 | (TDC & 0x3);
  ms.data[1] = 0x00;
  ms.data[2] = 0x00;
  ms.data[3] = 0x00;
  ms.data[4] = 0x00;
  ms.data[5] = 0x00;
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "x_config_scripted: Sending CONFIGURE_TDC:Block Start command:");
#endif

  if ( sendCAN_and_Compare(h, ms, "x_config_scripted: Control-Power Down:", 4000000, 2, true) != 0) // timeout = 4 sec
    my_private_exit(errno);
#endif

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
  printCANMsg(ms, "x_config_scripted: Sending CONFIGURE_TDC:Block Start command:");
#endif

  if ( sendCAN_and_Compare(h, ms, "x_config_scripted: CONFIGURE_TDC:Write Block Start", 4000000,2, true) != 0) // timeout = 4 sec
    my_private_exit(errno);

  
  // *************** "CONFIGURE_TDC:Block Data", 11 messages with 7 bytes each ***********
  ms.can_dlc = 8;

  for (i=0; i<11; i++) {
    ms.data[0] = 0x20;
    for (j=0; j<7; j++) ms.data[j+1] = confByte[i*7+j];
    
#ifdef LOCAL_DEBUG
    printCANMsg(ms, "x_config_scripted: Sending CONFIGURE_TDC:Block DATA packet:");
#endif
    
    
    if ( sendCAN_and_Compare(h, ms, "x_config_scripted: CONFIGURE_TDC:Block DATA", 4000000, 2, true) != 0) // timeout = 4 sec
      my_private_exit(errno);

  } // end "for(i=0, ... " loop


  // **************** "CONFIGURE_TDC:Block Data", ... and 1 last message with 4 bytes ******
  ms.can_dlc = 5;

  for (j=0; j<4; j++) ms.data[j+1] = confByte[77+j];
  
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "x_config_scripted: Sending CONFIGURE_TDC:Block DATA packet 12:");
#endif
  
  
  if ( sendCAN_and_Compare(h, ms, "x_config_scripted: CONFIGURE_TDC:Block DATA 12:", 4000000, 2, true) != 0) // timeout = 4 sec
    my_private_exit(errno);

  // *************************** CONFIGURE_TDC:Write Block End *************************
  ms.can_dlc = 1;
  ms.data[0] = 0x30;
  
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "x_config_scripted: Sending CONFIGURE_TDC:Write Block End packet:");
#endif


  if ( sendCAN_and_Compare(h, ms, "x_config_scripted: CONFIGURE_TDC:Write Block End:", 4000000, 8, true) != 0) // timeout = 4 sec
    my_private_exit(errno);
  

  // ****************************** "CONFIGURE_TDC:Write Block Target TDC" **********************
  ms.data[0] = 0x40 | (TDC&0x3);
  // the HLP document version 3f specifies 0x44 as the base, but the MCU code implemented 0x40 as
  // the base, so this statement is correct!!!!!!
  
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "x_config_scripted: Sending CONFIGURE_TDC:Write Block Target TDC packet:");
#endif
  
  if ( sendCAN_and_Compare(h, ms, "x_config_scripted: CONFIGURE_TDC:Write Block Target TDC:", 4000000, 2, true) != 0) // timeout = 4 sec
    my_private_exit(errno);

  // ************************* finished !!!! ****************************************
  // ********************************************************************************

  cout << "... Configuration finished successfully.\n";


  
  return errno;
}


int main(int argc, char *argv[])
{
  unsigned int tdigNodeID, tcpuNodeID, TDC;
  int devID = 0;
  ifstream scriptFile;
  char confFileName[256];
  

  cout << vcid << endl;
  cout.flush();

  if ( argc < 3 ) {
    cout << "USAGE: " << argv[0] << " <TCPU nodeID> <scriptFileName> [<devID>]\n";
    return 1;
  }
  
  tcpuNodeID =  strtol(argv[1], (char **)NULL, 0);
  if ((tcpuNodeID < 1) || (tcpuNodeID > 0x3f)) { 
    cerr << "tcpuNodeID = " << tcpuNodeID 
	 << " is an invalid entry.  Use a value between 1 and 0x3f (63) instead.\n";
    return 1;
  }
  
  if (argc == 4) {
    devID = atoi(argv[3]);
    if (devID < 100 || devID > 107) {
      printf("Invalid Device ID 0x%x. Use a device ID between 100 and 107\n", devID);
      return -1;
    }
  }

  scriptFile.open(argv[2]);
  if ( !scriptFile.good() ) {
    cerr << argv[1] << ": file error\n";
    return -1;
  }

  scriptFile.seekg(0, ios::beg); // move file pointer to beginning of file


  if((h = CAN_Open(devID)) < 0) {
    my_private_exit(-1);
  }

  int line = 0;
  while (scriptFile.good()) {
    // scriptFile >> TDC >> tdigNodeID >> tcpuNodeID >> confFileName;
    scriptFile >> hex >> TDC >> tdigNodeID >> confFileName;
    if ( !scriptFile.good() ) break;
    line++;
    cout << "Line " << dec << line 
	 << hex << ": TDC 0x" 
	 << TDC
	 << " tdigNodeID 0x" << tdigNodeID
	 << " tcpuNodeID 0x" << tcpuNodeID
	 << " confFileName " << confFileName << endl;
    p_config(confFileName, tdigNodeID, tcpuNodeID, TDC, devID);

  }
  scriptFile.close();

  my_private_exit(errno);

  return 0;
}

/* File name     : p_config.cc
 * Creation date : 8/18/04
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: p_config.cc,v 1.13 2007-10-11 19:05:17 jschamba Exp $";
#endif /* lint */

//****************************************************************************
// INCLUDES
// C++ header file
#include <iostream>
#include <fstream>
using namespace std;

// other headers
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>


// pcan include file
#include <libpcan.h>

// local utilities
#include "can_utils.h"

//****************************************************************************
// DEFINES

// #define LOCAL_DEBUG
//#define TDIG
#define TDIG_D

//****************************************************************************
// GLOBALS
HANDLE h = NULL;

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
#ifdef LOCAL_DEBUG
  cout << "p_config: finished (" << error << ")\n";
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
int p_config(const char *filename, unsigned int nodeID, int TDC, WORD devID)
{
  //__u32 dwPort = 0;
  //__u16 wIrq = 0;
  //__u16 wBTR0BTR1 = CAN_BAUD_250K;
  int i,j;

  bool isHexEntry = false;

  ifstream conf;
  unsigned char confByte[81];
  unsigned int tempval;
  char buffer[255];

  cout << "Configuring TDC " << TDC
       << " at NodeID 0x" << hex << nodeID
       << " with filename " << filename 
       << " on CANbus ID 0x" << hex << devID << "...\n";

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
  

  if((errno = openCAN(devID)) != 0) {
    my_private_exit(errno);
  }


  cout << "Starting Configuration Procedure....\n";

  TPCANMsg ms;
  TPCANRdMsg mr;


  // swallow any packets that might be present first
  errno = LINUX_CAN_Read_Timeout(h, &mr, 100000); // timeout = 100 mseconds
  
#ifdef TDIG_D
  /* 
   * The data in the configuration file for HLP 3 and higher is ordered with the LSB as the first byte.
   * The 81st byte is filled with an additional 0 at the highest (8th) bit. Data is sent with the LSB
   * first.
   */

  // ************** CONFIGURE_TDC:Write Block Start ****************************************
  int nodeIDVal = nodeID << 4;

  ms.MSGTYPE = MSGTYPE_STANDARD;
  ms.ID = 0x002 | nodeIDVal; // TDIG nodeIDs start currently with 0x10
  ms.LEN = 1;

  // "CONFIGURE_TDC:Block Start"
  ms.DATA[0] = 0x10;

#ifdef LOCAL_DEBUG
  printCANMsg(ms, "p_config: Sending CONFIGURE_TDC:Block Start command:");
#endif

  if ( sendCAN_and_Compare(ms, "p_config: CONFIGURE_TDC:Write Block Start", 1000000,2, true) != 0) // timeout = 1 sec
    my_private_exit(errno);

  
  // *************** "CONFIGURE_TDC:Block Data", 11 messages with 7 bytes each ***********
  ms.LEN = 8;

  for (i=0; i<11; i++) {
    ms.DATA[0] = 0x20;
    for (j=0; j<7; j++) ms.DATA[j+1] = confByte[i*7+j];
    
#ifdef LOCAL_DEBUG
    printCANMsg(ms, "p_config: Sending CONFIGURE_TDC:Block DATA packet:");
#endif
    
    
    if ( sendCAN_and_Compare(ms, "p_config: CONFIGURE_TDC:Block DATA", 1000000, 2, true) != 0) // timeout = 1 sec
      my_private_exit(errno);

  } // end "for(i=0, ... " loop


  // **************** "CONFIGURE_TDC:Block Data", ... and 1 last message with 4 bytes ******
  ms.LEN = 5;

  for (j=0; j<4; j++) ms.DATA[j+1] = confByte[77+j];
  
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "p_config: Sending CONFIGURE_TDC:Block DATA packet 12:");
#endif
  
  
  if ( sendCAN_and_Compare(ms, "p_config: CONFIGURE_TDC:Block DATA 12:", 1000000, 2, true) != 0) // timeout = 1 sec
    my_private_exit(errno);

  // *************************** CONFIGURE_TDC:Write Block End *************************
  ms.LEN = 1;
  ms.DATA[0] = 0x30;
  
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "p_config: Sending CONFIGURE_TDC:Write Block End packet:");
#endif


  if ( sendCAN_and_Compare(ms, "p_config: CONFIGURE_TDC:Write Block End:", 1000000, 8, true) != 0) // timeout = 1 sec
    my_private_exit(errno);
  

  // ****************************** "CONFIGURE_TDC:Write Block Target TDC" **********************
  ms.DATA[0] = 0x40 | (TDC&0x3);
  // the HLP document version 3f specifies 0x44 as the base, but the MCU code implemented 0x40 as
  // the base, so this statement is correct!!!!!!
  
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "p_config: Sending CONFIGURE_TDC:Write Block Target TDC packet:");
#endif
  
  if ( sendCAN_and_Compare(ms, "p_config: CONFIGURE_TDC:Write Block Target TDC:", 4000000, 2, true) != 0) // timeout = 4 sec
    my_private_exit(errno);
  


  // ************************* finished !!!! ****************************************
  // ********************************************************************************


// ********************************************************************************
// ********************************** old HLP Protocol ****************************
// ********************************************************************************
#else // old TDIG
  /*
   * Data in the configuration file for the old TDIG was ordered with the MSB first. The
   * lowest bit was filled with a 0 at bit position 0. Data is sent with the MSB first.
   */

  // TDCs 1-4 are encoded in the CAN MsgID as follows:
  // MsgID[5:4] = 00  => TDC1
  // MSgID[5:4] = 01  => TDC2
  // MsgID[5:4] = 10  => TDC3
  // MSgID[5:4] = 11  => TDC4
  int TDCVal = (TDC-1)<<4;

  // ***************************************************************************

  // ************** CONFIGURE_TDC:Start ****************************************

  ms.MSGTYPE = MSGTYPE_STANDARD;
  ms.ID = 0x200 | TDCVal | nodeID;
  ms.LEN = 1;

  // "CONFIGURE_TDC:Start"
  ms.DATA[0] = 0;

#ifdef LOCAL_DEBUG
  printCANMsg(ms, "p_config: Sending CONFIGURE_TDC:Start command:");
#endif

  if ( sendCAN_and_Compare(ms, "p_config: CONFIGURE_TDC:Start", 1000000) != 0) // timeout = 1 sec
    my_private_exit(errno);

  
  // *************** "CONFIGURE_TDC:Data", 11 messages with 7 bytes each ***********
  ms.LEN = 8;

  for (i=1; i<12; i++) {
    ms.DATA[0] = 0x40 | i;
    for (j=0; j<7; j++) ms.DATA[j+1] = confByte[(i-1)*7+j];
    
#ifdef LOCAL_DEBUG
    printCANMsg(ms, "p_config: Sending CONFIGURE_TDC:DATA packet:");
#endif
    
    
    if ( sendCAN_and_Compare(ms, "p_config: CONFIGURE_TDC:DATA", 1000000) != 0) // timeout = 1 sec
      my_private_exit(errno);

  } // end "for(i=1, ... " loop


  // **************** "CONFIGURE_TDC:Data", ... and 1 last message with 4 bytes ******
  ms.LEN = 5;

  ms.DATA[0] = 0x4c;
  for (j=0; j<4; j++) ms.DATA[j+1] = confByte[77+j];
  
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "p_config: Sending CONFIGURE_TDC:DATA packet 12:");
#endif
  
  
  if ( sendCAN_and_Compare(ms, "p_config: CONFIGURE_TDC:DATA 12:", 1000000) != 0) // timeout = 1 sec
    my_private_exit(errno);

  // *************************** CONFIGURE_TDC:Config_end *************************
  ms.LEN = 1;
  ms.DATA[0] = 0x80;
  
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "p_config: Sending CONFIGURE_TDC:Config_end packet:");
#endif


  if ( sendCAN_and_Compare(ms, "p_config: CONFIGURE_TDC:Config_end:", 1000000) != 0) // timeout = 1 sec
    my_private_exit(errno);
  

  // ****************************** "CONFIGURE_TDC:Program" **********************
  ms.DATA[0] = 0xc0;
  
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "p_config: Sending CONFIGURE_TDC:Program packet:");
#endif
  
  if ( sendCAN_and_Compare(ms, "p_config: CONFIGURE_TDC:Program:", 1000000) != 0) // timeout = 1 sec
    my_private_exit(errno);
  



  // *************************** "TDC Reset" at the end ***************************
  ms.ID = 0x500 | nodeID;
  ms.LEN = 3;
  
  ms.DATA[0] = 0xbb;
  ms.DATA[1] = 0x04;
  ms.DATA[2] = 0x01;
  
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "p_config: Sending DEBUG:TDC_Reset_Assert packet:");
#endif
  
  if ( sendCAN_and_Compare(ms, "p_config: DEBUG:Assert_TDC_Reset", 1000000) != 0) // timeout = 1 sec
    my_private_exit(errno);
  

  // ***************************** "TDC Reset" at the end, de-assert ****************
  ms.DATA[2] = 0x00;
  
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "p_config: Sending DEBUG:TDC_Reset_Deassert packet:");
#endif
  
  if ( sendCAN_and_Compare(ms, "p_config: DEBUG:Deassert_TDC_Reset", 1000000) != 0) // timeout = 1 sec
    my_private_exit(errno);


  // ************************* "... and finally, a PLD Reset at the end *************
  ms.LEN = 4;
  
  ms.DATA[0] = 0x69;
  ms.DATA[1] = 0x96;
  ms.DATA[2] = 0xa5;
  ms.DATA[3] = 0x5a;
  
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "p_config: Sending DEBUG:PLD_Reset packet:");
#endif
  
  if ( sendCAN_and_Compare(ms, "p_config: DEBUG:PLD_Reset", 1000000) != 0) // timeout = 1 sec
    my_private_exit(errno);
  

  // ************************* finished !!!! ****************************************
  // ********************************************************************************
#endif

  cout << "... Configuration finished successfully.\n";


  my_private_exit(errno);
  
  return errno;
}


int main(int argc, char *argv[])
{
  unsigned int nodeID;
  WORD devID = 255;

  cout << vcid << endl;
  cout.flush();
  
  if ( argc < 4 ) {
    cout << "USAGE: " << argv[0] << " <TDC ID> <node ID> <fileName> <devID>\n";
    return 1;
  }
  
  int TDC = atoi(argv[1]);
  if ((TDC < 1) || (TDC > 4)) { 
    cerr << "TDC = " << TDC << " invalid entry. Use 1-4 instead." << endl;
    return 1;
  }

  nodeID =  strtol(argv[2], (char **)NULL, 0);
#ifndef TDIG_D
  if ((nodeID < 0) || (nodeID > 7)) { 
    cerr << "nodeID = " << nodeID 
	 << " is an invalid entry.  Use a value between 0 and 7 instead.\n";
    return 1;
  }
#else
  if ((nodeID < 1) || (nodeID > 0x3f)) { 
    cerr << "nodeID = " << nodeID 
	 << " is an invalid entry.  Use a value between 0x1 and 0x3f (63) instead.\n";
    return 1;
  }
#endif
  
  if (argc == 5) {
    devID = atoi(argv[4]);
    if (devID > 255) {
      printf("Invalid Device ID 0x%x. Use a device ID between 0 and 255\n", devID);
      return -1;
    }
  }
  
  return p_config(argv[3], nodeID, TDC, devID);
}

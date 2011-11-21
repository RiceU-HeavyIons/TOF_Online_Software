/* File name     : x_getChecksum_l.cc
 * Creation date : 11/11/2008
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id$";
#endif /* lint */

/* 
 * Read program memory between start address and end address from MCU
 * and calculate checksum. Same as "getChecksum", but CAN messages
 * go through TCPU
 */

// #define LOCAL_DEBUG

//****************************************************************************
// INCLUDES
// C++ header file
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

// other headers
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

// pcan include file
#include <libpcan.h>

// local utilities
#include "can_utils.h"

//****************************************************************************
// GLOBALS
HANDLE h = NULL;
extern const char *RED_ON_WHITE;
extern const char *NORMAL_COLORS;

//****************************************************************************
// LOCALS


//****************************************************************************
// CODE 

// centralized entry point for all exits
static void my_private_exit(int error)
{
  if (h)
  {
    CAN_Close_l(h); 
  }
#ifdef LOCAL_DEBUG
  cout << "x_getChecksum: finished (" << error << ")\n";
#endif
  exit(error);
}

//****************************************************************************
// handles CTRL-C user interrupt
static void signal_handler(int signal)
{
  my_private_exit(0);
}


//**********************************************
// here all is done
int getChecksum(unsigned int tdigNodeID, 
		unsigned int tcpuNodeID, 
		unsigned short startAddr,
		unsigned short endAddr,
		WORD devID,
		unsigned short expectedChecksum)
{
  unsigned int addr;
  TPCANMsg ms;
  TPCANRdMsg mr;
  unsigned short checksum = 0;
  unsigned int expectedResponseID;

  // start address needs to be word aligned
  startAddr &= 0xFFFE;
  // end address needs to be odd
  endAddr |= 0x1;

  cout << "getting MCU checksum on TDIG NodeID 0x" << hex << tdigNodeID
       << " through TCPU NodeID 0x" << tcpuNodeID
       << "\nfrom address 0x" << startAddr
       << " to 0x" << endAddr
       << " at devID " << dec << devID << "...\n";


 
  errno = 0;

  // install signal handler for manual break
  signal(SIGINT, signal_handler);
  

  if((errno = openCAN_l(devID)) != 0) {
    my_private_exit(errno);
  }

  
  // swallow any packets that might be present first
  // errno = LINUX_CAN_Read_Timeout(h, &mr, 100000); // timeout = 100 mseconds


  addr = startAddr;
  // first read needs to include the address
  ms.MSGTYPE = MSGTYPE_EXTENDED;
  ms.ID = (((tdigNodeID<<4) | 0x004) << 18) | tcpuNodeID;
  ms.LEN = 5;
  ms.DATA[0] = 0x4c;	// read MCU Data
  ms.DATA[1] = addr & 0x00FF;
  ms.DATA[2] = (addr >> 8) & 0x00FF;
  ms.DATA[3] = 0; // higher address bytes = 0 for now
  ms.DATA[4] = 0;
  
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "sending");
#endif
  if ( (errno = CAN_Write_l(h, &ms, devID)) ) {
    perror("x_getChecksum_l: CAN_Write_l()");
    return(errno);
  }
  errno = LINUX_CAN_Read_Timeout_l(h, &mr, 4000000); // timeout = 4 second
  if (errno != 0) {
    if (errno == CAN_ERR_QRCVEMPTY)
      cout << "Timeout during MCU memory read\n";
    else
      cout << "CAN_Read_Timeout_l returned " << errno 
	   << " during MCU memory read \n";
  }

  // check for correct response message ID
  expectedResponseID = ms.ID | (0x1 <<18);
  if ( mr.Msg.ID != expectedResponseID ) {
    cout << "ERROR: " 
	 << " request: Got something other than readResponse: ID " 
	 << showbase << hex << (unsigned int)mr.Msg.ID 
	 << ", expected response to " << (unsigned int)ms.ID << endl;	
    printCANMsg(mr.Msg, "response:");
    my_private_exit(-2);
  }
  
  // check for correct message length
  if (mr.Msg.LEN != 5) {
    cout << "ERROR: " 
	 << " request: Got msg with incorrect data length " 
	 << dec << (int)mr.Msg.LEN << ", expected 5\n";
    cout << "response: first byte: " 
	 << showbase << hex << (unsigned int)mr.Msg.DATA[0] 
	 << " expected " << (unsigned int)ms.DATA[0] << endl;
    printCANMsg(mr.Msg, "response");
    my_private_exit(-3);
  }

  // check for correct echo
  if (mr.Msg.DATA[0] != ms.DATA[0]) {
    cout << "ERROR: Command response: first byte: " 
	 << showbase << hex << (unsigned int)mr.Msg.DATA[0] 
	 << " expected " << (unsigned int)ms.DATA[0] << endl;
    printCANMsg(mr.Msg, "response:");
    my_private_exit(-4);
  }

#ifdef LOCAL_DEBUG
  printCANMsg(mr.Msg, "response");
#endif
  for (int i = 0; i<4; i+=2) {
    checksum += mr.Msg.DATA[i+1];
    checksum += mr.Msg.DATA[i+2];
    if ((addr+1) > endAddr) break;
  }

  // consecutive messages don't need the address, 
  // it is automatically increased by firmware
  ms.LEN = 1;
  if(endAddr > (startAddr+2)) {
    for (addr = startAddr+2; addr <= endAddr; addr += 2) {
#ifdef LOCAL_DEBUG
      cout << "addr = " << showbase << hex << addr;
      printCANMsg(ms, " sending");
#endif
      if ( (errno = CAN_Write_l(h, &ms, devID)) ) {
	perror("MCU2:BlockEnd: CAN_Write_l()");
	return(errno);
      }
      errno = LINUX_CAN_Read_Timeout_l(h, &mr, 4000000); // timeout = 4 second
      if (errno != 0) {
	if (errno == CAN_ERR_QRCVEMPTY)
	  cout << "Timeout during MCU memory read\n";
	else
	  cout << "CAN_Read_Timeout_l returned " << errno 
	       << " during MCU memory read \n";
      }
      
      // check for correct response message ID
      expectedResponseID = ms.ID | (0x1 <<18);
      if ( mr.Msg.ID != expectedResponseID ) {
	cout << "ERROR: " 
	     << " request: Got something other than readResponse: ID " 
	     << showbase << hex << (unsigned int)mr.Msg.ID 
	     << ", expected response to " << (unsigned int)ms.ID << endl;	
	printCANMsg(mr.Msg, "response:");
	my_private_exit(-2);
      }
      
      // check for correct message length
      if (mr.Msg.LEN != 5) {
	cout << "ERROR: " 
	     << " request: Got msg with incorrect data length " 
	     << dec << (int)mr.Msg.LEN << ", expected 5\n";
	cout << "response: first byte: " 
	     << showbase << hex << (unsigned int)mr.Msg.DATA[0] 
	     << " expected " << (unsigned int)ms.DATA[0] << endl;
	printCANMsg(mr.Msg, "response");
	my_private_exit(-3);
      }

      // check for correct echo
      if (mr.Msg.DATA[0] != ms.DATA[0]) {
	cout << "ERROR: Command response: first byte: " 
	     << showbase << hex << (unsigned int)mr.Msg.DATA[0] 
	     << " expected " << (unsigned int)ms.DATA[0] << endl;
	printCANMsg(mr.Msg, "response:");
	my_private_exit(-4);
      }
      
#ifdef LOCAL_DEBUG
      printCANMsg(mr.Msg, "response");
#endif
      for (int i = 0; i<4; i+=2) {
	checksum += mr.Msg.DATA[i+1];
	checksum += mr.Msg.DATA[i+2];
	if ((addr+1) > endAddr) break;
      }

      
    }
  }

  cout << "\nChecksum = " << showbase << hex << checksum;
  if ( (expectedChecksum != 0) && (checksum != expectedChecksum)) {
    cout << " " << RED_ON_WHITE << "?? checksum don't match, expected " 
	 << expectedChecksum << NORMAL_COLORS << endl;
  }
  cout << endl;

  errno = 0;
  my_private_exit(errno);
  
  return 0;
}

//****************************************************************************
int main(int argc, char *argv[])
{
  unsigned int tdigNodeID, tcpuNodeID;
  WORD devID = 255;
  unsigned short startAddr, endAddr;
  unsigned short expectedChecksum = 0;

  cout << vcid << endl;
  cout.flush();

  if ( argc < 3 ) {
    cout << "USAGE: " << argv[0] << " <TDIG node ID> <TCPU node ID> [<startAddress> <endAddress>] [<devID>]\n";
    return 1;
  }
  
  tdigNodeID = strtol(argv[1], (char **)NULL, 0);
  if ((tdigNodeID < 0x10) || (tdigNodeID > 0x17)) {
    cerr << "TDIG nodeID = " << tdigNodeID 
	 << " is an invalid entry.  Use a value between 0x10 and 0x17 instead."  
	 << endl;
    return -1;
  }
  
  tcpuNodeID = strtol(argv[2], (char **)NULL, 0);
  if ((tcpuNodeID < 0x20) || (tcpuNodeID > 0x3F)) {
    cerr << "TCPU nodeID = " << tcpuNodeID 
	 << " is an invalid entry.  Use a value between 0x20 and 0x3F instead."  
	 << endl;
    return -1;
  }
  
  if (argc == 7) {
    expectedChecksum = (unsigned short)strtol(argv[6],(char **)NULL, 0);
    if (devID > 255) {
      printf("Invalid Device ID 0x%x. Use a device ID between 0 and 255\n", devID);
      return -1;
    }
  }

  if (argc >= 6) {
    devID = strtol(argv[5],(char **)NULL, 0);
    if (devID > 255) {
      printf("Invalid Device ID 0x%x. Use a device ID between 0 and 255\n", devID);
      return -1;
    }
  }

  startAddr = 0x0000;
  endAddr = 0x3FFF;

  if (argc > 4) {
    startAddr = strtol(argv[3],(char **)NULL, 0);
    endAddr = strtol(argv[4],(char **)NULL, 0);

    if (endAddr <= startAddr) {
      printf("endAddress must be > startAddress\n");
      return -1;
    }
  }

#ifdef LOCAL_DEBUG
  cout << "TDIG nodeID 0x" << hex << tdigNodeID
       << " TCPU nodeID 0x" << tcpuNodeID
       << " filename " << argv[2]
       << " startAddr 0x" << startAddr
       << " endAddr 0x" << endAddr
       << " devID 0x" << devID << endl;
#endif

  return getChecksum(tdigNodeID, tcpuNodeID, startAddr, endAddr, 
		     devID, expectedChecksum );
}

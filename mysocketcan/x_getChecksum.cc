/* File name     : x_getChecksum.cc
 * Creation date : 7/3/2012
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
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

// local utilities
#include "can_utils.h"

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
		int devID)
{
  unsigned int addr;
  struct can_frame ms;
  struct can_frame mr;
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
       << " at devID 0x" << hex << devID << "...\n";

  errno = 0;

  // install signal handler for manual break
  signal(SIGINT, signal_handler);

  if((h = CAN_Open(devID)) < 0) {
    my_private_exit(-1);
  }

  
  // swallow any packets that might be present first
  errno = CAN_Read_Timeout(h, &mr, 100000); // timeout = 100 mseconds


  addr = startAddr;
  // first read needs to include the address
  ms.can_id = (((tdigNodeID<<4) | 0x004) << 18) | tcpuNodeID | CAN_EFF_FLAG;
  ms.can_dlc = 5;
  ms.data[0] = 0x4c;	// read MCU Data
  ms.data[1] = addr & 0x00FF;
  ms.data[2] = (addr >> 8) & 0x00FF;
  ms.data[3] = 0; // higher address bytes = 0 for now
  ms.data[4] = 0;
  
#ifdef LOCAL_DEBUG
  printCANMsg(ms, "sending");
#endif
  if (write(h, &ms, sizeof(ms)) != sizeof(ms)) {
    perror("x_getChecksum:write()");
    return -1;
  }
  errno = CAN_Read_Timeout(h, &mr, 1000000); // timeout = 1 second
  if (errno < 0) {
    cout << "CAN_Read_Timeout returned " << errno 
	 << " during MCU memory read\n";
    return -1;
  }
  else if (errno == 0)
    cout << "Timeout during MCU memory read\n";

  // check for correct response message ID
  expectedResponseID = ms.can_id | (0x1 <<18);
  if ( mr.can_id != expectedResponseID ) {
    cout << "ERROR: " 
	 << " request: Got something other than readResponse: ID " 
	 << showbase << hex << (unsigned int)(mr.can_id & CAN_EFF_MASK) 
	 << ", expected response to " << (unsigned int)(ms.can_id & CAN_EFF_MASK) << endl;	
    printCANMsg(mr, "response:");
    my_private_exit(-2);
  }
  
  // check for correct message length
  if (mr.can_dlc != 5) {
    cout << "ERROR: " 
	 << " request: Got msg with incorrect data length " 
	 << dec << (int)mr.can_dlc << ", expected 5\n";
    cout << "response: first byte: " 
	 << showbase << hex << (unsigned int)mr.data[0] 
	 << " expected " << (unsigned int)ms.data[0] << endl;
    printCANMsg(mr, "response");
    my_private_exit(-3);
  }

  // check for correct echo
  if (mr.data[0] != ms.data[0]) {
    cout << "ERROR: Command response: first byte: " 
	 << showbase << hex << (unsigned int)mr.data[0] 
	 << " expected " << (unsigned int)ms.data[0] << endl;
    printCANMsg(mr, "response:");
    my_private_exit(-4);
  }

#ifdef LOCAL_DEBUG
  printCANMsg(mr.Msg, "response");
#endif
  for (int i = 0; i<4; i+=2) {
    checksum += mr.data[i+1];
    checksum += mr.data[i+2];
    if ((addr+1) > endAddr) break;
  }

  // consecutive messages don't need the address, 
  // it is automatically increased by firmware
  ms.can_dlc = 1;
  if(endAddr > (startAddr+2)) {
    for (addr = startAddr+2; addr <= endAddr; addr += 2) {
#ifdef LOCAL_DEBUG
      cout << "addr = " << showbase << hex << addr;
      printCANMsg(ms, " sending");
#endif
      if (write(h, &ms, sizeof(ms)) != sizeof(ms)) {
	perror("x_getChecksum:write()");
	return -1;
      }
      errno = CAN_Read_Timeout(h, &mr, 4000000); // timeout = 4 second
      if (errno < 0) {
	cout << "CAN_Read_Timeout returned " << errno 
	     << " during MCU memory read\n";
	return -1;
      }
      else if (errno == 0)
	cout << "Timeout during MCU memory read\n";

      // check for correct response message ID
      expectedResponseID = ms.can_id | (0x1 <<18);
      if ( mr.can_id != expectedResponseID ) {
      cout << "ERROR: " 
	   << " request: Got something other than readResponse: ID " 
	   << showbase << hex << (unsigned int)(mr.can_id & CAN_EFF_MASK) 
	   << ", expected response to " << (unsigned int)(ms.can_id & CAN_EFF_MASK) << endl;	
	printCANMsg(mr, "response:");
	my_private_exit(-2);
      }
      
      // check for correct message length
      if (mr.can_dlc != 5) {
	cout << "ERROR: " 
	     << " request: Got msg with incorrect data length " 
	     << dec << (int)mr.can_dlc << ", expected 5\n";
	cout << "response: first byte: " 
	     << showbase << hex << (unsigned int)mr.data[0] 
	     << " expected " << (unsigned int)ms.data[0] << endl;
	printCANMsg(mr, "response");
	my_private_exit(-3);
      }

      // check for correct echo
      if (mr.data[0] != ms.data[0]) {
	cout << "ERROR: Command response: first byte: " 
	     << showbase << hex << (unsigned int)mr.data[0] 
	     << " expected " << (unsigned int)ms.data[0] << endl;
	printCANMsg(mr, "response:");
	my_private_exit(-4);
      }
      
#ifdef LOCAL_DEBUG
      printCANMsg(mr.Msg, "response");
#endif
      for (int i = 0; i<4; i+=2) {
	checksum += mr.data[i+1];
	checksum += mr.data[i+2];
	if ((addr+1) > endAddr) break;
      }

      
    }
  }

  cout << "\nChecksum = " << showbase << hex << checksum << endl;

  errno = 0;
  my_private_exit(errno);
  
  return 0;
}

//****************************************************************************
int main(int argc, char *argv[])
{
  unsigned int tdigNodeID, tcpuNodeID;
  int devID = 0;
  unsigned short startAddr, endAddr;


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
  
  if (argc == 6) {
    devID = strtol(argv[5],(char **)NULL, 0);
    if (devID > 7) {
      printf("Invalid Device ID 0x%x. Use a device ID between 0 and 7\n", devID);
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

  return getChecksum(tdigNodeID, tcpuNodeID, startAddr, endAddr, devID);
}

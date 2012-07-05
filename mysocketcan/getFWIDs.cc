/* File name     : getFWIDs.cc
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
using namespace std;

// other headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include "can_utils.h"

//****************************************************************************
// DEFINES
// #define LOCAL_DEBUG

//****************************************************************************
// GLOBALS

//****************************************************************************
// CODE 

// here is where all is done
int getFWIDs(int tcpuNodeID, int devID)
{
  int tdigNodeID;
  unsigned short MCUfwID;

  int cansock; /* can raw socket */ 
  int nbytes;
  struct can_frame sframe, rframe;

  int nMicroSeconds = 1000000; // timeout = 1 sec
  int err;

  if ((cansock =  CAN_Open(devID)) < 0) {
    cout << "Problem opening devID " << devID << endl;
    return 1;
  }

  // CANbus data: get firmware IDs
  sframe.data[0] = 0xb1;
  sframe.can_dlc = 1;

  // -----------------------------------------
  // first all  of the TDIGs

  for (int tdignum = 0; tdignum<8; tdignum++) {
    tdigNodeID = 0x10 + tdignum;

    // create the CANbus message ID and data
    sframe.can_id = tdigNodeID & 0x3f;
    sframe.can_id = (sframe.can_id<<4) | 0x004;
    // now add extended msg ID from TCPU nodeID
    sframe.can_id = (sframe.can_id<<18) | tcpuNodeID | CAN_EFF_FLAG;
#ifdef LOCAL_DEBUG
    cout << "Message ID = " << showbase << hex << sframe.can_id << endl;
#endif
    
    // send the "GET_FIRMWARE_ID" CANbus HLP message
#ifdef LOCAL_DEBUG
    cout << "sending command\n";
#endif
    // send frame
    if ((nbytes = write(cansock, &sframe, sizeof(sframe))) != sizeof(sframe)) {
      perror("write");
      return 1;
    }

    err = CAN_Read_Timeout(cansock, &rframe, nMicroSeconds);

    if (err  < 0) { // error
      cout << "CAN_Read_Timeout returned " << err << endl;
      return 1;
    }
    else if (err == 0) { // timeout
      cout << "TDIG " << showbase << hex << tdigNodeID
	   << ": did not respond" << endl;
    }
    else {
      // print firmware IDs
      MCUfwID = rframe.data[1] + (rframe.data[2] << 8);
      cout << "TDIG " << showbase << hex << tdigNodeID
	   << ": MCU Firmware ID = " << MCUfwID
	   << " (\"" << noshowbase << (unsigned short)rframe.data[2] << (char)rframe.data[1]
	   << "\"); FPGA Firmware ID = "
	   << showbase << (unsigned short)rframe.data[3] << endl;
    }
  } // for (int tdignum...

  // --------------------------------------
  // Now do the TCPU:

  sframe.can_id = (tcpuNodeID<<4) | 0x004;
#ifdef LOCAL_DEBUG
  cout << "Message ID = " << hex << sframe.can_id << endl;
#endif

  // send the "GET_FIRMWARE_ID" CANbus HLP message
#ifdef LOCAL_DEBUG
  cout << "sending command\n";
#endif

  /* send frame */
  if ((nbytes = write(cansock, &sframe, sizeof(sframe))) != sizeof(sframe)) {
    perror("write");
    return 1;
  }

  err = CAN_Read_Timeout(cansock, &rframe, nMicroSeconds);
    
  if (err  < 0) { // error
    cout << "CAN_Read_Timeout returned " << err << endl;
    return 1;
  }
  else if (err == 0) { // timeout
    cout << "TCPU " << showbase << hex << tcpuNodeID
	 << ": did not respond" << endl;
  }
  else {
    // print firmware IDs
    MCUfwID = rframe.data[1] + (rframe.data[2] << 8);
    cout << "\nTCPU " << showbase << hex << tcpuNodeID
	 << ": MCU Firmware ID = " << MCUfwID
	 << " (\"" << noshowbase << (unsigned short)rframe.data[2] << (char)rframe.data[1]
	 << "\"); FPGA Firmware ID = "
	 << showbase << (unsigned short)rframe.data[3] << endl;
  }

  // --------------------------  

  close(cansock);
  return(0);
}



//****************************************************************************
// main entry point
int main(int argc, char *argv[])
{
  int devID;

  cout << vcid << endl;
  cout.flush();
  
  if ( argc < 2 ) {
    cout << "USAGE: " << argv[0] << " <TCPU nodeID> [<devID>]\n";
    return 1;
  }
  
  int tcpuNodeID = strtol(argv[1], (char **)NULL, 0);
  if ((tcpuNodeID < 1) || (tcpuNodeID > 0x3f)) {
    cerr << "tcpuNodeID = " << tcpuNodeID << " invalid entry. Use 0x1..0x3f (63) instead." << endl;
    return -1;
  }

  if (argc == 3) {
    devID = strtol(argv[2], (char **)NULL, 0);
    if ((devID < 0) || (devID > 7)) {
      cerr << "devID = " << devID << " invalid entry. Use 0..7 instead." << endl;
      return -1;
    }
  }
  else
    devID = 0;

  return getFWIDs(tcpuNodeID, devID);
}

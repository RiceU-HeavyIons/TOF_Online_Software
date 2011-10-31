/* File name     : getFWIDs.cc
 * Creation date : 8/4/2008
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: getFWIDs.cc,v 1.4 2011-10-31 14:43:51 jschamba Exp $";
#endif /* lint */

//****************************************************************************
// INCLUDES
// C++ header file
#include <iostream>
using namespace std;

// other headers
#include <stdlib.h>
#include <string.h>
#include <libpcan.h>
#include "can_utils.h"

//****************************************************************************
// DEFINES
// #define LOCAL_DEBUG

//****************************************************************************
// GLOBALS
HANDLE h = NULL;

//****************************************************************************
// CODE 


//****************************************************************************
// here is where all is done
int getFWIDs(int tcpuNodeID, int devID)
{
  int tdigNodeID;
  unsigned short MCUfwID;
  int errno;
  TPCANMsg ms;
  TPCANRdMsg mr;

  openCAN_l(devID);


  ms.LEN = 1;
  ms.DATA[0] = 0xb1;

  // -----------------------------------------
  // first all  of the TDIGs

  for (int tdignum = 0; tdignum<8; tdignum++) {
    ms.MSGTYPE = MSGTYPE_EXTENDED;
    tdigNodeID = 0x10 + tdignum;

    // create the CANbus message ID and data
    ms.ID = tdigNodeID & 0x3f;
    ms.ID = (ms.ID<<4) | 0x004;
    // now add extended msg ID from TCPU nodeID
    ms.ID = (ms.ID<<18) | tcpuNodeID;
#ifdef LOCAL_DEBUG
    cout << "Message ID = " << showbase << hex << ms.ID << endl;
#endif
    
    
    
    // send the "GET_FIRMWARE_ID" CANbus HLP message
#ifdef LOCAL_DEBUG
    cout << "sending command\n";
#endif

    CAN_Write_l(h, &ms, devID);
    
    if ( (errno = LINUX_CAN_Read_Timeout_l(h, &mr, 1000000)) != 0) { // timeout = 1 sec
#ifdef LOCAL_DEBUG
      cout << "LINUX_CAN_Read_Timeout_l returned " << errno << endl;
#endif

      cout << "TDIG " << showbase << hex << tdigNodeID
	   << ": did not respond" << endl;
      
//       CAN_Close_l(h);
//       return -1;
    }
    else {
    
      // print firmware IDs
      MCUfwID = mr.Msg.DATA[1] + (mr.Msg.DATA[2] << 8);
      cout << "TDIG " << showbase << hex << tdigNodeID
	   << ": MCU Firmware ID = " << MCUfwID
	   << " (\"" << noshowbase << (unsigned short)mr.Msg.DATA[2] << (char)mr.Msg.DATA[1]
	   << "\"); FPGA Firmware ID = "
	   << showbase << (unsigned short)mr.Msg.DATA[3] << endl;
    }
  } // for (int tdignum...

  // --------------------------------------
  // Now do the TCPU:

  ms.MSGTYPE = MSGTYPE_STANDARD;
  ms.ID = (tcpuNodeID<<4) | 0x004;
#ifdef LOCAL_DEBUG
  cout << "Message ID = " << hex << ms.ID << endl;
#endif

  // send the "GET_FIRMWARE_ID" CANbus HLP message
#ifdef LOCAL_DEBUG
  cout << "sending command\n";
#endif
    
  CAN_Write_l(h, &ms, devID);
  
  if ( (errno = LINUX_CAN_Read_Timeout_l(h, &mr, 1000000)) != 0) { // timeout = 1 sec
#ifdef LOCAL_DEBUG
    cout << "LINUX_CAN_Read_Timeout_l returned " << errno << endl;
#endif
    CAN_Close_l(h);
    return -1;
  }
    
  
  // print firmware IDs
  MCUfwID = mr.Msg.DATA[1] + (mr.Msg.DATA[2] << 8);
  cout << "\nTCPU " << showbase << hex << tcpuNodeID
       << ": MCU Firmware ID = " << MCUfwID
       << " (\"" << noshowbase << (unsigned short)mr.Msg.DATA[2] << (char)mr.Msg.DATA[1]
       << "\"); FPGA Firmware ID = "
       << showbase << (unsigned short)mr.Msg.DATA[3] << endl;
  
  // --------------------------  
  // tell pcanloop that we no longer want response messages
  CAN_Close_l(h);

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
    if ((devID < 1) || (devID > 255)) {
      cerr << "devID = " << devID << " invalid entry. Use 1..255 instead." << endl;
      return -1;
    }
  }
  else
    devID = 255;

  return getFWIDs(tcpuNodeID, devID);
}

/* File name     : getTHUBFWIDs.cc
 * Creation date : 10/6/2008
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
int getFWIDs(int devID)
{
  unsigned int fwID;
  int errno;
  TPCANMsg ms;
  TPCANRdMsg mr;

  openCAN_l(devID);

  ms.MSGTYPE = MSGTYPE_STANDARD;
  ms.ID = 0x404;
  ms.LEN = 2;
  ms.DATA[0] = 0x2;

  // -----------------------------------------
  // first all  of the FPGAs

  for (int fpganum = 0; fpganum<9; fpganum++) {
    ms.DATA[1] = fpganum;
    
    
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
    fwID = mr.Msg.DATA[0] + (mr.Msg.DATA[1] << 8) + (mr.Msg.DATA[2] << 16) + (mr.Msg.DATA[3] << 24);
    cout << "FPGA " << dec << fpganum
	 << ": Firmware User ID = " << showbase << hex << fwID << endl;
  } // for (int fpganum...

  // --------------------------------------
  // Now do the MCU:

  ms.LEN = 1;
  ms.DATA[0] = 0x1;
  CAN_Write_l(h, &ms, devID);
  
  if ( (errno = LINUX_CAN_Read_Timeout_l(h, &mr, 1000000)) != 0) { // timeout = 1 sec
#ifdef LOCAL_DEBUG
    cout << "LINUX_CAN_Read_Timeout_l returned " << errno << endl;
#endif
    CAN_Close_l(h);
    return -1;
  }
  
  // print firmware IDs
  fwID = mr.Msg.DATA[0] + (mr.Msg.DATA[1] << 8);
  cout << "\nMCU Firmware ID = " << showbase << hex << fwID
       << endl;

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
  
  
  if (argc == 2) {
    devID = strtol(argv[1], (char **)NULL, 0);
    if ((devID < 1) || (devID > 255)) {
      cerr << "devID = " << devID << " invalid entry. Use 1..255 instead." << endl;
      return -1;
    }
  }
  else
    devID = 255;

  return getFWIDs(devID);
}

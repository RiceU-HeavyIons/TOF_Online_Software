/* File name     : getTHUBFWIDs.cc
 * Creation date : 7/2/2012
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: getTHUBFWIDs.cc 756 2012-07-05 20:44:12Z jschamba $";
#endif /* lint */

//****************************************************************************
// INCLUDES
// C++ header file
#include <iostream>
using namespace std;

// other headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

// pcan include file
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include "can_utils.h"


//****************************************************************************
// DEFINES
// #define LOCAL_DEBUG

//****************************************************************************
// GLOBAL
int h = -1;

//****************************************************************************
// CODE 


//****************************************************************************
// here is where all is done
int getFWIDs(int devID)
{
  unsigned int fwID;
  int errno;
  struct can_frame ms;
  struct can_frame mr;

  if((h = CAN_Open(devID)) < 0) {
    return -1;
  }

  ms.can_id = 0x404;
  ms.can_dlc = 2;
  ms.data[0] = 0x2;

  // -----------------------------------------
  // first all  of the FPGAs

  for (int fpganum = 0; fpganum<9; fpganum++) {
    ms.data[1] = fpganum;
    
    
    // send the "GET_FIRMWARE_ID" CANbus HLP message
#ifdef LOCAL_DEBUG
    cout << "sending command\n";
#endif
    
    if (write(h, &ms, sizeof(ms)) != sizeof(ms)) {
      perror("getChecksum: write");
      return -1;
    }

    errno = CAN_Read_Timeout(h, &mr, 1000000); // timeout = 1 second
    if (errno < 0) {
      cout << "CAN_Read_Timeout returned " << errno << endl;
      close(h);
      return -1;
    }
    else if (errno == 0) {
      cout << "Timeout during MCU memory read\n";
      close(h);
      return -1;
    }
    
    
    // print firmware IDs
    fwID = mr.data[0] + (mr.data[1] << 8) + (mr.data[2] << 16) + (mr.data[3] << 24);
    cout << "FPGA " << dec << fpganum
	 << ": Firmware User ID = " << showbase << hex << fwID << endl;
  } // for (int fpganum...

  // --------------------------------------
  // Now do the MCU:

  ms.can_dlc = 1;
  ms.data[0] = 0x1;
  if (write(h, &ms, sizeof(ms)) != sizeof(ms)) {
    perror("getChecksum: write");
    return -1;
  }
  
  errno = CAN_Read_Timeout(h, &mr, 1000000); // timeout = 1 second
  if (errno < 0) {
    cout << "CAN_Read_Timeout returned " << errno << endl;
    close(h);
    return -1;
  }
  else if (errno == 0) {
    cout << "Timeout during MCU memory read\n";
    close(h);
    return -1;
  }
    
  
  // print firmware IDs
  fwID = mr.data[0] + (mr.data[1] << 8);
  cout << "\nMCU Firmware ID = " << showbase << hex << fwID
       << endl;

  close(h);

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
    if ((devID < 0) || (devID > 7)) {
      cerr << "devID = " << devID << " invalid entry. Use 0..7 instead." << endl;
      return -1;
    }
  }
  else
    devID = 0;

  return getFWIDs(devID);
}

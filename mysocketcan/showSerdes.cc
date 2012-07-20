/* File name     : showSerdes.cc
 * Creation date : 7/3/2012
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

// pcan include file
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include "can_utils.h"


//****************************************************************************
// DEFINES
// #define LOCAL_DEBUG

//****************************************************************************
// GLOBALS
int h = -1;

//****************************************************************************
// CODE 


//****************************************************************************
// here is where all is done
int showSerdes(int devID)
{
  int errno;
  struct can_frame ms;
  struct can_frame mr;

  if((h = CAN_Open(devID)) < 0) {
    return 1;
  }

  ms.can_id = 0x404;
  ms.can_dlc = 1;

  // -----------------------------------------
  // first all  of the FPGAs

  for (unsigned int serdesnum = 0x91; serdesnum<0x99; serdesnum++) {
    ms.data[0] = serdesnum;
    
    
    // send the "GET_FIRMWARE_ID" CANbus HLP message
#ifdef LOCAL_DEBUG
    cout << "sending command\n";
#endif
    
    if (write(h, &ms, sizeof(ms)) != sizeof(ms)) {
      perror("showSerdes:write()");
      return -1;
    }

    if ( (errno = CAN_Read_Timeout(h, &mr, 1000000)) <= 0) { // timeout = 1 sec
#ifdef LOCAL_DEBUG
      cout << "CAN_Read_Timeout returned " << errno << endl;
#endif
      close(h);
      return -1;
    }
    
    
    // print response
    cout << "Serdes " << (char)(serdesnum - 0x50)
	 << " = " << showbase << hex << (unsigned int)mr.data[0] << endl;
  } // for (int serdesnum...


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

  return showSerdes(devID);
}

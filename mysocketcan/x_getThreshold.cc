/* File name     : x_getThreshold.cc
 * Creation date : 11/14/2012
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: x_getThreshold.cc 793 2012-11-14 21:52:22Z jschamba $";
#endif /* lint */

// #define LOCAL_DEBUG

//****************************************************************************
// INCLUDES
// C++ header file
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
using namespace std;

// other headers
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// pcan include file
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

// local utilities
#include "can_utils.h"

// use one of the two following definition for debug printouts:
// const bool debug = true;
const bool debug = false;

//****************************************************************************
int main(int argc, char *argv[])
{
  unsigned int tdigNodeID, tcpuNodeID, devID;
  double tVal;
  int decVal;

  if (debug) cout << vcid << endl;
  cout.flush();

  if ( argc < 3 ) {
    cout << "USAGE: " << argv[0] << " <devID> <TDIG node ID> <TCPU node ID>\n";
    return 1;
  }
  
  devID      = strtol(argv[1], (char **)NULL, 0);
  tdigNodeID = strtol(argv[2], (char **)NULL, 0);
  tcpuNodeID = strtol(argv[3], (char **)NULL, 0);

  int h; 
  if((h = CAN_Open(devID)) < 0) {
    return 1;
  }

  struct can_frame ms;
  struct can_frame mr;

  ms.can_id = (0x004 | (tdigNodeID << 4))<<18 | tcpuNodeID | CAN_EFF_FLAG;  // Write Message
  ms.can_dlc = 1;
  ms.data[0] = 0x8;
  
  if (debug)
    cout << "TDIG nodeID = " << tdigNodeID
	 << ", TCPU nodeID = " << tcpuNodeID
	 << " msgID = " << showbase << hex << ms.can_id
	 << " DATA[0] = " << (unsigned int)ms.data[0] << endl;

  if (write(h, &ms, sizeof(ms)) != sizeof(ms)) {
    perror("x_readHPTDCconfig:write()");
    close(h);
    return -1;
  }
  
  if ( (errno = CAN_Read_Timeout(h, &mr, 1000000)) < 0) { // timeout = 1 sec
#ifdef LOCAL_DEBUG
    cout << "CAN_Read_Timeout returned " << errno << endl;
#endif
    close(h);
    return -1;
  }

  close(h);

  if (mr.can_dlc != 3) {
    cout << "Wrong message received. Length = " << mr.can_dlc << endl;
    return -1;
  }

  if (debug)
    cout << " Message received: msgID = " << showbase << hex << ms.can_id
	 << " DATA[0] = " << (unsigned int)ms.data[0] 
	 << " DATA[1] = " << (unsigned int)ms.data[1] 
	 << " DATA[2] = " << (unsigned int)ms.data[2] 
	 << endl;

  decVal = mr.data[1] + (mr.data[2]<<8);
  tVal = (double)decVal * 3300.0 / 4095.0;

  cout << "Threshold = " << (int)tVal << " mV" << endl;

  return 0;
}

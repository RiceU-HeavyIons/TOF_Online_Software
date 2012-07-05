/* File name     : x_enableChannel.cc
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
#include <iomanip>
#include <fstream>
#include <sstream>
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
  string cmdString;
  stringstream ss;

  bool enableCh[8] = {false, false, false, false, false, false, false, false};
  //char cmdString[255];

  if (debug) cout << vcid << endl;
  cout.flush();

  if ( argc < 5 ) {
    cout << "USAGE: " << argv[0] << " <devID> <TDIG node ID> <TCPU node ID> <TDCnum> [<channel1> <channel2> ...]\n";
    return 1;
  }
  
  int devID = strtol(argv[1], (char **)NULL, 0);
  if ((devID < 0) || (devID > 7)) {
    cerr << "devID = " << devID << " invalid entry. Use 0..7 instead." << endl;
    return -1;
  }

  int tdigNodeID = strtol(argv[2], (char **)NULL, 0);
  if ((tdigNodeID < 1) || (tdigNodeID > 0x3f)) {
    cerr << "tdigNodeID = " << tdigNodeID << " invalid entry. Use 0x1..0x3f (63) instead." << endl;
    return -1;
  }

  int tcpuNodeID = strtol(argv[3], (char **)NULL, 0);
  if ((tcpuNodeID < 1) || (tcpuNodeID > 0x3f)) {
    cerr << "tcpuNodeID = " << tcpuNodeID << " invalid entry. Use 0x1..0x3f (63) instead." << endl;
    return -1;
  }

  int tdcNum = atoi(argv[4]);
  if ((tdcNum < 1) || (tdcNum > 3)) {
    cerr << "TDC # = " << tdcNum << " invalid entry. Use 1..3 instead." << endl;
    return -1;
  }

  if (argc > 5) {
    for (int i=5; i<argc; i++) {
      int chNum = strtol(argv[i], (char **)NULL, 0);
      enableCh[chNum] = true;
    }
  }

  if (debug) {
    cout << "tdigNodeID = " << tdigNodeID
	 << ", tcpuNodeID = " << tcpuNodeID
	 << ", tdc = " << tdcNum;
    for (int i=0; i<8; i++) 
      cout << ", ch " << i << " = " << (enableCh[i] ? "e" : "d");
    cout << endl;
  }

  int h; 
  if((h = CAN_Open(devID)) < 0) {
    return 1;
  }

  struct can_frame ms;

  // build the message

  ms.can_id = (0x002 | (tdigNodeID << 4))<<18 | tcpuNodeID | CAN_EFF_FLAG;  // Write Message
  ms.can_dlc = 6;
  ms.data[0] = 0x4 + tdcNum;
  ms.data[1] = 0x4 + (enableCh[0] ? 0xe0 : 0);
  ms.data[2] = (enableCh[0] ? 0x1 : 0) + (enableCh[1] ? 0x1e : 0) + (enableCh[2] ? 0xe0 : 0);
  ms.data[3] = (enableCh[2] ? 0x1 : 0) + (enableCh[3] ? 0x1e : 0) + (enableCh[4] ? 0xe0 : 0);
  ms.data[4] = (enableCh[4] ? 0x1 : 0) + (enableCh[5] ? 0x1e : 0) + (enableCh[6] ? 0xe0 : 0);
  ms.data[5] = (enableCh[6] ? 0x1 : 0) + (enableCh[7] ? 0x1e : 0) + 0x80;

  sendCAN_and_Compare(h, ms, "xsetThreshold", 4000000, 2, true); // timeout 4sec

  close(h);

  return 0;
}

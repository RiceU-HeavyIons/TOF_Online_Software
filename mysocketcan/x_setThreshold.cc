/* File name     : x_setThreshold.cc
 * Creation date : 7/3/2012
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: x_setThreshold.cc 756 2012-07-05 20:44:12Z jschamba $";
#endif /* lint */

// #define LOCAL_DEBUG

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
  unsigned int tdigNodeID, tcpuNodeID, devID;
  double tVal;
  string cmdString;
  stringstream ss;
  //char cmdString[255];

  if (debug) cout << vcid << endl;
  cout.flush();

  if ( argc < 4 ) {
    cout << "USAGE: " << argv[0] << " <devID> <TDIG node ID> <TCPU node ID> <value>\n";
    return 1;
  }
  
  devID      = strtol(argv[1], (char **)NULL, 0);
  tdigNodeID = strtol(argv[2], (char **)NULL, 0);
  tcpuNodeID = strtol(argv[3], (char **)NULL, 0);

  tVal = atof(argv[4]);

  if ((tVal < 0) || (tVal > 3300)) {
    printf("ERROR: Input voltage %f is out of range.  Voltage must be between 0 mV and 3300 mV.\n",
	   tVal);
    return 1;
  }

  /* TDIG-D uses a different DAC (TI DAC7571)
   * Ideal conversion per datasheet page 12 is
   *       Vout = Vdd * (DAC/4095)
   * where Vout is resulting output voltage
   *       Vdd is nominal supply voltage (3.3V)
   *       DAC is DAC setting
   * We are supplied here with desired voltage (millivolts) and need to determine D.
   * Rearranging: DAC = (integer) Vout/Vdd * 4095
   * Vdd = 3300 mV,
   */
  double D = (tVal * (4095.0 / 3300.0)) + 0.5; // 0.5 for rounding
  int decVal = (int)D;
  decVal &= 0xFFF;   // mask to 12 bits, bits PD0, PD1 are always zero (not pwr'd down)
  

  int h; 
  if((h = CAN_Open(devID)) < 0) {
    return 1;
  }

  struct can_frame ms;

  ms.can_id = (0x002 | (tdigNodeID << 4))<<18 | tcpuNodeID | CAN_EFF_FLAG;  // Write Message
  ms.can_dlc = 3;
  ms.data[0] = 0x8;
  ms.data[1] = decVal & 0xFF;
  ms.data[2] = (decVal>>8) & 0x0F;

  if (debug)
    cout << "TDIG nodeID = " << tdigNodeID
	 << ", TCPU nodeID = " << tcpuNodeID
	 << ", thresholdVal = " << tVal
	 << "mV;  msgID = " << showbase << hex << ms.can_id
	 << " DATA[0] = " << (unsigned int)ms.data[0]
	 << " DATA[1] = " << (unsigned int)ms.data[1]
	 << " DATA[2] = " << (unsigned int)ms.data[2] << endl;

  sendCAN_and_Compare(h, ms, "x_setThreshold", 4000000, 2, true); // timeout 4sec
  
  close(h);

  return 0;
}

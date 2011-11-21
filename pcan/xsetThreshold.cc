/* File name     : xsetThreshold.cc
 * Creation date : 10/05/07
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id$";
#endif /* lint */

// #define LOCAL_DEBUG
#define TDIG_D

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

#ifdef TDIG_D
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
  
  unsigned int   ID = (0x002 | (tdigNodeID << 4))<<18 | tcpuNodeID;  // Write Message
  unsigned short DATA0 = 0x8;
  unsigned short DATA1 = decVal & 0x00FF;
  unsigned short DATA2 = (decVal>>8) & 0x000F;

  if (debug)
    cout << "TDIG nodeID = " << tdigNodeID
	 << ", TCPU nodeID = " << tcpuNodeID
	 << ", thresholdVal = " << tVal
	 << "mV;  msgID = " << showbase << hex << ID
	 << " DATA[0] = " << DATA0
	 << " DATA[1] = " << DATA1
	 << " DATA[2] = " << DATA2 << endl;


  ss << "./pc \"m e " << showbase << hex << ID
     << " 3 " << DATA0
     << " " << DATA1
     << " " << DATA2
     << " " << dec << devID
     << "\"";

#else // Run 5 TDIG
  if ((nodeID < 0) || (nodeID > 7)) { 
    cerr << "nodeID = " << nodeID 
	 << " is an invalid entry.  Use a value between 0 and 7 instead.\n";
    return -1;
  }

  if ((tVal < -100) || (tVal > 100)) {
    printf("ERROR: Input voltage %d is out of range.  Voltage must be between -100 mV and 100 mV.\n",
	   tVal);
    return 1;
  }

  double D = -20.003*tVal + 2030.5; // the result of the equation
  int decVal = int(D); // the decimal value D gets truncated to
  
  unsigned short ID = 0x600 | nodeID;
  unsigned short DATA0 = (decVal>>8) & 0x000F;
  unsigned short DATA1 = decVal & 0x00FF;

  if (debug)
    cout << "nodeID = " << nodeID
	 << ", thresholdVal = " << tVal
	 << "mV;  msgID = " << showbase << hex << ID
	 << " DATA[0] = " << DATA0
	 << " DATA[1] = " << DATA1 << endl;


  //sprintf(cmdString, "./pc \"m s 0x%x 2 0x%x 0x%x\"", ID, DATA0, DATA1);

  ss << "./pc \"m s " << showbase << hex << ID
     << " 2 " << DATA0
     << " " << DATA1 << "\"";
#endif

  // now send the message:
  cmdString = ss.str();

  if (debug) cout << "cmdString = " << cmdString << endl;

  int status = system(cmdString.c_str());
  if (debug) cout << "\"system\" call returned " << status << endl;

  return 0;
}

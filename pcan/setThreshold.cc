/* File name     : setThreshold.cc
 * Creation date : 1/21/05
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: setThreshold.cc,v 1.2 2005-01-24 21:47:53 jschamba Exp $";
#endif /* lint */

#define LOCAL_DEBUG

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
const bool debug = true;
//const bool debug = false;

//****************************************************************************
int main(int argc, char *argv[])
{
  unsigned int nodeID;
  double tVal;
  //char cmdString[255];

  if (debug) cout << vcid << endl;
  cout.flush();

  if ( argc < 3 ) {
    cout << "USAGE: " << argv[0] << " <node ID> <value>\n";
    return 1;
  }
  
  nodeID = atoi(argv[1]);
  if ((nodeID < 0) || (nodeID > 7)) { 
    cerr << "nodeID = " << nodeID 
	 << " is an invalid entry.  Use a value between 0 and 7 instead.\n";
    return -1;
  }

  tVal = atof(argv[2]);

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

  string cmdString;
  stringstream ss;
  ss << "./pc \"m s " << showbase << hex << ID
     << " 2 " << DATA0
     << " " << DATA1 << "\"";
  cmdString = ss.str();

  if (debug) cout << "cmdString = " << cmdString << endl;

  int status = system(cmdString.c_str());
  if (debug) cout << "\"system\" call returned " << status << endl;

  return 0;
}

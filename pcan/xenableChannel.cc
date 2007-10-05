/* File name     : xenableChannel.cc
 * Creation date : 10/05/2007
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: xenableChannel.cc,v 1.1 2007-10-05 13:59:16 jschamba Exp $";
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

// use one of the two following definition for debug printouts:
// const bool debug = true;
const bool debug = false;

//****************************************************************************
int main(int argc, char *argv[])
{
  string cmdString;
  stringstream ss;
  unsigned char data[6];

  bool enableCh[8] = {false, false, false, false, false, false, false, false};
  //char cmdString[255];

  if (debug) cout << vcid << endl;
  cout.flush();

  if ( argc < 4 ) {
    cout << "USAGE: " << argv[0] << " <TDIG node ID> <TCPU node ID> <TDCnum> [<channel1> <channel2> ...]\n";
    return 1;
  }
  
  int tdigNodeID = strtol(argv[1], (char **)NULL, 0);
  if ((tdigNodeID < 0) || (tdigNodeID > 7)) {
    cerr << "tdigNodeID = " << tdigNodeID << " invalid entry. Use 0..7 instead." << endl;
    return -1;
  }

  int tcpuNodeID = strtol(argv[2], (char **)NULL, 0);
  if ((tcpuNodeID < 0) || (tcpuNodeID > 31)) {
    cerr << "tcpuNodeID = " << tcpuNodeID << " invalid entry. Use 0..31 instead." << endl;
    return -1;
  }

  int tdcNum = atoi(argv[3]);
  if ((tdcNum < 1) || (tdcNum > 3)) {
    cerr << "TDC # = " << tdcNum << " invalid entry. Use 1..3 instead." << endl;
    return -1;
  }

  if (argc > 4) {
    for (int i=4; i<argc; i++) {
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


  // build the message

  unsigned int ID = (0x102 | (tdigNodeID << 4))<<18 | tcpuNodeID;  // Write Message

  data[0] = 0x4 + tdcNum;
  data[1] = 0x4 + (enableCh[0] ? 0xe0 : 0);
  data[2] = (enableCh[0] ? 0x1 : 0) + (enableCh[1] ? 0x1e : 0) + (enableCh[2] ? 0xe0 : 0);
  data[3] = (enableCh[2] ? 0x1 : 0) + (enableCh[3] ? 0x1e : 0) + (enableCh[4] ? 0xe0 : 0);
  data[4] = (enableCh[4] ? 0x1 : 0) + (enableCh[5] ? 0x1e : 0) + (enableCh[6] ? 0xe0 : 0);
  data[5] = (enableCh[6] ? 0x1 : 0) + (enableCh[7] ? 0x1e : 0) + 0x80;


  ss << "./pc \"m e 0x" << hex << ID << " 6 ";
  for (int i=0; i<6; i++) ss << " 0x" << (unsigned int)data[i];
  ss << "\"";

  cmdString = ss.str();

  if (debug) cout << "cmdString = " << cmdString << endl;


  // now send the message:
  int status = system(cmdString.c_str());
  if (debug) cout << "\"system\" call returned " << status << endl;

  return 0;
}

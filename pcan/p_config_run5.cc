/* File name     : p_config_run5.cc
 * Creation date : 2/07/05
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: p_config_run5.cc,v 1.1 2005-02-07 23:49:58 jschamba Exp $";
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
const bool debug = true;
//const bool debug = false;

//****************************************************************************
int main(int argc, char *argv[])
{
  ifstream leConf, teConf;
  ofstream leConf00, leConf01, leConf02, teConf03;
  ofstream leConf04, leConf05, leConf06, teConf07;
  ofstream leConf08, leConf09, leConf10, teConf11;
  ofstream leConf12, leConf13, leConf14, teConf15;


  if (debug) cout << vcid << endl;
  cout.flush();

  

  if ( argc < 3 ) {
    cout << "USAGE: " << argv[0] << " <LE config file> <TE config file>\n";
    return 1;
  }
  
  
  leConf.open(argv[1]); // "in" is default 
  if ( !leConf.good() ) {
    cerr << "Could not open file " << argv[1] << ": file error\n";
    return -1;
  }

  teConf.open(argv[2]); // "in" is default 
  if ( !teConf.good() ) {
    cerr << "Could not open file " << argv[2] << ": file error\n";
    return -1;
  }

  leConf00.open("/tmp/td00",  ofstream::out | ofstream::trunc );
  leConf01.open("/tmp/td01",  ofstream::out | ofstream::trunc );
  leConf02.open("/tmp/td02",  ofstream::out | ofstream::trunc );
  leConf04.open("/tmp/td04",  ofstream::out | ofstream::trunc );
  leConf05.open("/tmp/td05",  ofstream::out | ofstream::trunc );
  leConf06.open("/tmp/td06",  ofstream::out | ofstream::trunc );
  leConf08.open("/tmp/td08",  ofstream::out | ofstream::trunc );
  leConf09.open("/tmp/td09",  ofstream::out | ofstream::trunc );
  leConf10.open("/tmp/td10",  ofstream::out | ofstream::trunc );
  leConf12.open("/tmp/td12",  ofstream::out | ofstream::trunc );
  leConf13.open("/tmp/td13",  ofstream::out | ofstream::trunc );
  leConf14.open("/tmp/td14",  ofstream::out | ofstream::trunc );

  teConf03.open("/tmp/td03",  ofstream::out | ofstream::trunc );
  teConf07.open("/tmp/td07",  ofstream::out | ofstream::trunc );
  teConf11.open("/tmp/td11",  ofstream::out | ofstream::trunc );
  teConf15.open("/tmp/td15",  ofstream::out | ofstream::trunc );

  string tempStr;

  // first leading edge:
  leConf >> tempStr;
  leConf00 << tempStr << endl;
  leConf06 << tempStr << endl;
  leConf09 << tempStr << endl;
  leConf10 << tempStr << endl;
  leConf12 << tempStr << endl;
  
  // now change parity:
  if (tempStr[0] == '0') tempStr[0] = '1';
  else tempStr[0] = '0';

  leConf01 << tempStr << endl;
  leConf02 << tempStr << endl;
  leConf04 << tempStr << endl;
  leConf05 << tempStr << endl;
  leConf08 << tempStr << endl;
  leConf13 << tempStr << endl;
  leConf14 << tempStr << endl;
  

  // now the rest of the bits
  for (int i=1; (i<81 && leConf.good()); i++) {
    leConf >> tempStr;
    if (i == 75) {
      tempStr[3]='0';tempStr[4]='0';tempStr[5]='0';tempStr[6]='0'; leConf00 << tempStr << endl;
      tempStr[3]='0';tempStr[4]='0';tempStr[5]='0';tempStr[6]='1'; leConf01 << tempStr << endl;
      tempStr[3]='0';tempStr[4]='0';tempStr[5]='1';tempStr[6]='0'; leConf02 << tempStr << endl;
      tempStr[3]='0';tempStr[4]='1';tempStr[5]='0';tempStr[6]='0'; leConf04 << tempStr << endl;
      tempStr[3]='0';tempStr[4]='1';tempStr[5]='0';tempStr[6]='1'; leConf05 << tempStr << endl;
      tempStr[3]='0';tempStr[4]='1';tempStr[5]='1';tempStr[6]='0'; leConf06 << tempStr << endl;
      tempStr[3]='1';tempStr[4]='0';tempStr[5]='0';tempStr[6]='0'; leConf08 << tempStr << endl;
      tempStr[3]='1';tempStr[4]='0';tempStr[5]='0';tempStr[6]='1'; leConf09 << tempStr << endl;
      tempStr[3]='1';tempStr[4]='0';tempStr[5]='1';tempStr[6]='0'; leConf10 << tempStr << endl;
      tempStr[3]='1';tempStr[4]='1';tempStr[5]='0';tempStr[6]='0'; leConf12 << tempStr << endl;
      tempStr[3]='1';tempStr[4]='1';tempStr[5]='0';tempStr[6]='1'; leConf13 << tempStr << endl;
      tempStr[3]='1';tempStr[4]='1';tempStr[5]='1';tempStr[6]='0'; leConf14 << tempStr << endl;
    }
    else {
      leConf00 << tempStr << endl;
      leConf01 << tempStr << endl;
      leConf02 << tempStr << endl;
      leConf04 << tempStr << endl;
      leConf05 << tempStr << endl;
      leConf06 << tempStr << endl;
      leConf08 << tempStr << endl;
      leConf09 << tempStr << endl;
      leConf10 << tempStr << endl;
      leConf12 << tempStr << endl;
      leConf13 << tempStr << endl;
      leConf14 << tempStr << endl;
    }
  }
  leConf00.close();
  leConf01.close();
  leConf02.close();
  leConf04.close();
  leConf05.close();
  leConf06.close();
  leConf08.close();
  leConf09.close();
  leConf10.close();
  leConf12.close();
  leConf13.close();
  leConf14.close();

  leConf.close();

  // now trailing edge
  teConf >> tempStr;
  teConf03 << tempStr << endl;
  teConf15 << tempStr << endl;

  // now change parity
  if (tempStr[0] == '0') tempStr[0] = '1';
  else tempStr[0] = '0';

  teConf07 << tempStr << endl;
  teConf11 << tempStr << endl;

  // now the rest of the bits
  for (int i=1; (i<81 && leConf.good()); i++) {
    teConf >> tempStr;
    if (i == 75) {
      tempStr[3]='0';tempStr[4]='0';tempStr[5]='1';tempStr[6]='1'; teConf03 << tempStr << endl;
      tempStr[3]='0';tempStr[4]='1';tempStr[5]='1';tempStr[6]='1'; teConf07 << tempStr << endl;
      tempStr[3]='1';tempStr[4]='0';tempStr[5]='1';tempStr[6]='1'; teConf11 << tempStr << endl;
      tempStr[3]='1';tempStr[4]='1';tempStr[5]='1';tempStr[6]='1'; teConf15 << tempStr << endl;
    }
    else {
      teConf03 << tempStr << endl;    
      teConf07 << tempStr << endl;    
      teConf11 << tempStr << endl;    
      teConf15 << tempStr << endl;    
    }
  }
  teConf03.close();
  teConf03.close();
  teConf11.close();
  teConf15.close();

  teConf.close();

  //**************************************

  // now load all these files into the TDCs:

  // ----------- pVPDs -----------------
  system("./p_config 1 0 /tmp/td00 254");
  system("./p_config 1 0 /tmp/td00 253");
  
  system("./p_config 2 0 /tmp/td01 254");
  system("./p_config 2 0 /tmp/td01 253");
  
  system("./p_config 3 0 /tmp/td02 254");
  system("./p_config 3 0 /tmp/td02 253");
  
  system("./p_config 4 0 /tmp/td03 254");
  system("./p_config 4 0 /tmp/td03 253");
  
  // ---------- tray -------------------
  system("./p_config 1 0 /tmp/td00 255");
  system("./p_config 2 0 /tmp/td01 255");
  system("./p_config 3 0 /tmp/td02 255");
  system("./p_config 4 0 /tmp/td03 255");

  system("./p_config 1 1 /tmp/td04 255");
  system("./p_config 2 1 /tmp/td05 255");
  system("./p_config 3 1 /tmp/td06 255");
  system("./p_config 4 1 /tmp/td07 255");

  system("./p_config 1 2 /tmp/td08 255");
  system("./p_config 2 2 /tmp/td09 255");
  system("./p_config 3 2 /tmp/td10 255");
  system("./p_config 4 2 /tmp/td11 255");

  system("./p_config 1 3 /tmp/td12 255");
  system("./p_config 2 3 /tmp/td13 255");
  system("./p_config 3 3 /tmp/td14 255");
  system("./p_config 4 3 /tmp/td15 255");


  system("./p_config 1 4 /tmp/td00 255");
  system("./p_config 2 4 /tmp/td01 255");
  system("./p_config 3 4 /tmp/td02 255");
  system("./p_config 4 4 /tmp/td03 255");

  system("./p_config 1 5 /tmp/td04 255");
  system("./p_config 2 5 /tmp/td05 255");
  system("./p_config 3 5 /tmp/td06 255");
  system("./p_config 4 5 /tmp/td07 255");

  system("./p_config 1 6 /tmp/td08 255");
  system("./p_config 2 6 /tmp/td09 255");
  system("./p_config 3 6 /tmp/td10 255");
  system("./p_config 4 6 /tmp/td11 255");

  system("./p_config 1 7 /tmp/td12 255");
  system("./p_config 2 7 /tmp/td13 255");
  system("./p_config 3 7 /tmp/td14 255");
  system("./p_config 4 7 /tmp/td15 255");


  /************
  string cmdString;
  stringstream ss;
  ss << "./pc \"m s " << showbase << hex << ID
     << " 2 " << DATA0
     << " " << DATA1 << "\"";
  cmdString = ss.str();

  if (debug) cout << "cmdString = " << cmdString << endl;

  int status = system(cmdString.c_str());
  if (debug) cout << "\"system\" call returned " << status << endl;
  ********************/

  return 0;
}

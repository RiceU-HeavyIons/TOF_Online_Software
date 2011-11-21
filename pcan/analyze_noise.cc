/* File name     : analyze_noise.cc
 * Creation date : 11/09/05
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id$";
#endif /* lint */

// #define LOCALDEBUG

#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <math.h>

using namespace std;

const int LEADING=4;
const int TRAILING=5;

int tdcchan2modulechan(int tdcid, int tdcchan,int flag);

int main(int argc, char *argv[])
{

  if(argc<2) {
    cout << "Usage: " << argv[0] << " <filename>" << endl;
    return 0;
  }

  ifstream datafile;
  datafile.open(argv[1]); // "in" is default 
  if ( !datafile.good() ) {
    cerr << argv[1] << ": file open error\n";
    return -1;
  }

  datafile >> hex;
  cout << hex;

  int leadtdigh[8][3][8];    // 8 TDIG, 3 TDCs, 8 channels each
  int trailtdigh[8][3][8];   // 8 TDIG, 3 TDCs, 8 channels each

  int ldmoduleh[4][6];       // 4 modules, 6 channels
  int trmoduleh[4][6];       // 4 modules, 6 channels

  for (int tdig = 0; tdig<8; tdig++) {
    for(int id=0; id<3; id++){
      for(int ich=0; ich<8; ich++){
	leadtdigh[tdig][id][ich] = 0;
	trailtdigh[tdig][id][ich] = 0;
      }
    }

  }
  for(int im=0; im<4; im++){
    for(int ich=0; ich<6; ich++){
      ldmoduleh[im][ich] = 0;
      trmoduleh[im][ich] = 0;
    }
  }



  unsigned int value, dummy;
  int numEv = 0;
  int tdigMult = 0;
  int tdcid, tdcchan, tdig;
  int items = 0;

  while (datafile.good()) {
    datafile >> dummy >> value;
    if (!datafile.good()) break;

#ifdef LOCALDEBUG
    cout << "value = 0x" << value;
#endif

    int edgeid = int( (value & 0xf0000000)>>28 );

    // get rid of seperator words and "bad ID" words ...
    if (edgeid == 0xe) {
#ifdef LOCALDEBUG
      cout << " debug word\n";
#endif
      int count = (value & 0xff00) >> 8;
      if (count != items) 
	cout << "wrong number of data values: found "
	     << items << ", debug word reports "
	     << count << endl;
      items = 0;
      continue;
    }
    else if (edgeid == 0x2) {
#ifdef LOCALDEBUG
      cout << " TDC header\n";
#endif
      items++;
      continue;
    }
    else if(value == 0xc0000001) {
#ifdef LOCALDEBUG
      cout << " geo word 1\n";
#endif
      numEv++;
      tdigMult = 1;
      continue;
    }
    else if(value == 0xc0000000) {
#ifdef LOCALDEBUG
      cout << " geo word 0\n";
#endif
      tdigMult = 0;
      continue;
    }
    else if (edgeid == LEADING) {
      items++;
      tdcid = (value & 0x0f000000)>>24;
      tdig = ((tdcid & 0xc) >> 2) + tdigMult*4;
      tdcid &= 0x3;
      tdcchan = (value&0x00E00000)>>21;
#ifdef LOCALDEBUG
      cout << ", leading edge word" 
	   << ", tdig: " << tdig
	   << ", tdcid: " << tdcid
	   << ", tdcchan: " << tdcchan << endl;
#endif
      leadtdigh[tdig][tdcid][tdcchan]++;
    }
    else if (edgeid == TRAILING) {
      items++;
      tdcid = (value & 0x0f000000)>>24;
      tdig = ((tdcid & 0xc) >> 2) + tdigMult*4;
      tdcid &= 0x3;
      tdcchan = (value&0x00E00000)>>21;
#ifdef LOCALDEBUG
      cout << ", trailing edge word" 
	   << ", tdig: " << tdig
	   << ", tdcid: " << tdcid
	   << ", tdcchan: " << tdcchan << endl;
#endif
      trailtdigh[tdig][tdcid][tdcchan]++;
     }
    else {
#ifdef LOCALDEBUG
      cout << " Bad ID\n";
#endif
      continue;
    }

  }   // end while loop 

  datafile.close();
  cout << "Total number of Events: " << dec << numEv << endl;

  // rates by module and channels
  float totaltime = 0.000025 * numEv;

  for (tdig=0; tdig<8; tdig++) {
    for(int id=0; id<3; id++) {
      for(int ich=0; ich<8; ich++) {
	leadtdigh[tdig][id][ich] = int(leadtdigh[tdig][id][ich]/totaltime);
	trailtdigh[tdig][id][ich] = int(trailtdigh[tdig][id][ich]/totaltime);
      }
    }
    
#ifdef NOTNOW
    for(int im=0; im<4; im++){
      for(int ich=0; ich<6; ich++){
	ldmoduleh[tdig][im][ich] = int(ldmoduleh[tdig][im][ich]/totaltime);
	trmoduleh[tdig][im][ich] = int(trmoduleh[tdig][im][ich]/totaltime);
      }
    }
#endif
  }

  // print out the information
  cout << "===================================================" << endl;
  cout << "Rates(Hz) in " << totaltime << " seconds" << endl;

  float totalrates = 0;
  for(tdig=0; tdig<8; tdig++) {
    int moduleid;
    int modulechan;
    cout << "   TDIG " << tdig << ":" << endl;
    for(int id=0; id<3; id++) {
      cout << "TDC" << setw(1) << id+1 << " l: ";
      for(int ich=0; ich<8; ich++){
	cout << setw(3) << leadtdigh[tdig][id][ich] << " ";
	moduleid = tdcchan2modulechan(id, ich, 0);
	modulechan = tdcchan2modulechan(id, ich, 1);
	ldmoduleh[moduleid][modulechan] = leadtdigh[tdig][id][ich];
      }
      cout << endl;

      cout << "TDC" << setw(1) << id+1 << " t: ";
      for(int ich=0; ich<8; ich++){
	cout << setw(3) << trailtdigh[tdig][id][ich] << " ";
	moduleid = tdcchan2modulechan(id, ich, 0);
	modulechan = tdcchan2modulechan(id, ich, 1);
	trmoduleh[moduleid][modulechan] = trailtdigh[tdig][id][ich];
      }
      cout << endl;
      
    }
    cout << "------------------------------------------------" << endl;

    for(int im=0; im<4; im++) {
      cout << "Module(LD)" << setw(1) << im+1 << " ";
      for(int ich=0; ich<6; ich++) {
	cout << setw(3) << ldmoduleh[im][ich] << " ";
      }
      cout << endl;
    }
    cout << endl;
    for(int im=0; im<4; im++){
      cout << "Module(TR)" << setw(1) << im+1 << " ";
      for(int ich=0; ich<6; ich++){
	cout << setw(3) << trmoduleh[im][ich] << " ";
      }
      cout << endl;
    }
    cout << "===================================================" << endl;
  }
  // cout << "Total rates = " << totalrates << endl;
  cout << "Done analysis. " << endl;

  return 0;

}

//
int tdcchan2modulechan(int tdcid, int tdcchan, int flag)
{

  if(tdcid<0 || tdcid>2) {cout << "Error! Not a valid tdc id!" << endl; return -1;}
  if(tdcchan < 0 || tdcchan > 7) {
    cout<<"Error! Not a valid tdc channel!"<<endl;
    return -1;
  }

  int moduleid[24]  = {0,2,1,2,1,0,1,0,2,3,0,1,3,3,0,0,3,3,2,2,1,3,2,1};
  int modulechan[24]= {2,4,3,2,1,4,5,0,0,5,3,4,3,1,5,1,0,2,1,3,2,4,5,0};

  int index = tdcid*8 + tdcchan;

  int retmodule = moduleid[index];
  int retmodulechan = modulechan[index];

  if(flag == 0) return retmodule;
  if(flag == 1) return retmodulechan;
  return -1;
}

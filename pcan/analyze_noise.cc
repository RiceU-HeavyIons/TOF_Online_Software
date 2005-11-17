/* File name     : analyze_noise.cc
 * Creation date : 11/09/05
 * Author        : J. Schambach, UT Physics
 * Modified date : 
 *               : 
 */

#ifndef lint
static char  __attribute__ ((unused)) vcid[] = 
"$Id: analyze_noise.cc,v 1.3 2005-11-17 20:31:41 jschamba Exp $";
#endif /* lint */


#include <iostream>
#include <string>
#include <iomanip>
#include <stdio.h>
#include <math.h>

using namespace std;

const int LEADING=4;
const int TRAILING=5;

int tdcchan2modulechan(int tdcid, int tdcchan,int flag);

int main(int argc, char *argv[])
{

  if(argc<5) {
    cout << "Usage: " << argv[0] << " -f filename -t time " << endl;
    return 0;
  }

  string noisefilename;
  float totaltime;
  if (argc>1){
    for (int i=1;i<(argc);i++){
      if (!strcmp(argv[i],"-h") || !strcmp(argv[i],"--help")){
        cout<<"Usage: ----------->"<<endl;
        cout<<argv[0]<<" -f filename"<<endl;
        cout<<argv[0]<<" -t time (in seconds)"<<endl;
        return 0;
      }

      if (!strcmp(argv[i], "-f"))   {noisefilename = argv[++i];}
      if (!strcmp(argv[i], "-t"))   {totaltime  = atoi(argv[++i]);}
 
    }
  }

  float triggerfreq = 40000000/pow(2.,15);
  float occupancy = 0.000025 * triggerfreq;
  totaltime =  totaltime * occupancy;
  cout << " trigger freq, occupancy totaltime = " << triggerfreq << " " << 
    occupancy << " " << totaltime << endl;

  int leadtdigh[2][3][8];    // 2 TDIG, 3 TDCs, 8 channel each
  int trailtdigh[2][24];     // 2 TDIG, 24 channels

  int ldmoduleh[2][4][6];       // 2 TDIG, 4 modules, 6 channels
  int trmoduleh[2][4][6];       // 2 TDIG, 4 modules, 6 channels
  for (int tdig = 0; tdig<2; tdig++) {
    for(int id=0; id<3; id++){
      for(int ich=0; ich<8; ich++){
	leadtdigh[tdig][id][ich] = 0;
	int ch = id*8 + ich;
	trailtdigh[tdig][ch] = 0;
      }
    }

    for(int im=0; im<4; im++){
      for(int ich=0; ich<6; ich++){
	ldmoduleh[tdig][im][ich] = 0;
	trmoduleh[tdig][im][ich] = 0;
      }
    }
  }


  // open data file    
  FILE* inputfile = fopen(noisefilename.c_str(), "r");

  if(!inputfile) {
     cout << "Error! Can not open " << noisefilename << endl;
     return -1;
  }
  cout << " Open " << noisefilename << endl;

  unsigned int value;
  int numret=0;
  int goodevent=0;
  int badevent=0;
  int tdcid, tdcchan, tdig;

  while ((numret = fscanf(inputfile, "%x", &value)) == 1) {
    int edgeid =int( (value & 0xf0000000)>>28 );

    // get rid of seperator words and "bad ID" words ...
    if (edgeid == 0xe) continue;
    if((edgeid != LEADING) && (edgeid != TRAILING)) {badevent++; continue;} 

    goodevent++;

    cout << "value:0x" << hex << value << dec << " ";
    tdcid = (value & 0x0f000000)>>24;
    tdcid = tdcid % 8;
    cout << " tdcid1:" << tdcid << " ";
    tdig = tdcid/4;
    tdcid = tdcid % 4;

    if(edgeid == LEADING) {    // very high resolution, TDC 1-3
      tdcchan = (value&0x00E00000)>>21;
    } 
    else if(edgeid == TRAILING){
      tdcchan = (value&0x0F80000)>>19;
    } 
    else {
      cout << "Unknown data type " << endl;
      continue;
    }
    cout << "edge:" << edgeid << " tdig:" << tdig << " tdcid:" << tdcid << " tdcchan:" << tdcchan << endl;

    int moduleid = tdcchan2modulechan(tdcid, tdcchan, 0);
    int modulechan = tdcchan2modulechan(tdcid, tdcchan, 1);

    if(edgeid == LEADING){
       leadtdigh[tdig][tdcid][tdcchan]++;
       ldmoduleh[tdig][moduleid][modulechan]++;
    }   
    else if(edgeid == TRAILING){
       int trailchan = tdcchan;
       trailtdigh[tdig][trailchan]++;    
       trmoduleh[tdig][moduleid][modulechan]++;
    }  
  }   // end while loop 

  fclose(inputfile);

  cout << "Total words: " << goodevent << endl;

  // rates by module and channels
  float rates=0;

  for (tdig=0; tdig<2; tdig++) {
    for(int id=0; id<3; id++) {
      for(int ich=0; ich<8; ich++) {
	leadtdigh[tdig][id][ich] = int(leadtdigh[tdig][id][ich]/totaltime);
	int ch = id*8 + ich;
	trailtdigh[tdig][ch] = int(trailtdigh[tdig][ch]/totaltime);
      }
    }
    
    for(int im=0; im<4; im++){
      for(int ich=0; ich<6; ich++){
	ldmoduleh[tdig][im][ich] = int(ldmoduleh[tdig][im][ich]/totaltime);
	trmoduleh[tdig][im][ich] = int(trmoduleh[tdig][im][ich]/totaltime);
      }
    }
  }

  // print out the information
  cout << "===================================================" << endl;
  cout << "Rates(Hz) in " << totaltime << " seconds" << endl;

  float totalrates = 0;
  for(tdig=0; tdig<2; tdig++) {
    cout << "   TDIG " << tdig << ":" << endl;
    for(int id=0; id<3; id++) {
      cout << "TDC" << setw(1) << id+1 << ": ";
      for(int ich=0; ich<8; ich++){
	cout << setw(3) << leadtdigh[tdig][id][ich] << " ";
      }
      cout << endl;
      
      cout << "TDC" << setw(1) << 4 << ": ";
      for(int ich=0; ich<8; ich++){
	int ch = id*8 + ich;
	cout << setw(3) << trailtdigh[tdig][ch] << " ";
      }
      cout << endl;
    }
    cout << "------------------------------------------------" << endl;
    for(int im=0; im<4; im++) {
      cout << "Module(LD)" << setw(1) << im+1 << " ";
      for(int ich=0; ich<6; ich++) {
	cout << setw(3) << ldmoduleh[tdig][im][ich] << " ";
	totalrates = totalrates + ldmoduleh[tdig][im][ich];
      }
      cout << endl;
    }
    cout << endl;
    for(int im=0; im<4; im++){
      cout << "Module(TR)" << setw(1) << im+1 << " ";
      for(int ich=0; ich<6; ich++){
	cout << setw(3) << trmoduleh[tdig][im][ich] << " ";
      }
      cout << endl;
    }
    cout << "===================================================" << endl;
  }
  cout << "Total rates = " << totalrates << endl;
  cout << "Done analysis. " << endl;
  return 0;

}

//
int tdcchan2modulechan(int tdcid, int tdcchan, int flag)
{

  if(tdcid<0 || tdcid>3) {cout << "Error! Not a valid tdc id!" << endl; return -1;}
  if((tdcid >= 0 && tdcid <= 2) && (tdcchan < 0 || tdcchan > 7)) {
    cout<<"Error! Not a valid tdc channel!"<<endl;
    return -1;
  }
  if(tdcid == 3 && (tdcchan<0 || tdcchan>23)){
    cout<<"Error! Not a valid tdc channel!"<<endl;
    return -1;
  }

  int moduleid[24]  = {0,0,0,0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,3,3,3,3,3,3};
  int modulechan[24]= {0,1,2,3,4,5,0,1,2,3,4,5,0,1,2,3,4,5,0,1,2,3,4,5};
  int hptdc[24]     = {1,0,1,1,1,0,0,0,0,0,0,0,1,1,2,1,2,1,2,2,2,2,2,2};
  int datachan[24]  = {0,7,2,1,3,6,2,1,4,3,5,0,6,5,0,7,1,4,4,3,6,5,7,2};

  int trailchan[24]= {8,7,10,9,11,6,2,1,4,3,5,0,14,13,16,15,17,12,20,19,22,21,23,18};

  //get index according to tdcid and tdcchan
  int index = 0;
  if(tdcid < 3){
    for(int i=0; i<24; i++){
      if((tdcid == hptdc[i]) && (tdcchan == datachan[i])) {
        index = i;
        break;
      }
    }
  }

  if(tdcid == 3){
    for(int i=0; i<24; i++){
      if(tdcchan == trailchan[i]) {
        index = i;
        break;
      }
    }
  }
  int retmodule = moduleid[index];
  int retmodulechan = modulechan[index];

  if(flag == 0) return retmodule;
  if(flag == 1) return retmodulechan;
  return -1;
}

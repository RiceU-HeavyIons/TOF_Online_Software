#include <cstdlib>
#include <iostream>
#include <iomanip>
//#include <fstream>
#include <vector>


#include <TH1.h>

#include "TsPacket.h"


bool DEBUG=false;
bool BYHAND=false;

const static int nChannels=24;
TH1F* echo_leHisto=new TH1F("le_all","leading edges with echoes",nChannels,0,nChannels);
TH1F* echo_teHisto=new TH1F("te_all","trailing edges with echoes",nChannels,0,nChannels);


int x_map[] = {
  2, 22, 13, 19, 16, 11,
  8, 5, 15, 12, 1, 7, 21,
  18, 10, 4, 20, 23, 14, 
  6, 0, 17, 9, 3 
};

struct hit {
  int modId;
  int trayId;
  vector<float> LE;
  vector<float> TE;
  vector<float> tot;
  vector<float> Z;

  hit(int mId, int tId, vector<float> le, vector<float> te, vector<float> timeoverthresh, vector<float> z)
  { modId=mId;  trayId=tId; LE=le; TE=te; tot=timeoverthresh; Z=z; };

};



//function to take the head and tail timings and make a timing difference
//currently only takes 1st of both, but in principle can be optimized
std::vector<float> pairTimings(vector<float> a,vector<float> b)
{
  vector<float> pairs;
  pairs.clear();


  //for now just take 0,0 -- this can use some tuning later
  if(a.size()==1 && b.size()==1) {
    if(b[0]-a[0]<3000) {
      if(b[0]-a[0]>0) {
	pairs.push_back(b[0]-a[0]);
      }}}
  return pairs; 
};


//functions to determine Z-position 
// (1)  z1 = alpha*(t1) + t0
// (2)  z2 = L - z1 = alpha*(t2) + t0

// (1) + (2) =>
//  L = alpha*(t1+t2) + 2*t0
//  (1) - (2) =>
//  2*z1 - L = alpha*(t1-t2)

// assume t0=0;
// then alpha = L/(t1+t2)
// or solving for alpha
//  kappa == deltaT/sum(T) = 1 - 2*(z/L)
//  i.e.  z = 0.5*(1-kappa);

std::vector<float> getZs(std::vector<float> LEs, int channelId){
  vector<float> outZs;
  outZs.clear();

  float alpha=0;
  for(int aa=0;aa<LEs.size();aa++) {
    float tempZ=LEs[aa];
    outZs.push_back(tempZ);
  }
  return outZs;
};


int eventN=0; //set some counters
int badEventN=0; //set some counters

void readEvent(unsigned int n, vector<hit>& theHits, ifstream& ourIO)
{
  theHits.clear();

  vector<float> LEs[nChannels];
  vector<float> TEs[nChannels];
  vector<float> Zs[nChannels];
  vector<float> tots;

  int trayId=1;

  int tray_half=0;
  bool corrupt=false;
  int thisEvent=eventN;


  // the first word read on entry should be geographical word 1
  if(n == 0xc0000001 ) { //start event read of TDIG 4-7
    tray_half=2;
  }
  else {
    cout << "Didn't see StartOfEvent (0xc0000001)" << endl;
  }

  // loop through all data words until 0xe0XXXXXX
  while(eventN == thisEvent) {
    ourIO >> n; // read next packet

    //TsPacket *p = new TsPacket(n);

    if(n == 0xc0000001 ) { //start event read of TDIG 4-7
      tray_half=2;
    }

    else if( (n & 0xF0000000) == 0x20000000  ) { // TDC header
    }

    else if( (n & 0xFF000000) == 0xe1000000  ) { // end event read of TDIG 4-7
    }

    else if(n == 0xc0000000 ) { //start second half of event read of TDIG 0-3
      tray_half=1;
    }

    else if( (n & 0xFF000000) == 0xe0000000  ) { //end event read of TDIG 0-3

      //********************************************************************************************//
      //********************************************************************************************//
      //********************************************************************************************//
      //now event is completely done and we can do analysis
      
      //make sure the event was OK
      if(corrupt == false) { // currently not set anywhere
	//initialize
	
	//now loop over all modules and look at 
	//stored hits from event
	for(int aa=0;aa<nChannels;aa++) {
	  
	  //match LE's and TE's for a module
	  tots=pairTimings(LEs[aa],TEs[aa]);
	  //Zs=getZs(LEs[aa],aa);
	  //loop over matches 
	  
	  //we are essentially just taking the old format and converting it into "theHits"
	  theHits.push_back(   hit(aa, trayId,  LEs[aa], TEs[aa], tots, Zs[aa])   );
	  
	  
	  //clear for next loop
	  tots.clear();				
	  LEs[aa].clear();
	  TEs[aa].clear();
	  Zs[aa].clear();
	}		
	
	eventN++;
	//end analysis
	//********************************************************************************************//
	//********************************************************************************************//
	//********************************************************************************************//
	
      }
      else { cout<<"event malfunction!"<<endl;badEventN++; }
      corrupt=false;
      
      ///////////////////////////////////////////////
      ///////////////////////////////////////////////
      //	return;// actual end of function
      ///////////////////////////////////////////////
      ///////////////////////////////////////////////
      
    }

    else if ( ((n & 0xF0000000) == 0x40000000) ||
	      ((n & 0xF0000000) == 0x50000000) ) { // Leading or Trailing Edge

      // Submarine MTD Data is only in TDIG 0
      if ((tray_half == 1) && ((n & 0x0C000000) == 0)  ) {
	int tdcChannel   = (n & 0x00E00000) >> 21;
	int tdc = (n & 0x03000000) >> 24;
	int channel = tdc*8 + tdcChannel;
	int mtdChannel = x_map[channel];
	if(DEBUG) cout<<"le @ channel: " << mtdChannel << endl;

	unsigned int edgeTime = ((n & 0x00180000) >> 19) + ((n & 0x0007FFFF) << 2);
	
	if ((n & 0xF0000000) == 0x40000000) { // Leading edge
	  LEs[mtdChannel].push_back(edgeTime);
	  echo_leHisto->Fill(mtdChannel);
	}
	else { // Trailing edge
	  TEs[mtdChannel].push_back(edgeTime);
	  echo_teHisto->Fill(mtdChannel);
	}
      }
    }

    else {
      // Don't do anything with all other packets:
      cout << "*** unexpected packet 0x" << hex << n << endl;
    }


/*     else if(tray_half == 1 ) {  */
/*       /////// MTD Data is in  tray half 1 */
/*       int outChannel=p->GetIdChannel();// */
/*       int outTDig=p->tdc;//this function returns TDC channel */
/*       int tempChannel=8*(outTDig-0)+outChannel; */
/*       int abChannel=x_map[tempChannel]; */


/*       if( p->IsLE() ) {  */
	
/* 	if(DEBUG) cout<<"le @ channel: "<< abChannel */
/* 		      << " (" << tempChannel<< ")" << endl; */
/* 	unsigned int et=p->GetEdgeTime(); */
/* 	LEs[abChannel].push_back(et); */
/* 	echo_leHisto->Fill(abChannel); */

/*       } */

/*       if(p->IsTE() ) { */
/* 	cout << "*** tempChannel " << tempChannel  */
/* 	     << " tdc_map " << (int)(p->GetChannel()) << endl; */
/* 	unsigned int et=p->GetEdgeTime(); */
/* 	TEs[abChannel].push_back(et); */
/* 	echo_teHisto->Fill(abChannel); */
/* 	if(DEBUG) cout<<"te @ channel: "<<abChannel<<endl; */
/*       } */
/*     } */

    //delete p;

  } // while...

  return;
}


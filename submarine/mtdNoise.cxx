#include <cstdlib>
#include <iostream>
#include <iomanip>
//#include <fstream>
#include <vector>
#include <sstream>

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TNtuple.h>

#include "readMTD.h"
#include "regMany.h"


using namespace std;

#define DEBUG 0
#define BYHAND 0

const static int nStacks=1;
const static int nModules=1;

// globally read/filled histograms
TH1F* rawNoiseRate = new TH1F("noise","Raw Noise Rate",nChannels,0,nChannels);
TH1F* leHisto = new TH1F("le","leading edges",nChannels,0,nChannels);
TH1F* teHisto = new TH1F("te","trailing edges",nChannels,0,nChannels);
TH1F* totHisto = new TH1F("tots","tots",300,0,3000);
TNtuple* summary = new TNtuple("summary","summary","nrate:ch");
TH1F* totHistos[nChannels];
TH1F* zHistos[nChannels/2+1];
regressor* correlation = new regressor();



//  putting bulk of analysis code in this routine
void doAnalysis(vector<hit>& theHits, TNtuple* totTuple) {

  // init some variables, these are to calculate the correlations between the channels
  float x[nChannels];
  float x2[nStacks][24];
  float x3[nStacks][4][nModules];
  for(int aa=0;aa<nChannels;aa++){ x[aa]=0;}

  // fill above variables with "theHits" from "theEvent"
  for(int aa=0;aa<nChannels;aa++) {
    for(int bb=0;bb<theHits[aa].tot.size();bb++) {	
      x[aa]++;//number of hits in module	
    }

    for(int bb=0;bb<theHits[aa].tot.size();bb++) {	
      totHisto->Fill(theHits[aa].tot[bb]);	
      totHistos[aa]->Fill(theHits[aa].tot[bb]);	
    }
  }
  correlation->input(x,x);


  // the constants needed to calculate the Z position of the hit pair
  const double L     = 0.87;//meters
  const double alpha = 0.65;// 65% of the speed of light
  const double delta = 0.0075;// c/25pico seconds  -- this is the distance per bin
  const double dZ    = alpha*delta; // z distance traveled per tick
  const double invDZ = 1./dZ;

  // look at correlations in tot timings
  for(int aa=0;aa<nChannels/2;aa++) {
    if(theHits[aa].tot.size() && theHits[aa+12].tot.size()) {
      float tot1=theHits[aa].tot[0];
      float tot2=theHits[aa+12].tot[0];
      float deltaTot= tot1 - tot2;
      float LE1=theHits[aa].LE[0];
      float LE2=theHits[aa+12].LE[0];
      float TE1=theHits[aa].TE[0];
      float TE2=theHits[aa+12].TE[0];
      float deltaLE= LE1 - LE2; 
      float deltaTE= TE1 - TE2;

      double Z         = 0.5*( L + dZ*deltaLE );// z position of the hit
      double sumT      = LE1+LE2;
      double t0        = 0.5*invDZ*( dZ*sumT - L); // this is the offset time
      double trueSum1  = sumT - 2.*t0;

      zHistos[aa]->Fill(Z);
    }
  }


  // these are being handled in readMTD.h -- should be moved over here
  /////////////////////////////////////////////////////////////////////////////
  //  loop over all LEs
  //	if(p->IsLE()		echo_leHisto->Fill(abChannel);
  /////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////
  //   loop over all TEs
  //	if(p->IsTE() ){		echo_teHisto->Fill(abChannel);
  /////////////////////////////////////////////////////////////////////////////
  return;
}


///////////////////// main  /////////////////////////////////////
/////////////////// program /////////////////////////////////////

int main(int argc, char* argv[])
{
  if(argc<2) {
    cout << "Usage: " << argv[0] << " <filename> [<TDIG #> <maxEvents> <maxLines>] " << endl;
    return 0;
  }
    
  //input file
  string inName=argv[1];
  ifstream io(inName.c_str());
  if(!io) { cerr << "no input file!!" << endl; exit(1); }
  
  // default TDIG
  int tdig = 0;
  if(argc > 2) {tdig=atoi(argv[2]);}

  // max number of events
  int maxEventN=10000000-1;
  if(argc > 3) {maxEventN=atoi(argv[3]);}

  // max number of events
  int maxLineN=-1;
  if(argc == 5) {maxLineN=atoi(argv[4]);}

  //set path names for output files names

//   string outname="rootfiles/";
//   outname+=inName.substr(inName.find_last_of("/")+1);
//   outname+=".root";

  string outname = inName.substr(0,inName.find_last_of("/")+1);
  outname += "noisedata";
  //ostringstream temp;
  //temp << tdig;
  //outname += temp.str();
  outname += static_cast<ostringstream*>( &(ostringstream() << tdig) )->str();
  outname +=".root";

  cout << "saving ROOT file as:\n./" << outname << endl;
  TFile outFile(outname.c_str(),"recreate");
  cout << endl;

  TNtuple* totTuple=new TNtuple("totTuple","totTuple","aa:z");

  // initialize the time-over-threshold histograms (one for each MRPC)
  char hName[100];
  for(int aa=0; aa<nChannels; aa++) {
    sprintf(hName,"tot_%i ",aa);
    totHistos[aa] = new TH1F(hName,hName,300,0,3000);
  }

  for(int aa=0; aa<nChannels/2; aa++) {
    sprintf(hName,"z_%i ",aa);
    zHistos[aa] = new TH1F(hName,hName,100,-0.5,1.5);
  }

  TH2F* corrPlot = new TH2F("corrPlot","corrPlot",nChannels,0,nChannels,nChannels,0,nChannels);
  TH2F* corrPlot2[nStacks];
  for(int aa=0; aa<nStacks; aa++){
    sprintf(hName,"corrSmall_%d",aa);
    corrPlot2[aa] = new TH2F(hName,hName,24,0,23,24,0,23);
  }

  regressor correlationSmall[nStacks];
  float x2[nStacks][24];
  float x3[nStacks][4][nModules];
  for(int aa=0;aa<nStacks;aa++){for(int bb=0;bb<24;bb++){x2[aa][bb]=0;}}
  for(int aa=0;aa<nStacks;aa++){for(int bb=0;bb<4;bb++){for(int cc=0;cc<nModules;cc++){x3[aa][bb][cc]=0;}}}

  for(int aa=0;aa<nStacks;aa++){ correlationSmall[aa].init(24,24); }

  const int Xbins=nChannels;
  const int Ybins=nChannels;
  float x[Xbins];
  float x_m[Xbins];//correlation holder for the mapping used below

  correlation->init((int)Xbins,(int)Ybins);

  vector<hit> theHits;

  //now begin looking at data
  unsigned int dWord;
  int line=0;
  io >> hex;
  while (io >> dWord) {
    if(eventN > maxEventN){ if(line=0){cout<<"\n\nEnding early!\n\n"<<endl;} break; }
    if((maxLineN > 0) && (line > maxLineN) ){ break; }

    readEvent(dWord, theHits, io, tdig);
    doAnalysis(theHits, totTuple);
    line++;
  }

  cout<<"ending loop"<<endl;

  cout<<dec;

  float eventtime=25e-6;
  rawNoiseRate->Add(echo_leHisto);
  rawNoiseRate->Scale(1.0/(float)eventN/eventtime);

  for(int aa=0;aa<nChannels;aa++){
    float nR=rawNoiseRate->GetBinContent(aa);
    summary->Fill(nR,aa);
  }

  summary->Write();
  totTuple->Write();
  rawNoiseRate->Write();
  echo_leHisto->Write();
  echo_teHisto->Write();
  totHisto->Write();
  for(int aa=0;aa<nChannels;aa++){totHistos[aa]->Write();}
  for(int aa=0;aa<nChannels/2;aa++){zHistos[aa]->Write();}


  correlation->calc();
  float corrOut[nChannels*nChannels];


  //cout<<"The correlation matrix: "<<endl;
  correlation->get_cor(corrOut);
  for(int aa=0;aa<nChannels;aa++) {
    for(int bb=0;bb<nChannels;bb++) {
      if(aa==bb) { corrPlot->Fill(aa,bb, 0);} //may be backwards
      else { 
	float fillVal=corrOut[aa*nChannels+bb];
	if(fillVal==1) fillVal=.99;
	corrPlot->Fill(aa,bb,fillVal);
	//cout.width(5);
	//cout<<setw(5)<<setprecision(0)<< corrOut[aa*nChannels+bb]<<"  ";
      }
    }
    //cout<<endl;
  }	
  corrPlot->Write();
  cout<<endl;
  cout<<"Noise Rates:"<<endl;


  FILE *outTextFile;
//   string text_str="mtdNoiseTexts/";
//   text_str+=inName.substr(inName.find_last_of("/")+1);
//   text_str+=".txt";
  string text_str = inName.substr(0,inName.find_last_of("/")+1);
  text_str += "results";
  text_str += static_cast<ostringstream*>( &(ostringstream() << tdig) )->str();
  text_str += ".txt";
  cout<<"Printing out noise to file:\n"<<text_str<<endl;
  outTextFile=fopen(text_str.c_str(),"w");


  for(int aa=1;aa<nChannels;aa++) {
    float rate=rawNoiseRate->GetBinContent(aa);
    printf("stack#\t%i\t%f\n",aa,rate);
    fprintf(outTextFile, "channel#\t%i\t%f\n",aa,rate);
  }
  cout<<endl;

  fclose(outTextFile);
  outFile.Close();

  cout<<"total number of good events: "<<eventN<<endl;
  cout<<"total number of bad events: "<<badEventN<<endl;
  cout<<endl;
  cout<<endl;

  return 0;
}


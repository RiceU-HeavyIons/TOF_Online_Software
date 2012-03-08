#include <cstdlib>
#include <iostream>
#include <iomanip>
//#include <fstream>
#include <vector>

#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TNtuple.h>

#include "TsPacket.h"
#include "regMany.h"


using namespace std;

#define DEBUG 0


//function to take the head and tail timings and make a timing difference
//currently only takes 1st of both, but in principle can be optimized
std::vector<float> pairTimings(vector<float> a,vector<float> b, int channel)
{
	vector<float> pairs;
	pairs.clear();
	//for now just take 0,0 -- this can use some tuning later
	if(a.size()>0 && b.size()>0) {
		if(b[0]-a[0]<3000) {
			pairs.push_back(b[0]-a[0]);
		}}
	return pairs; 
}

//recursive readout -- there should be an equivalent cout type function...
void binary(unsigned int number) {
	if(number <= 1) { cout << number; return; }
	int remainder = number%2;
	binary(number >> 1);    
	cout << remainder;
	return;
}

//function getMrpcPath

///////////////////// main  /////////////////////////////////////
/////////////////// program /////////////////////////////////////

int main(int acgc, char* argv[])
{

	//input file
	string inName=argv[1];
	ifstream io(inName.c_str());
	if(!io){cerr<<"no input file!!"<<endl; exit(1);}
	//max number of events
	long unsigned int maxEventN=1000000-1;
	if(argv[2]){maxEventN=atoi(argv[2]);}
	//max number of events
	int maxLineN=-1;
	if(argv[3]){maxLineN=atoi(argv[3]);}


	//find text file containing mrpc serial numbers
	int setPosition=inName.find("set");
	string buffer=inName.substr(setPosition+3, 3);
	string mrpcPath="/home/dylan/projects/submarine/mrpcTexts/set";
	mrpcPath+=buffer;
	mrpcPath+=".mrpcs";
	cout<<"pulling module id's from file: "<<mrpcPath<<endl;
	ifstream getNames(mrpcPath.c_str());

	//read in said mrpc serial numbers
	string mrpcNames[32];
	int nInput=0;
	char buffer2[100];
	if(!getNames.is_open()){
		cerr<<"no mrpc list!!"<<endl;
		for(int aa=0;aa<32;aa++){ sprintf(buffer2, "%i",aa); mrpcNames[aa]=buffer2;}

	}
	while(getNames.good() && nInput<32)
	{  
		getNames.getline(buffer2,100);
		mrpcNames[nInput]=buffer2;
		nInput++;
	}



	//define analysis variables and plots
	int tray_half, eventN, badEventN, eventStep;//set some counters
	tray_half = eventN = badEventN = eventStep = 0;
	int nChannels=96*2;
	vector<float> LEs[nChannels];
	vector<float> TEs[nChannels];
	vector<float> tots;

	//set up ROOT specific analysis
	string outname="/home/tof/submarine/rootfiles/";
	setPosition=inName.find("/");
	outname+=inName.substr(setPosition+1, inName.length());
	outname+=".root";
	cout<<"saving ROOT file as: "<<outname<<endl;
	TFile outFile(outname.c_str(),"recreate");

	TH1F* echo_leHisto=new TH1F("le_all","leading edges with echoes",nChannels,0,nChannels);
	TH1F* echo_teHisto=new TH1F("te_all","trailing edges with echoes",nChannels,0,nChannels);
	TH1F* rawNoiseRate=new TH1F("noise","Raw Noise Rate",nChannels,0,nChannels);
	TH1F* leHisto=new TH1F("le","leading edges",nChannels,0,nChannels);
	TH1F* teHisto=new TH1F("te","trailing edges",nChannels,0,nChannels);
	TH1F* totHisto=new TH1F("tots","tots",300,0,3000);
	TNtuple* summary=new TNtuple("summary","summary","nrate:ch");

	TNtuple* totInfo=new TNtuple("totInfo","totInfo","tot:absCh:ch:mod");
	char hName[100];
/*
	TH1F* totHistos[nChannels];
	for(int aa=0;aa<nChannels;aa++){
		//sprintf(hName,"tot_Mod_%i_Channel_%i",aa/6,aa%6);
		sprintf(hName,"tot_ch%i",aa);
		totHistos[aa]=new TH1F(hName,hName,300,0,3000);
	}
*/

	TH2F* corrPlot=new TH2F("corrPlot","corrPlot",nChannels,0,nChannels-1,nChannels,0,nChannels-1);
	const int nStacks=8;
	TH2F* corrPlot2[nStacks];
	for(int aa=0;aa<nStacks;aa++){
		sprintf(hName,"corrSmall_%d",aa);
		corrPlot2[aa]=new TH2F(hName,hName,24,0,23,24,0,23);
	}

	//set up correlation code
	regressor correlationSmall[nStacks];
	float x2[nStacks][24];
	float x3[nStacks][4][6];
	for(int aa=0;aa<nStacks;aa++){for(int bb=0;bb<24;bb++){x2[aa][bb]=0;}}
	for(int aa=0;aa<nStacks;aa++){for(int bb=0;bb<4;bb++){for(int cc=0;cc<6;cc++){x3[aa][bb][cc]=0;}}}

	for(int aa=0;aa<nStacks;aa++){
		//correlationSmall[aa]=new regressor();
		correlationSmall[aa].init(24,24);
	}

	regressor* correlation=new regressor();
	const int Xbins=nChannels;
	const int Ybins=nChannels;
	float x[Xbins];
	correlation->init((int)Xbins,(int)Ybins);


	//now begin looking at data
	unsigned int n,m;
	int line=0;
	bool corrupt=false;
	io >> hex;
	while (io >> n){



		if(eventN > maxEventN){ if(line=0){cout<<"\n\nEnding early!\n\n"<<endl;} continue; }
		if((maxLineN > 0) && (line > maxLineN) ){ continue; }

		line++;
		//		io >> n;
		TsPacket *p=new TsPacket(n);

		if(DEBUG){
			cout<<hex<<n<<endl;
			binary(n);
			cout<<dec<<" ..."<<endl;
			p->Print();
			cout<<endl;
			//int fID=p->GetID();	
		}


		//cout<<eventStep<<endl;

		//
		if(n==0xc0000001 ){
			tray_half=2;
			eventStep=1;}//start event read of TDIG 4-7
			if( (n & 0xF0000000) >> 28 == 0x2  )//now should see a header
			{	
				if( DEBUG && (eventStep!=1 && eventStep!=4) ){corrupt=true; cout<<"doh @1/4 -- step: "<<eventStep<<"\tline: "<<line<<endl;}
				eventStep++;
			}

			if( (n & 0xFF000000) >> 24 == 0xe1  )//end event read of TDIG 4-7
			{
				if(DEBUG && eventStep!=2){corrupt=true; cout<<"doh @2 -- step: "<<eventStep<<"\tline: "<<line<<endl;}
				eventStep=3; 
			}
			if(n==0xc0000000 )//startsecond half of event read of TDIG 0-3
			{
				tray_half=1;
				if(DEBUG && eventStep!=3){corrupt=true; cout<<"doh @3 -- step: "<<eventStep<<"\tline: "<<line<<endl;}
				eventStep=4;

			}
			if( (n & 0xFF000000) >> 24 == 0xe0  )//end event read of TDIG 0-3
			{

				if(DEBUG && eventStep!=5){corrupt=true; cout<<"doh @5 -- step: "<<eventStep<<"\tline: "<<line<<endl;}
				eventStep=6; 

				//now event if completely done and we can do analysis



				if(corrupt==false){
					for(int aa=0;aa<nChannels;aa++){ x[aa]=0;}// x2[aa/8][aa%24]=0;   }
				for(int aa=0;aa<8;aa++){for(int bb=0;bb<24;bb++){x2[aa][bb]=0;}}


				for(int aa=0;aa<nChannels;aa++){

					tots=pairTimings(LEs[aa],TEs[aa],aa);//match leading and trailing edges
					for(int bb=0;bb<tots.size();bb++){	
						//MRPC aa, tot bb (bb should always=0, we are only looking at first hit)
						totHisto->Fill(tots[bb]);	
						totInfo->Fill(tots[bb],aa,aa%6,(int)aa/6);	

						x[aa]++;	
						x2[(int)aa/24][(int)aa%24]++;			

					}



					tots.clear();				
					LEs[aa].clear();
					TEs[aa].clear();
				}		


				//if(DEBUG)cout<<"end event!"<<endl<<endl;


				correlation->input(x,x);
				for(int aa=0;aa<nStacks;aa++){ correlationSmall[aa].input(x2[aa],x2[aa]);}


				eventN++;

			}
			if(corrupt==true){badEventN++;}

			corrupt=false;


	}

	if(tray_half){


		int fChannel = p->GetChannelTray4();
		int abChannel=fChannel+(tray_half-1)*96;
		if( p->IsLE() ){ 
			unsigned int et=p->GetEdgeTime();
			//	cout<<"edge time: "<<et<<endl;
			//	cout<<"fChannel: "<<fChannel<<endl;
			//	cout<<"tray_half: "<<tray_half<<endl;
			//	cout<<"abChannel: "<<abChannel<<endl;
			LEs[abChannel].push_back(et);
			echo_leHisto->Fill(abChannel);



			if(DEBUG && 0)cout<<"le @ channel: "<<fChannel<<"\ttray_half: "<<tray_half<<endl;

		}

		if(p->IsTE() ){
			unsigned int et=p->GetEdgeTime();
			TEs[abChannel].push_back(et);
			echo_teHisto->Fill(abChannel);
			if(DEBUG && 0)cout<<"te @ channel: "<<abChannel<<endl;
		}


	}

	delete p;


}

float eventtime=25e-6;// timing window = 25 micro seconds (25 us)
rawNoiseRate->Add(echo_leHisto);

//for(int aa=0;aa<rawNoiseRate->GetMaximumBin(); aa++) { cout<<"aa: "<<aa<<"\t"<<rawNoiseRate->GetBinContent(aa)<<endl; }

rawNoiseRate->Scale(1.0/((float)eventN*eventtime));

for(int aa=0;aa<nChannels;aa++){
	float nR=rawNoiseRate->GetBinContent(aa);
	summary->Fill(nR,aa);
}

summary->Write();
rawNoiseRate->Write();
echo_leHisto->Write();
echo_teHisto->Write();
totHisto->Write();
totInfo->Write();
//for(int aa=0;aa<nChannels;aa++){totHistos[aa]->Write();}


correlation->calc();
float corrOut[nChannels*nChannels];
float corrOut2[24*24];

correlation->get_cor(corrOut);
//for(int aa=0;aa<nChannels;aa++){corrOut[aa*nChannels+aa]=0;}//set diagonal to 0;
for(int aa=0;aa<nChannels;aa++){
	for(int bb=0;bb<nChannels;bb++){

		if(aa==bb){ corrPlot->Fill(aa,bb, 0);} //may be backwards
		else{ corrPlot->Fill(aa,bb,corrOut[aa*nChannels+bb]);}//may be backwards
	}}	
corrPlot->Write();

for(int aa=0;aa<nStacks;aa++){

	correlationSmall[aa].calc();
	correlationSmall[aa].get_cor(corrOut2);
	for(int bb=0;bb<24;bb++){
		for(int cc=0;cc<24;cc++){
			corrPlot2[aa]->Fill(bb,cc,corrOut2[bb*24+cc]);
		}}	
	corrPlot2[aa]->Write();
}

/////////////////////////cout text file////////////////////////////////


//printf("stack#\t\tModule#\tslot\tp1\t\tp2\t\tp3\t\tp4\t\tp5\t\tp6\t\tave\t\tmin\t\tmax\n");
printf("stack#\tslot\tModule#\tp1\tp2\tp3\tp4\tp5\tp6\tave\tmin\tmax\n");
FILE *outTextFile;
string text_str="outTexts/";
text_str+=buffer;
text_str+=".txt";
//outTextFile=fopen(text_str.c_str(),"w");

int pad;
float pp[6];
float minP,maxP,aveP;
int stack=0;

for(int aa=1;aa<=nChannels;aa++){
	float rate=rawNoiseRate->GetBinContent(aa);
	pad=(aa-1)%6;
	//cout<<"aa: "<<aa<<"\tpad: "<<pad<<endl;
	pp[pad]=rate;

	if((aa-1)%24==0){stack++;}

	if((aa-1)%6==5){ 
		minP=pp[0];
		maxP=pp[0];
		for(int bb=0;bb<6;bb++){
			aveP+=pp[bb];
			if(pp[bb]<minP) minP=pp[bb];
			if(pp[bb]>maxP) maxP=pp[bb];
		}
		aveP/=6;

		//char buffer[40];
		//mrpcNames[aa-1].copy(buffer,0,7);
		//cout<<buffer<<endl;
		//printf("stack#%i\t%i\t%s\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",stack,aa/6,mrpcNames[aa/6-1].c_str(),pp[0],pp[1],pp[2],pp[3],pp[4],pp[5],aveP,minP,maxP);
//	printf("stack#%i\t%i\t%s\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\n",stack,aa/6,mrpcNames[aa/6-1].c_str(),(int)pp[0],(int)pp[1],(int)pp[2],(int)pp[3],(int)pp[4],(int)pp[5],(int)aveP,(int)minP,(int)maxP);
//		fprintf(outTextFile, "%s\t%i\t%i\t%s\t%i\t%i\t%i\t%i\t%i\t%i\n",buffer.c_str(),stack,aa/6,mrpcNames[aa/6-1].c_str(),(int)pp[0],(int)pp[1],(int)pp[2],(int)pp[3],(int)pp[4],(int)pp[5]);
	printf("stack#%i\t%i\t%s\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\n",stack,aa/6,mrpcNames[aa/6 -1].c_str(),(int)pp[0],(int)pp[1],(int)pp[2],(int)pp[3],(int)pp[4],(int)pp[5],(int)aveP,(int)minP,(int)maxP);
//	fprintf(outTextFile, "%s\t%i\t%i\t%s\t%i\t%i\t%i\t%i\t%i\t%i\n","name",stack,aa/6,"name",(int)pp[0],(int)pp[1],(int)pp[2],(int)pp[3],(int)pp[4],(int)pp[5]);
	}
	aveP=0;

}

//fclose(outTextFile);
outFile.Close();

cout<<"total number of good events: "<<eventN<<endl;
cout<<"total number of bad events: "<<badEventN<<endl;

return 0;
}


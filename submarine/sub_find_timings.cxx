#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>

#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TF2.h>
#include <TGraph.h>
#include <TNtuple.h>

#include "TsPacket.h"
#include "regMany.h"


using namespace std;
//typedef struct { int id; float time;} aHit;
std::vector<float> pairTimings(vector<float> a,vector<float> b, int channel)
{

	const int myToTcut=5000;

	vector<float> pairs;
	pairs.clear();
	
/*
	float zero=0;
	if(a.size()){cout<<"le's:\t"; zero=a[0];}
	for(int aa=0;aa<a.size();aa++){cout<<a[aa]-zero<<"\t";}
	if(b.size()){cout<<endl<<"te's:\t";}
	if(b.size() && zero==0){zero=b[0];}
	for(int bb=0;bb<b.size();bb++){cout<<b[bb]-zero<<"\t";}
*/		

	//for now just take 0,0 -- this can use some tuning later
	if(a.size()>0 && b.size()>0) {
		//if(b[0]-a[0]<myToTcut) {
			pairs.push_back(b[0]-a[0]);
//			cout<<endl;
		}//}
	return pairs; 
}


int main(int acgc, char* argv[])
{

	const int maxEventN=500000-1;
	bool seeEvents=false;
	bool DEBUG=false;
	bool correctINL=true;

	const int nMRPCs=96*2;

	ifstream io(argv[1]);
	if(!io){cerr<<"no input file!!"<<endl; exit(1);}
	string inName=argv[1];
	char buffer[256];
	cout<<inName<<": "<<inName.find("set")<<endl;
	int length=inName.copy(buffer,3,inName.find("set")+3);
	cout<<"Set#: "<<buffer<<"\tlegnth: "<<length<<endl;


	string mrpcPath="mrpcTexts/set";
	mrpcPath+=buffer;
	mrpcPath+=".mrpcs";
	cout<<mrpcPath<<endl;


	ifstream loader(mrpcPath.c_str());
	string mrpcNames[32];
	
	int nInput=0;
	char buffer2[100];
	if(!loader.is_open()){cerr<<"no mrpc list!!"<<endl; exit(1);}
	while(loader.good() && nInput<32)
	{  
		loader.getline(buffer2,100);
		mrpcNames[nInput]=buffer2;
		cout<<nInput<<"\t"<<mrpcNames[nInput]<<endl;
		nInput++;
	}
	loader.close();
	loader.clear();

	//load inl tables
	float corrections[8][3][8][1024];//tdig tdc channel et
	memset(corrections, 0, 8*3*8*1024*sizeof(float));
	int tDIGs[]={78,81,86,87,94,95,96,99};



	float dump;
	for(int nTDiG=0;nTDiG<3;nTDiG++){
		int chCounter=0;
		for(int nTDC=0;nTDC<3;nTDC++){
			for(int channel=0;channel<8;channel++){
				
				//printf("inl_tables/tdig-ser%i/inl/tdig-ser%i.tdc%i.ch%i.inl\n",tDIGs[nTDiG],tDIGs[nTDiG],nTDC,channel);
				sprintf(buffer,"inl_tables/tdig-ser%i/inl/tdig-ser%i.tdc%i.ch%i.inl",tDIGs[nTDiG],tDIGs[nTDiG],nTDC,channel);
				chCounter++;
				loader.open(buffer);
				//loader.seekg(0,ios::beg);
				if(!loader.is_open()){cerr<<"Missing INL corrections for TDIG: "<<nTDC<<endl; exit(1);}
				int inlCounter=0;
				while(loader.good()){
				loader>>dump>>corrections[nTDiG][nTDC][channel][inlCounter];
				//cout<<corrections[chCounter][inlCounter]<<endl;
				inlCounter++;
				}
				loader.close();
				loader.clear();
				chCounter++;
			}
		}
	}

	if(!correctINL){memset(corrections, 0, nMRPCs*1024*sizeof(float));}



	//for(int aa=0;aa<32;aa++){cout<<aa<<"\t"<<mrpcNames[aa]<<endl;	}

	io >> hex;

	unsigned int n,m;

	int tray_half=0;
	int eventN=0;
	int badEventN=0;
	int eventStep=0;//to ensure that nothing is bad is afoot

	vector<float> LEs[nMRPCs];
	vector<float> TEs[nMRPCs];
	vector<float> tots[nMRPCs];

	int nBinsTiming=100;
	//const int largeOffset=2097151;
	const int largeOffset=20000;
	const int minBin=4020;


	TFile outFile("quick.root","recreate");


	TH2F* nCosmics=new TH2F("nCosmics","Cosmics Per Event",nMRPCs,0,nMRPCs-1,nMRPCs,0,nMRPCs-1);
	TH1F* nHits=new TH1F("nHits","Pad Hits Per Event",200,0,201);



	/*

	//here is where things got erased on accident....


	sprintf( hname, "hArray_%i_%i", aa,bb);
	sprintf( htitle, "Set %i %i", aa,bb);
	hArray[aa][bb] = new TH1F(hname,htitle,2*nBinsTiming,-largeOffset,largeOffset);
	}}
	 */	 

	TH1F *hTots[nMRPCs];
	for(int aa=0;aa<nMRPCs;aa++){
		TString htitle="ToT ";
		htitle+=mrpcNames[aa/6];
		htitle+=" ";
		htitle+=aa%6;

		//char htitle[150]; 
		char hname[150]; 
		sprintf( hname, "tot_ch%i ",aa );
		//sprintf( htitle, "Tots %s", mrpcNames[aa].c_str() );
		hTots[aa] = new TH1F(hname,htitle,nBinsTiming,4000,40000);
	}

TH1F  *tDiffs[8][6][4][4];//stack/modules/channel1/channel2
TH2F  *slew[8][6][4][4];//stack/modules/channel1/channel2



for(int st=0;st<8;st++){
	for(int ch=0;ch<6;ch++){
		for(int aa=0;aa<4;aa++){
			for(int bb=0;bb<4;bb++){
				char hname[150];
				char htitle[150];  
				sprintf( hname, "tDiffs_st%i_ch%i_aa%i_bb%i", st,ch,aa,bb);
				sprintf( htitle, "Set %i %i %i %i", st,ch,aa,bb);
				tDiffs[st][ch][aa][bb] = new TH1F(hname,htitle,2*nBinsTiming,-4000,4000);
				sprintf( hname, "Slewing_st%i_ch%i_aa%i_bb%i", st,ch,aa,bb);
				sprintf( htitle, "Slewing %i %i %i %i", st,ch,aa,bb);
				slew[st][ch][aa][bb] = new TH2F(hname,htitle,20,10000,25000,40,-2000,2000);
			}}}}




//const float totCut=500;//max tot


int line=0;
int babab=0;
bool corrupt=false;
int totalHits=0;
int eventsWithHits=0;
int cosmicsFound=0;

int isTrack[8][8];//stacks, channels (0)1-6(7)
int  marker[8][4][6];
int tempSwitch[8][6];

memset(marker, 0, 8*4*6*sizeof(int));
memset(isTrack, 0, 8*8*sizeof(int));
memset(tempSwitch, 0, 8*6*sizeof(int));

//	while (io >> m  && eventN<maxEventN){
while (io >> m ){

	line++;

	io >> n;

	//cout<<hex<<n<<endl;

	//if(eventN > maxEventN){if(!babab++){cout<<"\n\nEnding early!\n\n"<<endl;} continue;}


	TsPacket *p=new TsPacket(n);
	//p->Print();

	//int fID=p->GetID();	




	//cout<<eventStep<<endl;

	//
	if(n==0xc0000001 ){
		tray_half=2;
		eventStep=1;}//start event read of TDIG 4-7
		if( (n & 0xF0000000) >> 28 == 0x2  )//now should see a header
		{

			int triggertime=(n & 0xFFF);
				
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

			//if(corrupt==false){
			//if(corrupt==false && totalHits<=20){
			if(1){

				for(int aa=0;aa<nMRPCs;aa++){

					tots[aa]=pairTimings(LEs[aa],TEs[aa],aa);//match leading and trailing edges
					if(tots[aa].size()){hTots[aa]->Fill(tots[aa][0]);}
				}


				for(int st=0;st<8;st++){
					for(int ch=0;ch<6;ch++){
						for(int mod=0;mod<4;mod++){
							//	if(tempSwitch[st][ch])cout<<mod<<" marker: "<<marker[st][mod][ch]<<endl;
							isTrack[st][ch+1]=isTrack[st][ch+1]+marker[st][mod][ch];
							//	if(tempSwitch[st][ch])cout<<mod<<" isTrack: "<<isTrack[st][ch]<<endl;
							totalHits+=marker[st][mod][ch];
							//if(tempSwitch[st][ch])cout<<totalHits<<endl;
						}

						if(isTrack[st][ch+1]>=3 ){//&& (isTrack[st][ch]<=0 && isTrack[st][ch+2]<=0) ){
						

/*
							for(int mod=0;mod<4;mod++){
								int aa=st*24+mod*6+ch;		
								if(tots[aa].size()){hTots[aa]->Fill(tots[aa][0]);}
							}
						
*/

							cosmicsFound++;
							for(int aa=0;aa<4;aa++){
								for(int bb=0;bb<aa;bb++){
									int mrpc1=st*24+ch+aa*6;
									int mrpc2=st*24+ch+bb*6;


									//if(marker[st][aa][ch] && marker[st][bb][ch]){
									if(tots[mrpc1].size() && tots[mrpc2].size()){
										float timingDiff=-(LEs[mrpc2][0]-LEs[mrpc1][0]);
										tDiffs[st][ch][aa][bb]->Fill(timingDiff);
										//hArray[mrpc1][mrpc2]->Fill(timingDiff);
										nCosmics->Fill(mrpc1,mrpc2);
										slew[st][ch][aa][bb]->Fill(tots[mrpc1][0],timingDiff);
										slew[st][ch][bb][aa]->Fill(tots[mrpc2][0],timingDiff);
										// //slew[st][ch][aa][bb]->Fill(timingDiff,0);
									}
								}}

							}}}


							if(totalHits){
								nHits->Fill(totalHits);
								eventsWithHits++;
								if(seeEvents)cout<<"---------------------"<<endl;
							}
							totalHits=0;

							eventN++;
					}//end event if good


					if(corrupt==true) {badEventN++;}







					memset(marker, 0, 8*4*6*sizeof(int));
					memset(isTrack, 0, 8*8*sizeof(int));
					memset(tempSwitch, 0, 8*6*sizeof(int));



					for(int aa=0;aa<nMRPCs;aa++){

						//					if(LEs[aa].size()>0){printf("\nabChannel: %i\tsize: %i\tcounter: %i",aa,LEs[aa].size(),0);}
						tots[aa].clear();				
						LEs[aa].clear();
						TEs[aa].clear();
					}		


					//			if(DEBUG)cout<<"end event!"<<endl<<endl;





				}


				int fChannel = p->GetChannelTray4();
				int abChannel=fChannel+(tray_half-1)*96;
				if(abChannel>191){cout<<"\n"<<abChannel<<"\n"<<endl;}


				if( p->IsLE() ){ 
					unsigned int et=p->GetEdgeTime();
					int the_tdig=(int)p->tdig;
					int the_tdc=(int)p->out_tdc;
					int the_channel=(int)p->out_channel;
					float deltaEt=corrections[the_tdig][the_tdc][the_channel][(int)et%1024];

					//printf("fChan: %i\tthe_Channel: %i\ttddc: %i\t tdigSerial#: %i\ttime: %i\tmod1024: %i\tcorrection: %f\n",fChannel,the_channel,the_tdc,tDIGs[(int)p->tdig],et,et%1024,deltaEt);

					float time=( et + deltaEt)*25.0;//25 pico seconds per bin
					LEs[abChannel].push_back(time);
					int stack=abChannel/24;
					int mod=(abChannel%24)/6;
					int channel=(abChannel%24)%6;
					marker[stack][mod][channel]=1;
					tempSwitch[stack][channel]=1;
					if(seeEvents)printf("ch: %i\tstack: %i\tmod: %i\tchan: %i\n",abChannel,stack,mod,channel);


					//if(DEBUG)cout<<"le @ channel: "<<fChannel<<"\ttray_half: "<<tray_half<<endl;

				}

				if(p->IsTE() ){
					unsigned int et=p->GetEdgeTime();
					int the_tdig=(int)p->tdig;
					int the_tdc=(int)p->out_tdc;
					int the_channel=(int)p->out_channel;
					float deltaEt=corrections[the_tdig][the_tdc][the_channel][(int)et%1024];
					float time=( et + deltaEt)*25.0;//25 pico seconds per bin
					//float time=et*25.0;//25 pico seconds per bin
					TEs[abChannel].push_back(time);
					//if(DEBUG)cout<<"te @ channel: "<<abChannel<<endl;
				}




				delete p;


			}




			cout<<endl<<endl<<"Finished Reading Data, now saving results."<<endl;


			int timings[nMRPCs][nMRPCs];
			//float resolutions[nMRPCs][nMRPCs];
			float resolutions[8][6][4][4];



			/*
			   for(int aa=0;aa<nMRPCs;aa++){	for(int bb=0;bb<nMRPCs;bb++){
			   int daBin=hArray[aa][bb]->GetMaximumBin();//use to set timings

			   if(daBin==1){timings[aa][bb]=0; resolutions[aa][bb]=0;}
			   else{
			   timings[aa][bb]=hArray[aa][bb]->GetBinCenter(daBin);
			   hArray[aa][bb]->Fit("gaus");
			//float par0=hArray[aa][bb]->GetFunction("gaus")->GetParameter(0);
			//float par1=hArray[aa][bb]->GetFunction("gaus")->GetParameter(1);
			float par2=hArray[aa][bb]->GetFunction("gaus")->GetParameter(2);
			resolutions[aa][bb]=par2;
			hArray[aa][bb]->Write();
			}


			}}
			 */


			cout<<"saving ToT histograms..."<<endl;

			for(int aa=0;aa<nMRPCs;aa++){hTots[aa]->Write();}

			cout<<"saving dT and slewing histograms..."<<endl;

			TF2* f2d[8][6][4][4];
			float estTimings[nMRPCs];
			float inc[nMRPCs];
			for(int aa=0;aa<nMRPCs;aa++){estTimings[aa]=0;inc[aa]=0;}

			for(int st=0;st<8;st++){
				for(int ch=0;ch<6;ch++){
					for(int aa=0;aa<4;aa++){
						for(int dd=0;dd<aa;dd++){
							int daBin=tDiffs[st][ch][aa][dd]->GetMaximumBin();//use to set timings
							if(daBin==1){
								resolutions[st][ch][aa][dd]=0;
							}
							else{
								tDiffs[st][ch][aa][dd]->Fit("gaus");
								float par2=tDiffs[st][ch][aa][dd]->GetFunction("gaus")->GetParameter(2);
								resolutions[st][ch][aa][dd]=par2;
								int which1=st*24+aa*6+ch;
								int which2=st*24+dd*6+ch;
								cout<<par2<<endl;
								estTimings[which1]+=par2/sqrt(2.0);inc[which1]++;
								estTimings[which2]+=par2/sqrt(2.0);inc[which2]++;
								//	printf( "Set %i %i %i_ %i_ \n", st,ch,aa,dd);
								tDiffs[st][ch][aa][dd]->Write();
								//	char fname[150];
								//	sprintf( fname, "f2d_%i_%i_%i_%i", st,ch,aa,dd);
								//	f2d[st][ch][aa][dd]=new TF2(fname,"[0]+[1]*y",0,30000,-2000,2000);
								//slew[st][ch][aa][dd]->FitSlicesY();
								slew[st][ch][aa][dd]->Write();
								slew[st][ch][dd][aa]->Write();

							}


						}
					}
				}
			}

			for(int aa=0;aa<nMRPCs;aa++){estTimings[aa]/=inc[aa];  cout<<"nMRPC["<<aa<<"]: "<<estTimings[aa]<<endl;}

			TGraph* testem=new TGraph(slew[4][2][2][1]);
			testem->SetTitle("testem");
			testem->Draw("surf1");
			testem->Write();





			cout<<"saving nHits and nCosmics histograms..."<<endl;

			nHits->Write();
			nCosmics->Write();

			outFile.Close();

			//for(int aa=0;aa<nMRPCs;aa++){for(int bb=0;bb<nMRPCs;bb++){if(timings[aa][bb]!=0)hArray[aa][bb]->Write();}}

			cout<<"writing out timing resolutions..."<<endl;

			//f->Close();
			TString outName="raw_timing_res_";
			outName+="addSetNumber.text";
			const char* realOutName=outName;
			ofstream writeTimings(realOutName);
			for(int aa=0;aa<nMRPCs;aa++){
				for(int bb=0;bb<nMRPCs;bb++) {
					writeTimings <<  resolutions[aa][bb] << " ";
					//if(!out.good()) break;
				}
				writeTimings<< endl;
			}


			writeTimings.close();





			cout<<"total number of good events: "<<eventN<<endl;
			cout<<"total number of bad events: "<<badEventN<<endl;
			cout<<"percent good = "<<(float)eventN/(float)(eventN+badEventN)*100.<<"%"<<endl;
			cout<<"percent Events with Hits: "<<(float)eventsWithHits/(float)eventN*100.<<"%"<<endl;
			cout<<"cosmics per event: "<<(float)cosmicsFound/(float)eventN<<endl;



			return 0;
		}


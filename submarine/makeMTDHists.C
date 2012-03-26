int makeMTDHists (
		  TString inFile="/home/tof/submarine/rootfiles/USTC_1_7300_2500.dat.root",
		  TString stInstitute="USTC",
		  TString stMrpcNum="4",
		  TString stHV="6300",
		  TString stThr="2500",
		  TString stNEvents="-1"
		  )
  
{

  cout<<"loading file: "<<inFile<<endl;
  TFile T(inFile);

  //TString basicTitle="USTC, MRPC #4, HV=6300, Thr=2500";
  TString basicTitle = stInstitute
    + ", MRPC #" + stMrpcNum 
    + ", HV=" + stHV
    + ", Thr=" + stThr;

  //Save the Output as...
//   TString psFiles="psFiles/";
//   TString fileType="pdf";

   int POSTSCRIPT_MULTI=1;


  //=========Macro generated from canvas: c1/c1
  //=========  (Tue Nov 23 14:24:52 2010) by ROOT version5.18/00b


  // create file name from function arguments
//   TString stFileName = psFiles +
//     stInstitute + "_" + 
//     stMrpcNum + "_" + 
//     stHV + "V_" +
//     stThr + "T." +
//     fileType;

  TString stFileName = TString(inFile(0, inFile.Last('/')+1) + "results.pdf");
  cout << "results saved to " << stFileName <<  endl;
  TString runtimesFileName = TString(inFile(0, inFile.Last('/')+1) + "runtimes");
  TString runName1 = TString(inFile(0,inFile.Last('/')));
  TString runName = TString(runName1(runName1.Last('/')+1,runName1.Last('/')+10));

  ifstream runtimesFile(runtimesFileName);
  time_t startT;
  time_t endT;
  runtimesFile >> startT;
  runtimesFile >> endT;
  char startTStr[] = ctime(&startT);
  startTStr[strlen(startTStr)-1] = 0;
  char endTStr[] = ctime(&endT);
  startTStr[strlen(endTStr)-1] = 0;
  

  //-------------------- Start Configuration text -------------------------//
  string confTable;
  TCanvas *c2 = new TCanvas("c2", "c2",1280,87,1105,936);
  TPaveText textBox1(0.0, .1, .9, 0.9);
  textBox1.SetTextAlign(1);
  textBox1.AddText("");
  textBox1.AddText("        Configuration");
  textBox1.AddLine(0.0, 0.85, 1.0, 0.85);
  textBox1.AddText("");
  confTable = Form("%12c Run :   %s", ' ', (const char *)(runName));
  textBox1.AddText(confTable.c_str());
  confTable = Form("%14c HV :   %sV", ' ', (const char *)stHV);
  textBox1.AddText(confTable.c_str());
  confTable = Form("%9c MRPC :   %s", ' ', (const char *)(stInstitute+"_"+stMrpcNum));
  textBox1.AddText(confTable.c_str());
  confTable = Form("%2c Threshold :   %s", ' ', (const char *)(stThr));
  textBox1.AddText(confTable.c_str());
  confTable = Form("%11c Start :   %s", ' ', startTStr);
  textBox1.AddText(confTable.c_str());
  confTable = Form("%12c End :   %s", ' ', endTStr);
  textBox1.AddText(confTable.c_str());
  confTable = Form("%3c Run Time :   %d sec", ' ', (int)(endT-startT));
  textBox1.AddText(confTable.c_str());
  confTable = Form("%7c Events :   %s", ' ', (const char *)stNEvents);
  textBox1.AddText(confTable.c_str());
  textBox1.AddText("");
  textBox1.AddText("");
  textBox1.AddText("");
  textBox1.Draw();
  c2->Update();
  if(POSTSCRIPT_MULTI)   c2->Print(stFileName+"(", "Title:Configuration");

  delete c2;
  //-------------------- End Configuration text -------------------------//


  //-------------------- Start Histo 1 ~ Noise -------------------------//


  TCanvas *c1 = new TCanvas("c1", "c1",1280,87,1105,936);
  // gStyle->SetOptStat(0);
  c1->Range(-3.125,-16.37012,28.15341,146.6115);
  c1->SetBorderSize(2);
  c1->SetFrameFillColor(0);

  //TH1 *noise = new TH1F("noise","TSU, MRPC #1, HV=7300, Nino Thrsh=2500",25,0,25);
  noise->SetTitle(basicTitle);


  Int_t ci;   // for color index setting
  ci = TColor::GetColor("#ff6600");
  noise->SetFillColor(ci);
  // noise->SetFillStyle(1);
  noise->SetMarkerStyle(2);
  noise->SetMarkerSize(2.1);
  noise->GetXaxis()->SetTitle("Channel# (0-11) & (12-23)");
  noise->GetXaxis()->CenterTitle(true);
  noise->GetYaxis()->SetTitle("Noise Rate (Hz)");
  noise->GetYaxis()->CenterTitle(true);
  noise->SetMinimum(0);
  noise->Draw("BARP");

  TPaveText *pt = new TPaveText(0.01,0.945,0.71,0.995,"blNDC");
  pt->SetName("title");
  pt->SetBorderSize(2);
  pt->SetFillColor(19);
  TText *text = pt->AddText(basicTitle);
  pt->Draw();
  TLine *line = new TLine(11.98864,-0.03483106,11.98864,c1->GetUymax());
  line->Draw();

  tex = new TLatex(2.239441,(noise->GetMaximum())/2.0,"High Z");
  tex->SetLineWidth(2);
  tex->Draw();
  tex = new TLatex(14.61455,(noise->GetMaximum())/2.0 , "Low Z");
  tex->SetLineWidth(2);
  tex->Draw();

  c1->Modified();
  c1->cd();
  c1->SetSelected(c1);


  //if(POSTSCRIPT_MULTI)   c1->Print(stFileName+"(", "Title:Noise");
  if(POSTSCRIPT_MULTI)   c1->Print(stFileName, "Title:Noise");
  //-------------------- End Histo 1 ~ Noise -------------------------//



  //---------------------Start Summary Noise Plot --------------------//
  TH1D* summaryNoise=new TH1D("summaryNoise","Summary of Noise",30,0,200);

  for(int aa=1;aa<noise->GetNbinsX();aa++){
    double content=noise->GetCellContent(aa,0);
    cout<<aa<<": "<<content<<endl;
    summaryNoise->Fill(noise->GetCellContent(aa,0));
  }
  ci = TColor::GetColor("#ff6600");
  summaryNoise->SetFillColor(ci);
  // noise->SetFillStyle(1);
  summaryNoise->SetMarkerStyle(2);
  summaryNoise->SetMarkerSize(2.1);
  summaryNoise->GetXaxis()->SetTitle("Noise Rate (Hz)");
  summaryNoise->GetXaxis()->CenterTitle(true);
  summaryNoise->GetYaxis()->SetTitle("Count of Channels");
  summaryNoise->GetYaxis()->CenterTitle(true);
  summaryNoise->Draw("BARP");


  summaryNoise->Draw();
  if(POSTSCRIPT_MULTI)   c1->Print(stFileName, "Title:Summary of Noise");
  //---------------------End Summary Noise Plot --------------------//


  //-------------------- Start Histo 2 ~ Correlations BW -------------------------//
  corrPlot->SetTitle(basicTitle);
  corrPlot->Draw();
  if(POSTSCRIPT_MULTI)   c1->Print(stFileName, "Title:Correlations BW");
  //-------------------- End   Histo 2 ~ Correlations BW -------------------------//


  //-------------------- Start Histo 3 ~ Correlations Color-----------------------//
  gStyle->SetPalette(1);
  corrPlot->Draw("lego2");
  if(POSTSCRIPT_MULTI)   c1->Print(stFileName, "Title:Correlations Color");
  //-------------------- End   Histo 3 ~ Correlations Color-----------------------//


  //-------------------- Start Histo 4 ~ ToT's Total       -----------------------//
  tots->SetTitle(basicTitle);
  tots->Draw();
  if(POSTSCRIPT_MULTI)   c1->Print(stFileName, "Title:ToTs");
  //-------------------- End Histo 4 ~ ToT's Total       -----------------------//



  c1->Clear();
  c1->Divide(3,4);
  TString title;

  int totColor=24;
  title="tot_0"; c1->cd(1); tot_0->Draw();
  title="tot_1"; c1->cd(2); tot_1->Draw();
  title="tot_2"; c1->cd(3); tot_2->Draw();
  title="tot_3"; c1->cd(4); tot_3->Draw();
  title="tot_4"; c1->cd(5); tot_4->Draw();
  title="tot_5"; c1->cd(6); tot_5->Draw();
  title="tot_6"; c1->cd(7); tot_6->Draw();
  title="tot_7"; c1->cd(8); tot_7->Draw();
  title="tot_8"; c1->cd(9); tot_8->Draw();
  title="tot_9"; c1->cd(10); tot_9->Draw();
  title="tot_10"; c1->cd(11); tot_10->Draw();
  title="tot_11"; c1->cd(12); tot_11->Draw();


  c1->Update();
  if(POSTSCRIPT_MULTI)   c1->Print(stFileName, "Title:ToT 0 - 11");

  c1->Clear();
  c1->Divide(3,4);
  title="tot_12"; c1->cd(1); tot_12->Draw();
  title="tot_13"; c1->cd(2); tot_13->Draw();
  title="tot_14"; c1->cd(3); tot_14->Draw();
  title="tot_15"; c1->cd(4); tot_15->Draw();
  title="tot_16"; c1->cd(5); tot_16->Draw();
  title="tot_17"; c1->cd(6); tot_17->Draw();
  title="tot_18"; c1->cd(7); tot_18->Draw();
  title="tot_19"; c1->cd(8); tot_19->Draw();
  title="tot_20"; c1->cd(9); tot_20->Draw();
  title="tot_21"; c1->cd(10); tot_21->Draw();
  title="tot_22"; c1->cd(11); tot_22->Draw();
  title="tot_23"; c1->cd(12); tot_23->Draw();


  c1->Update();
  if(POSTSCRIPT_MULTI)   c1->Print(stFileName, "Title:ToT 12 - 23");

  c1->Clear();
  delete c1;
  TCanvas *c2 = new TCanvas("c2", "c2",1280,87,1105,936);
  TPaveText textBox(0.0, .1, .9, 0.9);
  textBox.SetTextAlign(1);
  textBox.AddText("");
  textBox.AddText("\ \ \ Channel Number	\ \ \ \ \ \ \ \ \ \ Noise Rate");
  textBox.AddLine(0.0, 0.945, 1.0, 0.945);
  string noiseTable;
  float tempNoise;
  for(int aa=1;aa<=9;aa++){
    tempNoise=noise->GetBinContent(aa);
    noiseTable=Form("%10c %d %32c %4.2f", ' ',aa, ' ', tempNoise);
    textBox.AddText(noiseTable.c_str());
  }
  for(int aa=10;aa<=24;aa++){
    tempNoise=noise->GetBinContent(aa);
    noiseTable=Form("%10c %d %30c %4.2f", ' ',aa, ' ', tempNoise);
    textBox.AddText(noiseTable.c_str());
  }
  for(int aa=0;aa<4;aa++){
    textBox.AddText("");
  }


  textBox.AddLine(0.2, 1.0 , 0.2, 0.14);
  textBox.AddLine(0.0, 0.14, 1.0, 0.14);
  textBox.Draw();


  c2->Update();
  if(POSTSCRIPT_MULTI)   c2->Print(stFileName, "Title:Noise Rates");

  delete c2;
  TCanvas *c3 = new TCanvas("c3", "c3",1280,87,1105,936);
  c3->Clear();
  c3->Divide(3,2);
		
  totTuple->SetFillColor(33);
  c3->cd(1);z_0->Draw();
  c3->cd(2);z_1->Draw();
  c3->cd(3);z_2->Draw();
  c3->cd(4);z_3->Draw();
  c3->cd(5);z_4->Draw();
  c3->cd(6);z_5->Draw();
  c3->Update();

  if(POSTSCRIPT_MULTI)   c3->Print(stFileName, "Title:z 0-5");
	
  c3->cd(1);z_6->Draw();
  c3->cd(2);z_7->Draw();
  c3->cd(3);z_8->Draw();
  c3->cd(4);z_9->Draw();
  c3->cd(5);z_10->Draw();
  c3->cd(6);z_11->Draw();
  c3->Update();

  //if(POSTSCRIPT_MULTI)   c3->Print(psFiles+"test."+fileType, "Title:Jo5");

  if(POSTSCRIPT_MULTI)   c3->Print(stFileName+")", "Title:z 6-11");

  gApplication->Terminate();
  //return;


}

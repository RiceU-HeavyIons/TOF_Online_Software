int makeMTDHists(TString inFile="/home/dylan/data/mtd_ustc002_noise002.dat.root")
{

	cout<<"loading file: "<<inFile<<endl;
	TFile T(inFile);

	TString basicTitle="USTC, MRPC #1, HV=7300, Thr=1500";

	//Save the Output as...
	int POSTSCRIPT_MULTI=1;
	TString psFiles="psFiles/";
	TString fileType="ps";//I don't think others work



	//=========Macro generated from canvas: c1/c1
	//=========  (Tue Nov 23 14:24:52 2010) by ROOT version5.18/00b


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
	noise->Draw("BARP");

	TPaveText *pt = new TPaveText(0.01,0.945,0.71,0.995,"blNDC");
	pt->SetName("title");
	pt->SetBorderSize(2);
	pt->SetFillColor(19);
	TText *text = pt->AddText(basicTitle);
	pt->Draw();
	TLine *line = new TLine(11.98864,-0.03483106,11.98864,130.4618);
	line->Draw();

	tex = new TLatex(2.239441,14.2183,"High Z");
	tex->SetLineWidth(2);
	tex->Draw();
	tex = new TLatex(14.61455,13.65999,"Low Z");
	tex->SetLineWidth(2);
	tex->Draw();

	c1->Modified();
	c1->cd();
	c1->SetSelected(c1);
	if(POSTSCRIPT_MULTI)   c1->Print(psFiles+"test."+fileType+"[");
	if(POSTSCRIPT_MULTI)   c1->Print(psFiles+"test."+fileType);



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
	if(POSTSCRIPT_MULTI)   c1->Print(psFiles+"test."+fileType);



	//-------------------- Start Histo 2 ~ Correlations BW -------------------------//



	corrPlot->SetTitle(basicTitle);
	corrPlot->Draw();
	if(POSTSCRIPT_MULTI)   c1->Print(psFiles+"test."+fileType);
	//-------------------- End   Histo 2 ~ Correlations BW -------------------------//
	//-------------------- Start Histo 3 ~ Correlations Color-----------------------//

	gStyle->SetPalette(1);
	corrPlot->Draw("lego2");
	if(POSTSCRIPT_MULTI)   c1->Print(psFiles+"test."+fileType);

	//-------------------- End   Histo 3 ~ Correlations Color-----------------------//

	//-------------------- Start Histo 4 ~ ToT's Total       -----------------------//

	tots->SetTitle(basicTitle);
	tots->Draw();
	if(POSTSCRIPT_MULTI)   c1->Print(psFiles+"test."+fileType);

	//-------------------- End Histo 4 ~ ToT's Total       -----------------------//


	c1->Clear();
	c1->Divide(3,4);
	TString title;

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
	if(POSTSCRIPT_MULTI)   c1->Print(psFiles+"test."+fileType);

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
	if(POSTSCRIPT_MULTI)   c1->Print(psFiles+"test."+fileType);
	if(POSTSCRIPT_MULTI)   c1->Print(psFiles+"test."+fileType+"]");


	return;


}

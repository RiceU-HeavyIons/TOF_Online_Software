int makeTOFHists(TString inFile="/home/dylan/projects/submarine/data/noise_set078_1.dat.root",
	TString basicTitle="TOF Tray 03, TH=1200, HV=7000")
{

	cout<<"loading file: "<<inFile<<endl;
	TFile T(inFile);


	//Save the Output as...
	int POSTSCRIPT_MULTI=1;
	TString psFiles="psFiles/";
	TString fileType="ps";//I don't think others work


	//=========Macro generated from canvas: c1/c1
	//=========  (Tue Nov 23 14:24:52 2010) by ROOT version5.18/00b


	//-------------------- Start Histo 0 ~ Histo of Noise -------------------------//

	TCanvas *c1 = new TCanvas("c1", "c1",1280,87,1105,936);

	int noiseMin=0;
	int noiseMax=100;
	int nChannels=192;
	int ci = TColor::GetColor("#ff9900");


	TH1D*  noise0=new TH1D("NoiseRates","NoiseRates",noiseMax-noiseMin+1, noiseMin,noiseMax);
	for( int aa=1;aa<=nChannels;aa++){ double value=noise->GetCellContent(aa,0); if(value > noiseMax) { value=noiseMax-0.5;} noise0->Fill(value); }
	noise0->SetFillColor(ci);
	noise0->GetXaxis()->SetTitle("Noise Rate (Hz)");
	noise0->GetXaxis()->CenterTitle(true);
	noise0->GetYaxis()->SetTitle("Counts");
	noise0->GetYaxis()->CenterTitle(true);
	noise0->SetTitle(basicTitle);
	noise0->Draw("BARP");
	if(POSTSCRIPT_MULTI)   c1->Print(psFiles+"test."+fileType+"[");
	if(POSTSCRIPT_MULTI)   c1->Print(psFiles+"test."+fileType);


	//-------------------- Start Histo 1 ~ Noise -------------------------//

	c1->Clear();
	c1->Divide(1,2);

	TH1D* half1=new TH1D("firstHalf","firstHalf",nChannels/2,1,nChannels/2);
	TH1D* half2=new TH1D("secondHalf","secondHalf",nChannels/2,1,nChannels/2);

	half1->SetMinimum(0);
	half2->SetMinimum(0);

	double maxV=0;
	for(int aa=1;aa<=nChannels/2;aa++){ 
		int val1=noise->GetCellContent(aa,0); 
		int val2=noise->GetCellContent(aa+nChannels/2,0); 
		half1->SetBinContent(aa,val1); 
		half2->SetBinContent(aa,val2); 
		if(val1>maxV){ maxV=val1;}
		if(val2>maxV){ maxV=val2;}
	}

	half1->SetMaximum(maxV);
	half2->SetMaximum(maxV);

	// Draw 1st half of TOF
	c1->cd(1);
	half1->SetFillColor(ci);
	half1->SetStats(0);
	half1->Draw("BARP");

	half1->GetXaxis()->SetTitle("Channel");
	half1->GetXaxis()->CenterTitle(true);
	half1->GetYaxis()->SetTitle("Noise Rate (Hz)");
	half1->GetYaxis()->CenterTitle(true);
	half1->SetTitle("Modules 1-16");
	double width=half1->GetBinWidth(1);
	for(int aa=1;aa<16;aa++){ 
		//TLine *line=new TLine(aa*6*width, 0, aa*6*width, half1->GetMaximum()+2.5);
		TLine *line=new TLine(aa*6*width, 0, aa*6*width, c1->GetUymax());
		line->SetLineWidth(0.5); 
		line->SetLineStyle(7); 
		line->Draw();
	}

	// Draw 2nd half of TOF
	c1->cd(2);
	half2->SetFillColor(ci);
	half2->SetStats(0);
	half2->Draw("BARP");
	half2->GetXaxis()->SetTitle("Channel");
	half2->GetXaxis()->CenterTitle(true);
	half2->GetYaxis()->SetTitle("Noise Rate (Hz)");
	half2->GetYaxis()->CenterTitle(true);
	half2->SetTitle("Modules 17-32");
	double width=half2->GetBinWidth(1);
	for(int aa=1;aa<16;aa++){ TLine *line=new TLine(aa*6*width, 0, aa*6*width, c1->GetUymax()); line->SetLineWidth(0.5); line->SetLineStyle(7); line->Draw();}

	if(POSTSCRIPT_MULTI)   c1->Print(psFiles+"test."+fileType);



	//-------------------- Start Histo 2 ~ Correlations BW -------------------------//


	//-------------------- End   Histo 2 ~ Correlations BW -------------------------//
	//-------------------- Start Histo 3 ~ Correlations Color-----------------------//
	c1->Clear();
	gStyle->SetPalette(1);
	corrPlot->SetTitle(basicTitle);
	corrPlot->Draw("colz");
	corrPlot->SetStats(0);
	width=corrPlot->GetBinWidth(1);
	corrPlot->GetXaxis()->SetTitle("Channel Id");
	corrPlot->GetXaxis()->CenterTitle(true);
	corrPlot->GetYaxis()->SetTitle("Channel Id");
	corrPlot->GetYaxis()->CenterTitle(true);
	corrPlot->SetTitle("All Modules -- Correlation Plot");
	for(int aa=1;aa<32;aa++){ TLine *line=new TLine(aa*6*width, 0, aa*6*width,192*width ); line->SetLineWidth(2); line->SetLineStyle(1); line->Draw();}
	for(int aa=1;aa<32;aa++){ TLine *line=new TLine(0, aa*6*width,192*width, aa*6*width ); line->SetLineWidth(2); line->SetLineStyle(1); line->Draw();}
	c1->Update();
	if(POSTSCRIPT_MULTI)   c1->Print(psFiles+"test."+fileType);
	//-------------------- End   Histo 3 ~ Correlations Color-----------------------//

	//-------------------- Start Histo 4 ~ ToT's Total       -----------------------//

	c1->Clear();
	tots->SetTitle("All Modules -- Time Over Threshold");
	tots->Draw();
	c1->Update();
	if(POSTSCRIPT_MULTI)   c1->Print(psFiles+"test."+fileType);

	//-------------------- End Histo 4 ~ ToT's Total       -----------------------//


	//tots for first 16 mrps
	c1->Clear();
	c1->Divide(4,4);

	for(int aa=0;aa<32;aa++){
		//********************************
		int window=aa+1;
		if(aa>15){window-=16;}
		if(aa==16){ 
			c1->Update();
			if(POSTSCRIPT_MULTI)   c1->Print(psFiles+"test."+fileType);
		}

		//********************************
		TString fred="mod==";
		fred+=aa;
		TCut bob=fred;
		bob+="tot>0";
		c1->cd(window);
		totInfo->SetFillColor(41);
		totInfo->Draw("tot",bob);

		TPaveText *pt = new TPaveText(0.01,0.9317937,0.4801836,0.995,"blNDC");
		pt->SetName("title");
		pt->SetBorderSize(2);
		text = pt->AddText("tot {(mod==4)&&(tot>0)}");
		pt->Draw();
		TString bingo="Mod: ";
		bingo+=aa+1;
		tex = new TLatex(1000,10,bingo);
		tex->SetTextSize(0.1338616);
		tex->SetLineWidth(2);
		tex->Draw();

		c1->Update();
		//totInfo->SetStats(0);
	}
	c1->Update();
	if(POSTSCRIPT_MULTI)   c1->Print(psFiles+"test."+fileType);

	/*
	   c1->Clear();
	   c1->Divide(6,16);
	   c1->cd(1); tot_ch0->Draw();
	   c1->cd(2); tot_ch1->Draw();
	   c1->cd(3); tot_ch2->Draw();
	   c1->cd(4); tot_ch3->Draw();
	   c1->cd(5); tot_ch4->Draw();
	   c1->cd(6); tot_ch5->Draw();
	   c1->cd(7); tot_ch6->Draw();
	   c1->cd(8); tot_ch7->Draw();
	   c1->cd(9); tot_ch8->Draw();
	   c1->cd(10); tot_ch9->Draw();
	   c1->cd(11); tot_ch10->Draw();
	   c1->cd(12); tot_ch11->Draw();
	   c1->cd(13); tot_ch12->Draw();
	   c1->cd(14); tot_ch13->Draw();
	   c1->cd(15); tot_ch14->Draw();
	   c1->cd(16); tot_ch15->Draw();
	   c1->cd(17); tot_ch16->Draw();
	   c1->cd(18); tot_ch17->Draw();
	   c1->cd(19); tot_ch18->Draw();
	   c1->cd(20); tot_ch19->Draw();
	   c1->cd(21); tot_ch20->Draw();
	   c1->cd(22); tot_ch21->Draw();
	   c1->cd(23); tot_ch22->Draw();
	   c1->cd(24); tot_ch23->Draw();
	   c1->cd(25); tot_ch24->Draw();
	   c1->cd(26); tot_ch25->Draw();
	   c1->cd(27); tot_ch26->Draw();
	   c1->cd(28); tot_ch27->Draw();
	   c1->cd(29); tot_ch28->Draw();
	   c1->cd(30); tot_ch29->Draw();
	   c1->cd(31); tot_ch30->Draw();
	   c1->cd(32); tot_ch31->Draw();
	   c1->cd(33); tot_ch32->Draw();
	   c1->cd(34); tot_ch33->Draw();
	   c1->cd(35); tot_ch34->Draw();
	   c1->cd(36); tot_ch35->Draw();
	   c1->cd(37); tot_ch36->Draw();
	   c1->cd(38); tot_ch37->Draw();
	   c1->cd(39); tot_ch38->Draw();
	   c1->cd(40); tot_ch39->Draw();
	   c1->cd(41); tot_ch40->Draw();
	   c1->cd(42); tot_ch41->Draw();
	   c1->cd(43); tot_ch42->Draw();
	   c1->cd(44); tot_ch43->Draw();
	   c1->cd(45); tot_ch44->Draw();
	   c1->cd(46); tot_ch45->Draw();
	   c1->cd(47); tot_ch46->Draw();
	   c1->cd(48); tot_ch47->Draw();
	   c1->cd(49); tot_ch48->Draw();
	   c1->cd(50); tot_ch49->Draw();
	   c1->cd(51); tot_ch50->Draw();
	   c1->cd(52); tot_ch51->Draw();
	   c1->cd(53); tot_ch52->Draw();
	   c1->cd(54); tot_ch53->Draw();
	   c1->cd(55); tot_ch54->Draw();
	   c1->cd(56); tot_ch55->Draw();
	   c1->cd(57); tot_ch56->Draw();
	   c1->cd(58); tot_ch57->Draw();
	   c1->cd(59); tot_ch58->Draw();
	   c1->cd(60); tot_ch59->Draw();
	   c1->cd(61); tot_ch60->Draw();
	   c1->cd(62); tot_ch61->Draw();
	   c1->cd(63); tot_ch62->Draw();
	   c1->cd(64); tot_ch63->Draw();
	   c1->cd(65); tot_ch64->Draw();
	   c1->cd(66); tot_ch65->Draw();
	   c1->cd(67); tot_ch66->Draw();
	   c1->cd(68); tot_ch67->Draw();
	c1->cd(69); tot_ch68->Draw();
	c1->cd(70); tot_ch69->Draw();
	c1->cd(71); tot_ch70->Draw();
	c1->cd(72); tot_ch71->Draw();
	c1->cd(73); tot_ch72->Draw();
	c1->cd(74); tot_ch73->Draw();
	c1->cd(75); tot_ch74->Draw();
	c1->cd(76); tot_ch75->Draw();
	c1->cd(77); tot_ch76->Draw();
	c1->cd(78); tot_ch77->Draw();
	c1->cd(79); tot_ch78->Draw();
	c1->cd(80); tot_ch79->Draw();
	c1->cd(81); tot_ch80->Draw();
	c1->cd(82); tot_ch81->Draw();
	c1->cd(83); tot_ch82->Draw();
	c1->cd(84); tot_ch83->Draw();
	c1->cd(85); tot_ch84->Draw();
	c1->cd(86); tot_ch85->Draw();
	c1->cd(87); tot_ch86->Draw();
	c1->cd(88); tot_ch87->Draw();
	c1->cd(89); tot_ch88->Draw();
	c1->cd(90); tot_ch89->Draw();
	c1->cd(91); tot_ch90->Draw();
	c1->cd(92); tot_ch91->Draw();
	c1->cd(93); tot_ch92->Draw();
	c1->cd(94); tot_ch93->Draw();
	c1->cd(95); tot_ch94->Draw();
	c1->cd(96); tot_ch95->Draw();

	c1->Update();
	if(POSTSCRIPT_MULTI)   c1->Print(psFiles+"test."+fileType);

	c1->Clear();
	c1->Divide(6,16);
	c1->cd(1); tot_ch96->Draw();
	c1->cd(2); tot_ch97->Draw();
	c1->cd(3); tot_ch98->Draw();
	c1->cd(4); tot_ch99->Draw();
	c1->cd(5); tot_ch100->Draw();
	c1->cd(6); tot_ch101->Draw();
	c1->cd(7); tot_ch102->Draw();
	c1->cd(8); tot_ch103->Draw();
	c1->cd(9); tot_ch104->Draw();
	c1->cd(10); tot_ch105->Draw();
	c1->cd(11); tot_ch106->Draw();
	c1->cd(12); tot_ch107->Draw();
	c1->cd(13); tot_ch108->Draw();
	c1->cd(14); tot_ch109->Draw();
	c1->cd(15); tot_ch110->Draw();
	c1->cd(16); tot_ch111->Draw();
	c1->cd(17); tot_ch112->Draw();
	c1->cd(18); tot_ch113->Draw();
	c1->cd(19); tot_ch114->Draw();
	c1->cd(20); tot_ch115->Draw();
	c1->cd(21); tot_ch116->Draw();
	c1->cd(22); tot_ch117->Draw();
	c1->cd(23); tot_ch118->Draw();
	c1->cd(24); tot_ch119->Draw();
	c1->cd(25); tot_ch120->Draw();
	c1->cd(26); tot_ch121->Draw();
	c1->cd(27); tot_ch122->Draw();
	c1->cd(28); tot_ch123->Draw();
	c1->cd(29); tot_ch124->Draw();
	c1->cd(30); tot_ch125->Draw();
	c1->cd(31); tot_ch126->Draw();
	c1->cd(32); tot_ch127->Draw();
	c1->cd(33); tot_ch128->Draw();
	c1->cd(34); tot_ch129->Draw();
	c1->cd(35); tot_ch130->Draw();
	c1->cd(36); tot_ch131->Draw();
	c1->cd(37); tot_ch132->Draw();
	c1->cd(38); tot_ch133->Draw();
	c1->cd(39); tot_ch134->Draw();
	c1->cd(40); tot_ch135->Draw();
	c1->cd(41); tot_ch136->Draw();
	c1->cd(42); tot_ch137->Draw();
	c1->cd(43); tot_ch138->Draw();
	c1->cd(44); tot_ch139->Draw();
	c1->cd(45); tot_ch140->Draw();
	c1->cd(46); tot_ch141->Draw();
	c1->cd(47); tot_ch142->Draw();
	c1->cd(48); tot_ch143->Draw();
	c1->cd(49); tot_ch144->Draw();
	c1->cd(50); tot_ch145->Draw();
	c1->cd(51); tot_ch146->Draw();
	c1->cd(52); tot_ch147->Draw();
	c1->cd(53); tot_ch148->Draw();
	c1->cd(54); tot_ch149->Draw();
	c1->cd(55); tot_ch150->Draw();
	c1->cd(56); tot_ch151->Draw();
	c1->cd(57); tot_ch152->Draw();
	c1->cd(58); tot_ch153->Draw();
	c1->cd(59); tot_ch154->Draw();
	c1->cd(60); tot_ch155->Draw();
	c1->cd(61); tot_ch156->Draw();
	c1->cd(62); tot_ch157->Draw();
	c1->cd(63); tot_ch158->Draw();
	c1->cd(64); tot_ch159->Draw();
	c1->cd(65); tot_ch160->Draw();
	c1->cd(66); tot_ch161->Draw();
	c1->cd(67); tot_ch162->Draw();
	c1->cd(68); tot_ch163->Draw();
	c1->cd(69); tot_ch164->Draw();
	c1->cd(70); tot_ch165->Draw();
	c1->cd(71); tot_ch166->Draw();
	c1->cd(72); tot_ch167->Draw();
	c1->cd(73); tot_ch168->Draw();
	c1->cd(74); tot_ch169->Draw();
	c1->cd(75); tot_ch170->Draw();
	c1->cd(76); tot_ch171->Draw();
	c1->cd(77); tot_ch172->Draw();
	c1->cd(78); tot_ch173->Draw();
	c1->cd(79); tot_ch174->Draw();
	c1->cd(80); tot_ch175->Draw();
	c1->cd(81); tot_ch176->Draw();
	c1->cd(82); tot_ch177->Draw();
	c1->cd(83); tot_ch178->Draw();
	c1->cd(84); tot_ch179->Draw();
	c1->cd(85); tot_ch180->Draw();
	c1->cd(86); tot_ch181->Draw();
	c1->cd(87); tot_ch182->Draw();
	c1->cd(88); tot_ch183->Draw();
	c1->cd(89); tot_ch184->Draw();
	c1->cd(90); tot_ch185->Draw();
	c1->cd(91); tot_ch186->Draw();
	c1->cd(92); tot_ch187->Draw();
	c1->cd(93); tot_ch188->Draw();
	c1->cd(94); tot_ch189->Draw();
	c1->cd(95); tot_ch190->Draw();
	c1->cd(96); tot_ch191->Draw();

	c1->Update();
	if(POSTSCRIPT_MULTI)   c1->Print(psFiles+"test."+fileType);

	*/



		if(POSTSCRIPT_MULTI)   c1->Print(psFiles+"test."+fileType+"]");

	return;


}

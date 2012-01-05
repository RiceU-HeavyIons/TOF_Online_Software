// $Id$
// A script to look at noise rate differences
// Orignial :Kohei Kajimoto
// Poorly Modified: Drew Oldag


#include <iostream>
#include <fstream>

#include <TSystem.h>
#include <TSQLServer.h>
#include <TSQLRow.h>
#include <TSQLResult.h>


using namespace std;

const int level = 3; // logging level 0-7
const bool socket = true;

const int user_id = 102;
const int type_id = 1;

void NSDistMaker (char dname[] = ".") {

//    char server[] = "mysql://lone/toftstand";
	char server[]   = "mysql://localhost/TOF?socket=/var/lib/mysql/mysql.sock";
	char dbuser[]   = "tofp";
	char passwd[]   = "t4flight";

	TSQLServer *srv = TSQLServer::Connect(server, dbuser, passwd);

	if(level > 3)
		cout << srv->ServerInfo() << endl;
	
	const int bsize = 256;
	char line[bsize];
	int bins = 100;
	int xmax = 60;

	TH1F *hnr1 = new TH1F("nr_dist_ch_1", "noise rate distribution ch 1 UM", bins, 0, xmax);
	TH1F *hnr2 = new TH1F("nr_dist_ch_2", "noise rate distribution ch 2 UM", bins, 0, xmax);
	TH1F *hnr3 = new TH1F("nr_dist_ch_3", "noise rate distribution ch 3 UM", bins, 0, xmax);
        TH1F *hnr4 = new TH1F("nr_dist_ch_4", "noise rate distribution ch 4 UM", bins, 0, xmax);
	TH1F *hnr5 = new TH1F("nr_dist_ch_5", "noise rate distribution ch 5 UM", bins, 0, xmax);
        TH1F *hnr6 = new TH1F("nr_dist_ch_6", "noise rate distribution ch 6 UM", bins, 0, xmax);

	TH1F *hnr7 = new TH1F("nr_dist_ch_1", "noise rate distribution ch 1 TM", bins, 0, xmax);
        TH1F *hnr8 = new TH1F("nr_dist_ch_2", "noise rate distribution ch 2 TM", bins, 0, xmax);
        TH1F *hnr9 = new TH1F("nr_dist_ch_3", "noise rate distribution ch 3 TM", bins, 0, xmax);
        TH1F *hnr10= new TH1F("nr_dist_ch_4", "noise rate distribution ch 4 TM", bins, 0, xmax);
        TH1F *hnr11= new TH1F("nr_dist_ch_5", "noise rate distribution ch 5 TM", bins, 0, xmax);
        TH1F *hnr12= new TH1F("nr_dist_ch_6", "noise rate distribution ch 6 TM", bins, 0, xmax);


	TH1F *hnr = new TH1F("nr_dist_total_UM", "average noise rate distribution UM", bins, 0, xmax);
	TH1F *htr = new TH1F("nr_dist_total_TM", "average noise rate distribution TM", bins, 0, xmax);
	TH1F *nrcomb = new TH1F("nr_combined", "noise rate distribution TM and UM", bins, 0, xmax);

	TH2D *htr2d = new TH2D("tr_corr", "Timing Resolutions", 80, 40., 220., 80, 40., 220.0);
	TH2D *hnr2d = new TH2D("nr_corr", "Noise Rates", 50, -5, 80, 50, -5, 50);
	
	htr2d->GetXaxis()->SetTitle("UT");
	htr2d->GetYaxis()->SetTitle("USTC / Tsinghua Univ");

	hnr2d->GetXaxis()->SetTitle("UT");
	hnr2d->GetYaxis()->SetTitle("USTC / Tsinghua Univ");

	char chk_tr[500];	// need to increase size some day
	char chk_nr[500];	// same
	for(int i=0; i < 500; i++) {
		chk_tr[i] = 0;
		chk_nr[i] = 0;
	}
	
	char stm[256];
	sprintf(stm,
		"SELECT m.module_id, m.run_id, m.slot, m.pad1, m.pad2,m.pad3, m.pad4, m.pad5, m.pad6, i.vendor_id from module_results m inner join modules i on m.module_id=i.id order by m.id desc");
	TSQLResult *rt = srv->Query(stm);
	TSQLRow *row;

	int num = 0;
	bool slot_cut = false;
	bool tr_cut = true;
	bool nr_cut = true;
	int timing = 0;

	
	while( (row = rt->Next()) != NULL) {
		num++;
		int mid = atoi(row->GetField(0));
		int rid = atoi(row->GetField(1));
		int slt = atoi(row->GetField(2));
		int vid = atoi(row->GetField(9));
		int off = 3;
		//		if(slot_cut && (slt == 1 || slt == 4)) continue;  I don't know what this does....
		sprintf(stm, "SELECT run_type_id from runs where id=%d", rid);
		int tid = atoi(srv->Query(stm)->Next()->GetField(0));
		TSQLRow *cron;
		switch(tid) {
			case 2:
			  //if(tr_cut && chk_tr[mid] > 0) break;
			  //	for(int i=0; i < 6; i++) htr->Fill(atof(row->GetField(i+off)));
			  //	sprintf(stm, "SELECT pad1, pad2, pad3, pad4, pad5, pad6 from chinese_results"
			  //		" where module_id=%d and result_type=1", mid);
			  //	if(crow = srv->Query(stm)->Next()) {
			  //		for(int i=0; i < 6; i++)
			  //			htr2d->Fill(atof(row->GetField(i+off)), atof(crow->GetField(i)));
			  //		chk_tr[mid]++;
			  //	}
			  timing=1;
			  break;
			case 3:	
			  //			  if(timing != 1) break;
			  if(nr_cut && chk_nr[mid] > 0) break;
				if(vid == 1){
  				for(int i=0; i < 6; i++) hnr->Fill(atof(row->GetField(i+off)));
				hnr1->Fill(atof(row->GetField(3)));
                                hnr2->Fill(atof(row->GetField(4)));
                                hnr3->Fill(atof(row->GetField(5)));
                                hnr4->Fill(atof(row->GetField(6)));
                                hnr5->Fill(atof(row->GetField(7)));
                                hnr6->Fill(atof(row->GetField(8)));}
				else {for(int i=0; i < 6; i++) htr->Fill(atof(row->GetField(i+off)));
				hnr7->Fill(atof(row->GetField(3)));
				hnr8->Fill(atof(row->GetField(4)));
				hnr9->Fill(atof(row->GetField(5)));
				hnr10->Fill(atof(row->GetField(6)));
				hnr11->Fill(atof(row->GetField(7)));
				hnr12->Fill(atof(row->GetField(8)));}
				sprintf(stm, "SELECT pad1, pad2, pad3, pad4, pad5, pad6 from chinese_results"
					" where module_id=%d and result_type=2", mid);
				if(crow = srv->Query(stm)->Next()) {
					for(int i=0; i < 6; i++)
						hnr2d->Fill(atof(row->GetField(i+off)), atof(crow->GetField(i)));
					chk_nr[mid]++;
				}
				timing=0;
				break;
		}
	}
	TDatime *td = new TDatime();
	TCanvas *c = new TCanvas("c1", "c1", 0, 0, 1200+4, 2400+28);

	gStyle->SetPalette(1);
	gStyle->SetOptFit(1);
	gStyle->SetOptStat(1);
	//c->Divide(3, 6, 0.01, 0.03);
	c->Divide(3, 5, 0.01, 0.02);
	int i=1;

	
        TF1 *pois= new TF1("pois", "x*x*[0]*TMath::Poisson(x,[1])",0,60);

	pois->SetParameter(1, 1);
	pois->SetParameter(0, 1);

        c->cd(i++);
        hnr->Scale(1/6.0);
	//        hnr->Fit(pois);
        hnr->Draw();

        i++;

        c->cd(i++);
        htr->Scale(1/6.0);
        //htr->Fit(pois);
        htr->Draw();

        c->cd(2);
        nrcomb->Add(hnr, htr, 1,1);
        nrcomb->Scale(1/2.0);
        nrcomb->Draw(); //Combination of the two NR histos.  

	c->cd(i++);
	hnr1->Draw();
	c->Update();
	
	c->cd(i++);
	hnr2->Draw();
	
	c->cd(i++);
	hnr3->Draw();
	
	c->cd(i++);
	hnr4->Draw();
	
	c->cd(i++);
	hnr5->Draw();

	c->cd(i++);
	hnr6->Draw();
		
	c->cd(i++);
	hnr7->Draw();
	c->Update();
	
	c->cd(i++);
	hnr8->Draw();
	
	c->cd(i++);
	hnr9->Draw();
	
	c->cd(i++);
	hnr10->Draw();
	
	c->cd(i++);
	hnr11->Draw();
	
	c->cd(i++);
	hnr12->Draw();
	


	c->cd(0);




	TDatime *td = new TDatime();
	TPaveText *pt = new TPaveText(0.85, 0.98, 0.99, 1.00);
	pt->SetMargin(0.08);
	char str[255];
	sprintf(str, "Updated: %s", td->AsSQLString());
	pt->AddText(str);
	pt->Draw();
	
	c->Print(TString(dname) + "/" + "noise_diff.gif");

	//       	ChineseTimingPlot(srv);
	
	srv->Close();
}

void ChineseTimingPlot(TSQLServer *srv)
{
	TH1D *h[3];
	int nbin = 80;
	int xmin = 40;
	int xmax = 220;
	h[0] = new TH1D("total",    "total",    nbin, xmin, xmax);
	h[1] = new TH1D("ustc",     "ustc",     nbin, xmin, xmax);
	h[2] = new TH1D("tsinghua", "tsinghua", nbin, xmin, xmax);
	TCanvas *c = new TCanvas("c2", "c2", 600+4, 600+28);
	char stm[256];
	for(int i=1; i < 3; i++) {
		sprintf(stm, "SELECT pad1, pad2, pad3, pad4, pad5, pad6"
			" from chinese_results where result_type=1 and vendor_id=%d", i);
		TSQLResult *rt = srv->Query(stm);
		TSQLRow *row;
		while(row = rt->Next())
		{
			for(int j=0; j < 6; j++) {
				h[0]->Fill( atof(row->GetField(j)) );
				h[i]->Fill( atof(row->GetField(j)) );
			}
		}
	}
	h[1]->SetFillColor(2);
	h[2]->SetFillColor(3);
	THStack *hs = new THStack("hs", "hs");
	for(int i=1; i < 3; i++) hs->Add(h[i]);
	hs->Draw();
	//	c->Print("ch.gif");
		
}

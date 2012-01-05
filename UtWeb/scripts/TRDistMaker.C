// $Id$
//
// Author: Kohei Kajimoto 2007
// Copyright (C) 2007, Kohei Kajimoto. All rights reserved.

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

void TRDistMaker (char dname[] = ".") {

//    char server[] = "mysql://lone/toftstand";
	char server[]   = "mysql://localhost/TOF?socket=/var/lib/mysql/mysql.sock";
	char dbuser[]   = "tofp";
	char passwd[]   = "t4flight";

	TSQLServer *srv = TSQLServer::Connect(server, dbuser, passwd);

	if(level > 3)
		cout << srv->ServerInfo() << endl;
	
	const int bsize = 256;
	char line[bsize];

	
	TH1F *htr = new TH1F("tr_dist", "timing resolution distribution", 50, 40, 140);
	TH1F *hnr = new TH1F("nr_dist", "noise rate distribution", 50, 0, 100);
	TH2D *htr2 = new TH2D("tr_corr", "Timing Resolutions", 80, 40., 220., 80, 40., 220.0);
	TH2D *hnr2 = new TH2D("nr_corr", "Noise Rates", 50, -5, 80, 50, -5, 50);
	
	htr2->GetXaxis()->SetTitle("UT");
	htr2->GetYaxis()->SetTitle("USTC / Tsinghua Univ");

	hnr2->GetXaxis()->SetTitle("UT");
	hnr2->GetYaxis()->SetTitle("USTC / Tsinghua Univ");
	
	const int nentries = 10000;
	char chk_tr[nentries];	// need to increase size some day
	char chk_nr[nentries];	// same
	for(int i=0; i < nentries; i++) {
		chk_tr[i] = 0;
		chk_nr[i] = 0;
	}
	
	char stm[256];
	sprintf(stm,
		"SELECT module_id, run_id, slot, "
		"pad1, pad2, pad3, pad4, pad5, pad6 from module_results "
		"order by id desc"
	);	// hopely best result comes last
	TSQLResult *rt = srv->Query(stm);
	TSQLRow *row;
	int num = 0;
	bool slot_cut = false;
	bool tr_cut = true;
	bool nr_cut = true;
	
	while( (row = rt->Next()) != NULL) {
		num++;
		int mid = atoi(row->GetField(0));
		int rid = atoi(row->GetField(1));
		int slt = atoi(row->GetField(2));
		int off = 3;
		if(slot_cut && (slt == 1 || slt == 4)) continue;
		sprintf(stm, "SELECT run_type_id from runs where id=%d", rid);
		int tid = atoi(srv->Query(stm)->Next()->GetField(0));
		TSQLRow *cron;
		switch(tid) {
			case 2:
				if(tr_cut && chk_tr[mid] > 0) break;
				for(int i=0; i < 6; i++) htr->Fill(atof(row->GetField(i+off)));
				sprintf(stm, "SELECT pad1, pad2, pad3, pad4, pad5, pad6 from chinese_results"
					" where module_id=%d and result_type=1", mid);
				if(crow = srv->Query(stm)->Next()) {
					for(int i=0; i < 6; i++)
						htr2->Fill(atof(row->GetField(i+off)), atof(crow->GetField(i)));
					chk_tr[mid]++;
				}
				break;
			case 3:	
				if(nr_cut && chk_nr[mid] > 0) break;
				for(int i=0; i < 6; i++) hnr->Fill(atof(row->GetField(i+off)));
				sprintf(stm, "SELECT pad1, pad2, pad3, pad4, pad5, pad6 from chinese_results"
					" where module_id=%d and result_type=2", mid);
				if(crow = srv->Query(stm)->Next()) {
					for(int i=0; i < 6; i++)
						hnr2->Fill(atof(row->GetField(i+off)), atof(crow->GetField(i)));
					chk_nr[mid]++;
				}
				break;
		}
	}
	TDatime *td = new TDatime();
	TCanvas *c = new TCanvas("c1", "c1", 0, 0, 1200+4, 800+28);

	gStyle->SetPalette(1);
	
	c->Divide(3, 2, 0.01, 0.03);
//	c->cd(1); htr->Draw();
	int i=1;
	c->cd(i++);
	htr2->Draw("colz");
	c->Update();
                                                                                                            
	c->cd(i++);
	TH1D *hpx = htr2->ProjectionX();
	hpx->Draw();

	c->cd(i++);
	TH1D *hpy = htr2->ProjectionY();
//	hpy->SetLineColor(4);
//	hpy->Draw("same");
	hpy->Draw();

	c->cd(i++);
	hnr2->Draw("colz");

	c->cd(i++);
	TH1D *hnx = hnr2->ProjectionX();
	hnx->Draw();

	c->cd(i++);
	TH1D *hny = hnr2->ProjectionY();
	hny->Draw();

	c->cd(0);

	TDatime *td = new TDatime();
	TPaveText *pt = new TPaveText(0.85, 0.98, 0.99, 1.00);
	pt->SetMargin(0.08);
	char str[255];
	sprintf(str, "Updated: %s", td->AsSQLString());
	pt->AddText(str);
	pt->Draw();
	
	c->Print(TString(dname) + "/" + "tr_dist.gif");

	ChineseTimingPlot(srv);
	
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

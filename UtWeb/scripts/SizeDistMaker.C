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

void SizeDistMaker () {

//    char server[] = "mysql://lone/toftstand";
    char server[]   = "mysql://localhost/TOF";
    char dbuser[]   = "tofp";
    char passwd[]   = "t4flight";

	if(socket)
    	gSystem->Exec("ln -s /var/lib/mysql/mysql.sock /tmp/mysql.sock");

    TSQLServer *srv = TSQLServer::Connect(server, dbuser, passwd);

	if(level > 2) {
		cout << "Now connecting to server...." << endl;	
    	printf("Host: %s\n", srv->GetHost());
    	if(!socket)
    		printf("Port: %d\n", srv->GetPort());
    	else
    		printf("Port: socket\n");
 	  	printf("Info: %s\n", srv->ServerInfo());
	}

	if(level > 2) {	
  		cout << "Now  Loading Datafile..." << endl;
	}
	
	const int bsize = 256;
	char line[bsize];
 	
	
	TH1F *hl = new TH1F("length", "length (mm)", 20, 211, 213);
	TH1F *hw = new TH1F("width", "width (mm)", 20, 93.2, 94.8);
	TH1F *ht = new TH1F("thickness", "thickness (mm)", 20, 16.7, 19.2);

	char stm[256];
	sprintf(stm, "SELECT "
	"module_id, length1, length2, width1, width2, thickness1, thickness2 from module_sizes");
	TSQLResult *rt = srv->Query(stm);
	TSQLRow *row;
	int num = 0;
	while( (row = rt->Next()) != NULL) {
		num++;
		hl->Fill(atof(row->GetField(1)));
		hl->Fill(atof(row->GetField(2)));
		hw->Fill(atof(row->GetField(3)));
		hw->Fill(atof(row->GetField(4)));
		ht->Fill(atof(row->GetField(5)));
		ht->Fill(atof(row->GetField(6)));
	}
	TDatime *td = new TDatime();
	TCanvas *c = new TCanvas();
	c->Divide(2,2);
	gStyle->SetOptStat(1111);
	gStyle->SetOptFit(111);
	
	c->cd(2);
	hl->Draw();
	hl->Fit("gaus");
	double l_mean = hl->GetFunction("gaus")->GetParameter(1);
	double l_sigma = hl->GetFunction("gaus")->GetParameter(2);

	c->cd(3);
	hw->Draw();
	hw->Fit("gaus");
	double w_mean = hw->GetFunction("gaus")->GetParameter(1);
	double w_sigma = hw->GetFunction("gaus")->GetParameter(2);
	
	c->cd(4);
	ht->Draw();
	ht->Fit("gaus");
	double t_mean = ht->GetFunction("gaus")->GetParameter(1);
	double t_sigma = ht->GetFunction("gaus")->GetParameter(2);
	
	c->cd(1);
	TPaveText *pt = new TPaveText(0.1, 0.9, 0.9, 0.1);
	pt->SetMargin(0.08);
	char str[255];
	sprintf(str, "Updated: %s", td->AsSQLString());
	pt->AddText(str);
	sprintf(str, "Number of Modules: %d", num);
	pt->AddText(str);
	sprintf(str, "Length (mean, sigma): %6.2f mm, %5.3f mm", l_mean, l_sigma);
	pt->AddText(str);
	sprintf(str, "Width (mean, sigma): %6.2f mm, %5.3f mm", w_mean, w_sigma);
	pt->AddText(str);
	sprintf(str, "Thickness (mean, sigma): %6.2f mm, %5.3f mm", t_mean, t_sigma);
	pt->AddText(str);
		
	sprintf(str, "");
	pt->AddText(str);
	pt->Draw();

	c->cd(0);
	c->Print("dim_dist.gif");

 	if(level > 2)
 		cout << "Done" << endl;

	srv->Close();
	if(socket)
		gSystem->Exec("rm /tmp/mysql.sock");
}

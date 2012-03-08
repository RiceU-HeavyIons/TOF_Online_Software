#include <string>


int ViewHisto(string inFile="test.root"){

TFile* f=new TFile(inFile.c_str());
noise->Draw();

string tray=inFile;

int loc1=tray.find_first_of("_");
tray.insert(loc1+5," ");
int loc2=tray.find_first_of("_",loc1+1);
string title=tray.substr(loc1+1,loc2-loc1-1);

title+=" measurement# ";
title+=tray.substr(loc2+1,1);

noise->SetTitle(title.c_str());


return 0;

}




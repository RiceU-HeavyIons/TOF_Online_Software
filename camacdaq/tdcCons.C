//int hcons()
{
  // Histogram consumer script. Create a canvas divided into 6 pads. Connect
  // to memory mapped file "hsimple.map", that was created by daqloop.
  // It reads the histograms from shared memory and displays them
  // in the pads (sleeping for 0.1 seconds before starting a new read-out
  // cycle). This script runs in an infinite loop, so use ctrl-c to stop it.
  
  gROOT->Reset();
  gStyle->SetOptStat(111111);
  
  // Create a new canvas and 3 pads
  TCanvas *c1 = new TCanvas("c1","Shared Memory Consumer",200,10,700,780);

  //c1->Divide(2,3);
  c1->Divide(2,2);
  
  // Open the memory mapped file "hsimple.map" in "READ" (default) mode.
  mfile = TMapFile::Create("hsimple.map");
  
  // Print status of mapped file and list its contents
  mfile->Print();
  mfile->ls();
  
  // Create pointers to the objects in shared memory.
  TH1F *htdc0 = 0;
  TH1F *htdc1 = 0;
  TH1F *htdc2 = 0;
  TH1F *htdc3 = 0;
  //TH1F *htdc4 = 0;
  //TH1F *htdc5 = 0;
  
  // Loop displaying the histograms. Once the producer stops this
  // script will break out of the loop.
  Double_t oldentries = 0;
  while (1) {
    char tempstr[10];
    htdc0    = (TH1F *) mfile->Get("htdc0", htdc0);
    //if (htdc0->GetEntries() == oldentries) break;
    //oldentries = htdc0->GetEntries();
    c1->cd(1);
    htdc0->Draw();

    htdc1    = (TH1F *) mfile->Get("htdc1", htdc1);
    c1->cd(2);
    htdc1->Draw();
    
    htdc2    = (TH1F *) mfile->Get("htdc2", htdc2);
    c1->cd(3);
    htdc2->Draw();

    htdc3    = (TH1F *) mfile->Get("htdc3", htdc3);
    c1->cd(4);
    htdc3->Draw();

    //htdc4    = (TH1F *) mfile->Get("htdc4", htdc4);
    //c1->cd(5);
    //htdc4->Draw();

    //htdc5    = (TH1F *) mfile->Get("htdc5", htdc5);
    //c1->cd(6);
    //htdc5->Draw();

    c1->Modified();
    c1->Update();
    gSystem->Sleep(1000);   // sleep for 0.1 seconds
    if (gSystem->ProcessEvents())
      break;
  }
}

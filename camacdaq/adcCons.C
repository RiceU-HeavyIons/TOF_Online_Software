//int hcons()
{
  // Histogram consumer script. Create a canvas divided into 6 pads. Connect
  // to memory mapped file "hsimple.map", that was created by daqloop.
  // It reads the histograms from shared memory and displays them
  // in the pads (sleeping for 0.1 seconds before starting a new read-out
  // cycle). This script runs in an infinite loop, so use ctrl-c to stop it.
  
  gROOT->Reset();
  
  // Create a new canvas and 3 pads
  TCanvas *c1 = new TCanvas("c1","Shared Memory Consumer",200,10,700,780);

  c1->Divide(2,3);
  
  // Open the memory mapped file "hsimple.map" in "READ" (default) mode.
  mfile = TMapFile::Create("hsimple.map");
  
  // Print status of mapped file and list its contents
  mfile->Print();
  mfile->ls();
  
  // Create pointers to the objects in shared memory.
  TH1F *hadc0 = 0;
  TH1F *hadc1 = 0;
  TH1F *hadc2 = 0;
  TH1F *hadc3 = 0;
  TH1F *hadc4 = 0;
  TH1F *hadc5 = 0;
  
  // Loop displaying the histograms. Once the producer stops this
  // script will break out of the loop.
  Double_t oldentries = 0;
  while (1) {
    char tempstr[10];
    hadc0    = (TH1F *) mfile->Get("hadc0", hadc0);
    if (hadc0->GetEntries() == oldentries) break;
    oldentries = hadc0->GetEntries();
    c1->cd(1);
    hadc0->Draw();

    hadc1    = (TH1F *) mfile->Get("hadc1", hadc1);
    c1->cd(2);
    hadc1->Draw();
    
    hadc2    = (TH1F *) mfile->Get("hadc2", hadc2);
    c1->cd(3);
    hadc2->Draw();

    hadc3    = (TH1F *) mfile->Get("hadc3", hadc3);
    c1->cd(4);
    hadc3->Draw();

    hadc4    = (TH1F *) mfile->Get("hadc4", hadc4);
    c1->cd(5);
    hadc4->Draw();

    hadc5    = (TH1F *) mfile->Get("hadc5", hadc5);
    c1->cd(6);
    hadc5->Draw();

    c1->Modified();
    c1->Update();
    gSystem->Sleep(1000);   // sleep for 0.1 seconds
    if (gSystem->ProcessEvents())
      break;
  }
}

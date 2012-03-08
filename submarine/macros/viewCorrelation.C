#include <TString>
#include <unistd>

int viewCorrelation(TString inFile="/home/tof/submarine/quick.root"){

	cout<<inFile<<endl;
	TFile T(inFile);
	corrPlot->Draw("colz");
	gStyle->SetPalette(1);
	corrPlot->SetStats(0);
	c1->Update();
	std::sleep(5);
	c1->SaveAs("corrPlot.gif");
	

	return 1;
}

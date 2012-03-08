#include <TString>
#include <unistd>

int viewCorrelation(TString inFile="/data2/dylan/submarine_data/noise_set202_22.dat.root"){

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

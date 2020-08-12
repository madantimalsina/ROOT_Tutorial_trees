{
  TFile *MCIn = new TFile("../Stage_2/output_MC.root", "READ");
  TTreeReader myReader("CESTree", MCIn);
  TTreeReaderValue<Double_t> E(myReader, "E");
  TTreeReaderValue<Double_t> Ne(myReader, "Ne");
  TFile *fIn = new TFile("energyFit.root");
  TF1* fdata = (TF1*)fIn->Get("fGauss_data");
  TF1* fMC = (TF1*)fIn->Get("fGauss_MC");
  fIn->Close();
  TFile *fOut = new TFile("weightedMC.root", "RECREATE");
  TH1D* hE = new TH1D("hE", ";E [keV];", 40, 0., 20.);
  TH1D* hNe = new TH1D("hNe", ";Extracted electrons;", 101, -0.5, 100.5);

  while (myReader.Next())
  {
    double weight = fdata->Eval(*E)/fMC->Eval(*E);
    hE->Fill(*E, weight);
    hNe->Fill(*Ne, weight);
  }
  hE->Write();
  hNe->Write();
  fOut->Close();
}

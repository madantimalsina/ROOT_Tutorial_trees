{
  TTreeReader myReader("CESTree", _file0);
  TTreeReaderValue<Double_t> E(myReader, "E");
  //TTreeReaderValue<Double_t> Ne(myReader, "Ne");

  TFile *fOut = new TFile("output.root", "RECREATE");
  TH1D* hE = new TH1D("hE", ";E [keV];", 40, 0., 20.);

  while (myReader.Next())
  {
    hE->Fill(*E);
  }
  hE->Write();
  fOut->Close();
}

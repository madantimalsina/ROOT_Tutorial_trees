{
  TFile *fOut = new TFile("energyFit.root", "RECREATE"); //to save the fit to data
  TF1* fGauss = new TF1("fGauss", "gaus", 0, 20);
  TH1D* hE_data = (TH1D*)_file0->Get("hE");
  hE_data->SetName("hE_data");
  TH1D* hE_MC = (TH1D*)_file1->Get("hE");
  hE_MC->SetName("hE_MC");

  gStyle->SetOptFit(true);
  TCanvas *c = new TCanvas("c", "", 1600, 600);
  c->Divide(2, 1); //two columns
  c->cd(1);
  hE_data->Fit(fGauss, "0R"); //0 means "do not draw", R means "respect TF1 range"
  fGauss->SetName("fGauss_data");
  fGauss->Write();
  fGauss->SetName("fGauss");
  double mu_data = fGauss->GetParameter(1);
  double sigma_data = fGauss->GetParameter(2);

  fGauss->SetRange(mu_data - 1.5*sigma_data, mu_data + 1.5*sigma_data);
  hE_data->Fit(fGauss, "0R"); //You could refit in a smaller range around the mean based on the previous fit results...

  //Or you can base the starting values of the parameters on the histogram:
  fGauss->SetRange(0, 20);
  fGauss->SetParameters(1., hE_data->GetMean(), hE_data->GetRMS());
  hE_data->Fit(fGauss, "+R");
  mu_data = fGauss->GetParameter(1);
  sigma_data = fGauss->GetParameter(2);
  c->cd(2);

  fGauss->SetParameters(1., hE_MC->GetMean(), hE_MC->GetRMS());

  hE_MC->Fit(fGauss, "+R");
  fGauss->SetName("fGauss_MC");
  fGauss->Write();
  fOut->Close();
  fGauss->SetName("fGauss");

  double mu_MC = fGauss->GetParameter(1);
  double sigma_MC = fGauss->GetParameter(2); //access uncertainties with GetParError(2), ...

  //These look a bit different... (in fact we generated them from different distributions!)
  std::cout << std::endl;
  std::cout << setw(10) << "" << setw(10) << "Mean" << setw(10) << "Sigma" << std::endl;
  std::cout << setw(10) << "Data" << setw(10) << mu_data << setw(10) << sigma_data << std::endl;
  std::cout << setw(10) << "MC" << setw(10) << mu_MC << setw(10) << sigma_MC << std::endl;


  //What are the chances that the (MC) distribution is able to generate a data sample with a mean as high as what we measured? Let's calculate a "p-value" by hand...

  TH1D* hE = (TH1D*)hE_data->Clone();
  hE->SetName("hE");
  hE->Reset(); //empty histogram...

  TH1D* hMeans = new TH1D("hMeans", ";Mean of Gaussian;", 100, 7., 8.);

  long int nsamples = 1E4;
  int ndata = hE_data->GetEntries();
  for(long int i = 0; i < nsamples; ++i)
  {
    hE->Reset();
    hE->FillRandom(hE_MC, ndata);
    fGauss->SetParameters(1., 7.5, 2.5);
    hE->Fit(fGauss, "0RQ"); //Q = quiet mode
    hMeans->Fill(fGauss->GetParameter(1));
  }
  TCanvas *cMeans = new TCanvas("cMeans", "", 800, 600);
  hMeans->Draw("histo");

  TLine *ldata = new TLine(mu_data, 0, mu_data, 100);
  ldata->SetLineColor(kRed);
  ldata->SetLineStyle(kDashed);
  ldata->Draw();

  //Let's make some "nice" shaded area ... (not sure if there's a better way)
  TH1D* hLargeMeans = (TH1D*)hMeans->Clone();
  hLargeMeans->SetFillColor(kRed);
  for(int i = 0; i < hLargeMeans->FindBin(mu_data); ++i)
    hLargeMeans->SetBinContent(i, 0);

  hLargeMeans->Draw("samehisto");

  stringstream ss;
  ss << "p-val = " << hLargeMeans->Integral()/hMeans->Integral();
  TText* t_pval = new TText(7.72, 100., ss.str().c_str());
  t_pval->Draw(); //TText x, y position refers to actual values of the x- and y-axes

  double qqpvalueqq = hMeans->Integral(hMeans->FindBin(mu_data), hMeans->GetNbinsX())/hMeans->Integral();
  std::cout << "\"p-value\" = " << qqpvalueqq << std::endl;

  //We should probably regenerate the MC with the parameters that we have fit from the data... (or maybe re-scale the MC to match the data)
}

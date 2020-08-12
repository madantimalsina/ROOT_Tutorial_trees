{
  TFile *fIn = new TFile("../Stage_2/output_MC.root", "READ");
  TTree* tree = (TTree*)fIn->Get("CESTree");
  TFile *fOut = new TFile("fitOutput.root", "RECREATE");
  TH2D* hENe = new TH2D("hENe", ";E [keV];Extracted electrons", 400., 0., 20., 101, -0.5, 100.5);
  
  TCanvas *c = new TCanvas("c", "", 800, 600);
  tree->Draw("Ne:E>>hENe", "", "colz"); //yes, Ne comes first even though it's the Y axis...
  hENe->Write();

  //Copy the formula relating Ne and E from CEST:
  //(1./W)*(alpha + beta*E + gamma*E*E);

  TF1* fQy = new TF1("fQy", "x/[0]*([1] + [2]*x + [3]*x^2)", 0, 20);

  fQy->SetParameters(13.6, 0.9, 7.1, -0.3); //Draw the true curve first
  fQy->SetLineColor(kGreen+3);
  fQy->SetLineStyle(kDashed);
  fQy->DrawClone("same");
  fQy->SetName("fQy_true");
  fQy->Write();
  fQy->SetName("fQy");
  fQy->SetLineColor(kRed);
  fQy->SetLineStyle(kSolid);
  
  fQy->SetParNames("W", "alpha", "beta", "gamma");
  fQy->SetParameters(13.6, 0.85, 6.8, -0.2); //Setting some reasonable initial parameter values
  fQy->FixParameter(0, 13.6); //We know this value already
  fQy->SetParLimits(1, 0.1, 1.5);
  fQy->SetParLimits(3, -0.4, -0.15); //We have some information about this parameter
  
  hENe->Fit(fQy, "+RB"); //B = "respect parameter bounds"
  fQy->Write();
  //fQy->Draw("same");


  //Let's also fit the slices individually and using a custom function:

  TF1* fNe = new TF1("fNe", "[0]*TMath::Poisson(x, [1])", -0.5, 100.5);
  fNe->SetNpx(101);
  TGraphErrors *gNe = new TGraphErrors(); //we could initialise with the number of points, but it won't matter much for just a few hundred points...
  gNe->SetName("gNe");
  //First select the X-bin (energy) for our slice along Y (Ne):

  int skipped_slices = 0; //keep count of slices with zero entries that don't make it into the graph
  for(int ix = 1; ix <= hENe->GetXaxis()->GetNbins(); ++ix) //bin 0 is underflow
  {
    //Now "project" onto Y, but only this bin, effectively just selecting a slice:
    TH1D* hproj = hENe->ProjectionY("hproj", ix, ix);
    // hproj->Draw();
    if(hproj->Integral() <= 1E-10)
    {
      skipped_slices += 1;
      continue;
    }
    fNe->SetParameters(hproj->Integral(), hproj->GetMean());
    hproj->Fit(fNe, "0R");
    gNe->SetPoint(ix - 1 - skipped_slices, hENe->GetXaxis()->GetBinCenter(ix), fNe->GetParameter(1));
    gNe->SetPointError(ix - 1 - skipped_slices, 0, fNe->GetParError(1));
    hproj->Delete();
  }

  gNe->Fit(fQy, "0R");
  fQy->SetName("fQy_graph");
  fQy->Write();
  fQy->SetName("fQy");

  // c->WaitPrimitive(); //the histogram disappears when the file is closed...
  gNe->Write();
  c->Close();
  fOut->Close();
}

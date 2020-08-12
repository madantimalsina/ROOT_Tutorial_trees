{
  //We know that about ~20% of the times, a PMT can respond to a single photon with the emission of two photoelectrons. Let's model the response of the PMT with a TF1, assuming that the single photoelectron area is gaussian distributed and neglecting the area "lost" to the left of zero (where we set the gaussian = 0):

  //The gaussian function gausn has three parameters (norm, mu, sigma)
  //The SPE distribution is just a gaussian with those parameters
  //The DPE part of the distribution is a gaussian with 2*mu and sqrt(2)*sigma, and ~0.2*norm

  //So I use gausn(0) to specify that the built-in gaussian function should start to use parameters from 0, [0], [1], [2], and then explicitely write down a normalised gaussian with [3]*[0], 2*[1] and sqrt(2)*[2] as norm, mu and sigma
  TF1* fDPE = new TF1("fDPE", "gausn(0) + [0]*[3]/(sqrt(2*TMath::Pi())*sqrt(2)*[1])*exp(-(x-2*[1])^2/(4*[2]^2))", 0, 5);
 
  fDPE->SetParameters(1., 1., 0.38, 0.23);

  TCanvas *c = new TCanvas("c", "", 800, 600);
  c->SetLogy(); //Easier to see the second gaussian
  fDPE->Draw();

  /*
    You may ask yourself: why don't we lower the LZ S1 threshold from 3-fold to 2-fold? Is that possible?
    The answer is usually that the probability of coincidence of two dark counts is too high
    Can we exploit the DPE effect to get rid of these dark count coincidences? Maybe.

    Dark counts have a zero DPE fration, so we can use the area of the individual photons to try to distinguish between dark count coincidences and genuine S1s
    Let's randomly generate S1s composed of 2 photons, and let's pick the photon with the highest area. Then let's do the same from dark count coincidences and compre the distributions
  */

  TH1D* hMaxPhotonArea_S1 = new TH1D("hMaxPhotonArea_S1", ";Area [phe];", 500, 0, 5);
  hMaxPhotonArea_S1->SetLineColor(kBlack);
  TH1D* hMaxPhotonArea_DCs = new TH1D("hMaxPhotonArea_DCs", ";Area [phe];", 500, 0, 5);
  hMaxPhotonArea_DCs->SetLineColor(kRed);


  //"Real" S1 first: DPE fraction = 23%
  fDPE->SetParameters(1., 1., 0.38, 0.23);

  long int nsamples = 1E5;

  double a1, a2, A;

  for(long int i = 0; i < nsamples; ++i)
  {
    a1 = fDPE->GetRandom();
    a2 = fDPE->GetRandom();
    if(a1 >= a2)
      A = a1;
    else
      A = a2;
    hMaxPhotonArea_S1->Fill(A);
  }

  //Now do two DCs, DPE fraction = 0%
  fDPE->SetParameters(1., 1., 0.38, 0.);

  for(long int i = 0; i < nsamples; ++i)
  {
    a1 = fDPE->GetRandom();
    a2 = fDPE->GetRandom();
    if(a1 >= a2)
      A = a1;
    else
      A = a2;
    hMaxPhotonArea_DCs->Fill(A);
  }

  TCanvas *c2 = new TCanvas("c2", "", 800, 600);
  c2->SetLogy();
  hMaxPhotonArea_S1->Draw("histo");
  hMaxPhotonArea_DCs->Draw("samehisto");

  //What percentage of real and fake S1s do we throw away if we require that at least one of the photon pulses has an area greater than X?
  //We can create custom functions that return these values (or, more sensibly, we could calculate a few points and put them in a graph instead):

  TCanvas *c3 = new TCanvas("c3", "", 800, 600);
  auto c3frame = c3->DrawFrame(0., 0., 3., 1.); //pre-create the axes to draw on, so the ranges are specified
  c3frame->GetXaxis()->SetTitle("DPE cut [phe]");
  c3frame->GetYaxis()->SetTitle("Efficiency");

  TF1* fS1eff = new TF1("fS1eff", [&](double *x, double *p){int bin = hMaxPhotonArea_S1->FindBin(x[0]); return hMaxPhotonArea_S1->Integral(bin, hMaxPhotonArea_S1->GetNbinsX())/hMaxPhotonArea_S1->Integral();}, 0., 3., 0);
  fS1eff->SetNpx(hMaxPhotonArea_S1->GetNbinsX());
  fS1eff->SetLineColor(kBlack);
  fS1eff->Draw("same");

  TF1* fDCeff = new TF1("fDCeff", [&](double *x, double *p){int bin = hMaxPhotonArea_DCs->FindBin(x[0]); return hMaxPhotonArea_DCs->Integral(bin, hMaxPhotonArea_DCs->GetNbinsX())/hMaxPhotonArea_DCs->Integral();}, 0., 3., 0);
  fDCeff->SetNpx(hMaxPhotonArea_DCs->GetNbinsX());
  fDCeff->SetLineColor(kRed);
  fDCeff->Draw("same");

  //How useful a certain cut is will depend on the absolute rate of events with real S1s and S2s vs events formed of DC coincidences plus an S2. If you have those rates, you can use this plot to calculate the rates after the cut :-)
}

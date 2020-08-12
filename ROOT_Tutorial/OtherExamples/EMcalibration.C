#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "Math/Vector4D.h"
#include "TTreeReader.h"
#include "TCanvas.h"
#include "TF1.h"

 
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"

//should probably do this as a class, but no time
namespace em
{
  TFile* fFSR = new TFile("fFSR.root", "READ");

  TTreeReader rZmumug("tZmumug", fFSR);
  TTreeReaderValue<double> mu1E(rZmumug, "mu1E");
  TTreeReaderValue<double> mu1p(rZmumug, "mu1p");

  TTreeReaderValue<double> mu2E(rZmumug, "mu2E");
  TTreeReaderValue<double> mu2px(rZmumug, "mu2px");
  TTreeReaderValue<double> mu2py(rZmumug, "mu2py");

  TTreeReaderValue<double> phE(rZmumug, "phE");
  TTreeReaderValue<double> phphi(rZmumug, "phphi");

  TTreeReader rZmumu("tZmumu", fFSR);
  TTreeReaderValue<double> mu1E_mm(rZmumu, "mu1E");
  TTreeReaderValue<double> mu1p_mm(rZmumu, "mu1p");

  TTreeReaderValue<double> mu2E_mm(rZmumu, "mu2E");
  TTreeReaderValue<double> mu2p_mm(rZmumu, "mu2px");

  TH1D* hZmassMuMuGamma_noFSR = new TH1D("hZmassMuMuGamma_noFSR", ";Uncorrected #mu#mu mass [GeV];", 250, 60., 110.);
  TH1D* hZmassMuMuGamma_FSR = new TH1D("hZmassMuMuGamma_FSR", ";Corrected #mu#mu#gamma mass [GeV];", 250, 60., 110.);
  TH1D* hZmassMuMu = new TH1D("hZmassMuMu", ";#mu#mu Mass [GeV];", 250, 60., 110.);

  ROOT::Math::XYZTVector mu1;
  ROOT::Math::XYZTVector mu2;
  ROOT::Math::XYZTVector ph;

  TF1* fitZmumug = new TF1("fitZmumug", "gaus", 60., 110.);

  double ZmumuMean = -1;
  double mean, sigma;

}

using namespace em;

void fillZmumug(double EMscale = 1.);
void fillZmumu();
double fitAndCompare();
double fillAndFit(const double *EMscale);
double fillAndKS(const double *EMscale);

void EMcalibration()
{
  //Copying from: https://root.cern/doc/master/NumericalMinimization_8C.html
  
  ROOT::Math::Minimizer* calibrator = ROOT::Math::Factory::CreateMinimizer("Minuit2", "");
  calibrator->SetMaxFunctionCalls(10000000); // for Minuit/Minuit2
  calibrator->SetMaxIterations(10000);  // for GSL
  calibrator->SetTolerance(0.001); //"minimisation will stop when the computed EDM is less than 0.002 * tolerance * 1"
  calibrator->SetStrategy(1); //0, 1, 2 from +speed to +precision
  calibrator->SetPrintLevel(2);

  ROOT::Math::Functor f(&fillAndKS, 1); //wrap the function to feed it to the minimiser
  double step[1] = {1}; //not fixed, just gives the algorithm an idea of the scale.. (consider doing a for loop for fixed steps, which is actually what I did long ago!)
  double variable[1] = {0.91};

  calibrator->SetFunction(f);
  calibrator->SetVariable(0, "EMscale", variable[0], step[0]);
  calibrator->Minimize();
  
  const double *xs = calibrator->X();
  std::cout << "Minimum: f(" << xs[0] << "): " << calibrator->MinValue() << std::endl;

  fillZmumug(xs[0]); //recreate histogram with the value that we found
  
  TCanvas *cmass = new TCanvas("cmass", "", 800, 600);
  auto cmassframe = cmass->DrawFrame(60., 0., 110., 2.5E2);

  cmassframe->GetXaxis()->SetTitle("Dimuon mass [GeV]");

  hZmassMuMu->Scale(hZmassMuMuGamma_FSR->Integral()/hZmassMuMu->Integral());
  hZmassMuMu->SetLineColor(kBlue);
  hZmassMuMu->Draw("samehisto");

  hZmassMuMuGamma_noFSR->SetLineColor(kRed);
  hZmassMuMuGamma_noFSR->SetMarkerColor(kRed);
  hZmassMuMuGamma_noFSR->SetMarkerStyle(kFullCircle);
  hZmassMuMuGamma_noFSR->Draw("sameerrors");

  hZmassMuMuGamma_FSR->SetLineColor(kBlack);
  hZmassMuMuGamma_FSR->SetMarkerColor(kBlack);
  hZmassMuMuGamma_FSR->SetMarkerStyle(kFullCircle);
  hZmassMuMuGamma_FSR->Draw("sameerrors");

  TLegend *leg = new TLegend(0.14787, 0.659722, 0.464912, 0.831597);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->AddEntry(hZmassMuMu, "M_{#mu#mu} Z#rightarrow#mu#mu data", "l");
  leg->AddEntry(hZmassMuMuGamma_noFSR, "M_{#mu#mu} Z#rightarrow#mu#mu#gamma data", "lp");
  leg->AddEntry(hZmassMuMuGamma_FSR, "M_{#mu#mu#gamma} Z#rightarrow#mu#mu#gamma data", "lp");
  leg->Draw();
}

void fillZmumug(double EMscale)
{
  rZmumug.Restart();
  hZmassMuMuGamma_FSR->Reset();
  while(rZmumug.Next())
    {
      mu1.SetCoordinates(*mu1p, 0., 0., *mu1E);
      mu2.SetCoordinates(*mu2px, *mu2py, 0., *mu2E);

      ph.SetCoordinates(- *phE/EMscale * cos(*phphi), *phE/EMscale * sin(*phphi), 0., *phE/EMscale);

      hZmassMuMuGamma_FSR->Fill((mu1 + mu2 + ph).M());
    }
}

void fillZmumu()
{
  rZmumu.Restart();
  hZmassMuMu->Reset();
  while(rZmumu.Next())
    {
      mu1.SetCoordinates(*mu1p_mm, 0., 0., *mu1E_mm);
      mu2.SetCoordinates(*mu2p_mm, 0., 0., *mu2E_mm);

      hZmassMuMu->Fill((mu1 + mu2).M());
    }
  rZmumug.Restart();
  hZmassMuMuGamma_noFSR->Reset();
  while(rZmumug.Next())
    {
      mu1.SetCoordinates(*mu1p, 0., 0., *mu1E);
      mu2.SetCoordinates(*mu2px, *mu2py, 0., *mu2E);

      hZmassMuMuGamma_noFSR->Fill((mu1 + mu2).M());
    }

}

double fitAndCompare()
{
  if(ZmumuMean < 0)
    {
      mean = hZmassMuMu->GetMean();
      sigma = hZmassMuMu->GetRMS();
      fitZmumug->SetRange(mean-3*sigma, mean+3*sigma);
      fitZmumug->SetParameters(1., mean, sigma);
      hZmassMuMu->Fit(fitZmumug, "0R");
      ZmumuMean = fitZmumug->GetParameter(1);
    }

  mean = hZmassMuMuGamma_FSR->GetMean();
  sigma = hZmassMuMuGamma_FSR->GetRMS();
  fitZmumug->SetRange(mean-3*sigma, mean+3*sigma);
  fitZmumug->SetParameters(1., mean, sigma);
  hZmassMuMuGamma_FSR->Fit(fitZmumug, "0R");
  return pow(ZmumuMean/fitZmumug->GetParameter(1), 1);

}

double fillAndFit(const double *EMscale)
{
  if(ZmumuMean < 0)
    {
      fillZmumu();
    }
  fillZmumug(EMscale[0]);

  return fitAndCompare();
}

double fillAndKS(const double *EMscale)
{
  if(ZmumuMean < 0)
    {
      fillZmumu();
    }
  fillZmumug(EMscale[0]);

  return hZmassMuMu->KolmogorovTest(hZmassMuMuGamma_FSR, "M");
}

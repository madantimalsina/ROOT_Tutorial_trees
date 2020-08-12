#include <iostream>
#include <iomanip>

#include "CEST.h"
#include <random>

#include "TFile.h"
#include "TH1D.h"
#include "TTree.h"
#include "TF1.h"

int main(int argc, char *argv[])
{
  double alpha = 0.9, beta = 7.1, gamma = -0.3, W = 13.6;
  double Emin = 0., Emax = 20.;
  CEST* c = new CEST(alpha, beta, gamma, W, Emin, Emax);

  long int nsamples = 1E5;
  long int samples_above_threshold = 0;
  int threshold = 3;

  double thisE = 0;
  double thisNe = 0;

  TFile* fOut = new TFile("output.root", "RECREATE");
  //Create a histogram of the number of electrons
  TH1D* hNe = new TH1D("hNe", ";Number of electrons;#Occurrences", 101, -0.5, 100.5);

  //Save the energy and number of electrons of each sample
  TTree *tree = new TTree("CESTree", "CESTree");
  //link the variables "thisE" and "thisNe" to the tree
  //when we call tree->Fill(), the contents of those variables will be saved
  tree->Branch("E", &thisE, "E/D");
  tree->Branch("Ne", &thisNe, "Ne/D");


  //Check TF1 constructors here: https://root.cern.ch/doc/master/classTF1.html
  TF1* fGauss = new TF1("fGauss", "gausn", 0, 20);
  fGauss->SetParameters(1, 7.5, 2.5);

  TF1* fModGauss = new TF1("fModGauss", [&](double *x, double *p){if(x[0] < 3){return fGauss->Eval(x[0])/p[0] + p[1]*x[0];} else {return fGauss->Eval(x[0]);} }, 0, 20, 2 /*Number of parameters*/);
  fModGauss->SetParameters(3, 0.001);

  std::cout << std::setw(5) << "E/keV" << std::setw(5) << "Ne" << std::endl;
  for(long int i = 0; i < nsamples; ++i )
  {
    //thisE = c->getE();
    //thisE = fGauss->GetRandom();
    thisE = fModGauss->GetRandom();
    thisNe = c->getNe(thisE);
    //std::cout << std::setw(10) << thisE << std::setw(10) << thisNe << std::endl;
    if(thisNe > threshold)
    {
      samples_above_threshold += 1;
      hNe->Fill(thisNe);
    }
    tree->Fill(); //Save the current values of thisE and thisNe
  }

  std::cout << "Samples above threshold: " << samples_above_threshold << "/" << nsamples << std::endl;


  hNe->Write();
  tree->Write();
  fOut->Close();

  return 0;
}

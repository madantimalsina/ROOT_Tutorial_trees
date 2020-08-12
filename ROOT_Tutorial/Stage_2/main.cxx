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

  long int nsamples_MC = 1E5;
  long int nsamples_data = 1E3;

  double thisE = 0;
  double thisNe = 0;

  TFile* fOut_MC = new TFile("output_MC.root", "RECREATE");

  TTree *tree_MC = new TTree("CESTree", "CESTree");
  tree_MC->Branch("E", &thisE, "E/D");
  tree_MC->Branch("Ne", &thisNe, "Ne/D");


  TF1* fGauss = new TF1("fGauss", "gausn", 0, 20);
  fGauss->SetParameters(1, 7.5, 2.5);

  for(long int i = 0; i < nsamples_MC; ++i )
  {
    thisE = fGauss->GetRandom();
    thisNe = c->getNe(thisE);
    tree_MC->Fill();
  }
  tree_MC->Write();
  fOut_MC->Close();

  //Data and simulation trees will usually have the same name, so we need to store them in a different file...
  TFile* fOut_data = new TFile("output_data.root", "RECREATE");

  TTree *tree_data = new TTree("CESTree", "CESTree");
  tree_data->Branch("E", &thisE, "E/D");
  tree_data->Branch("Ne", &thisNe, "Ne/D");

  fGauss->SetParameters(1, 7.7, 2.4);

  for(long int i = 0; i < nsamples_data; ++i )
  {
    thisE = fGauss->GetRandom();
    thisNe = c->getNe(thisE);
    tree_data->Fill();
  }
  tree_data->Write();
  fOut_data->Close();

  return 0;
}

#include "TTree.h"
#include "TFile.h"
#include "TF1.h"

void FSRsim()
{
  //The Z boson may decay into two muons (Z->mu+mu-). Sometimes, however, one may find photons too (Z->mu+mu-gamma) due to one of the muons radiating a photon in the final state (FSR, Final State Radiation).

  //In particle detectors where muon reconstruction is "easy" in comparison with photon reconstruction (such as ATLAS or CMS), one can use these Z->mumugamma events to calibrate the photon reconstruction by comparing the Z mass reconstructed with Z->mumugamma and the Z mass reconstructed from Z->mumu events.

  //The analysis continues in EMcalibration.C, this script only simulates some data...
  

  //Since we have nobody to give us some data, we need to simulate some of it. Some part of physics will definitely be violated in the following lines:

  double Zmass = 91.1876; //GeV
  double Zwidth = 1.5*2.4952; //GeV //can't remember what's a reasonable detector resolution
  
  TF1* fZboson = new TF1("fZboson", "gaus", 60., 110.);
  fZboson->SetParameters(1, Zmass, Zwidth);

  double Mumass = 0.105658; //GeV
  
  double thisZmass = 0.;

  //Made up functions to sample photon angle and energy (wrt initial muon) 
  TF1* fPhPhi = new TF1("fPhPhi", "expo", 0., TMath::Pi()/2.5);
  fPhPhi->SetParameters(1., -1./(0.1*TMath::Pi()));

  TF1* fPhE = new TF1("fPhE", "expo", 0., 15.);
  fPhE->SetParameters(1., -1./10.);
  
  double mu1E, mu1p;
  double mu2E, mu2px, mu2py, mu2phi;
  double phE, phE_true, phpx, phpy, phphi;

  TFile *fFSR = new TFile("fFSR.root", "RECREATE");
  TTree* tZmumug = new TTree("tZmumug", "Tree with FSR events");
  tZmumug->Branch("mu1E", &mu1E, "mu1E/D");
  tZmumug->Branch("mu1p", &mu1p, "mu1p/D");

  tZmumug->Branch("mu2E", &mu2E, "mu2E/D");
  tZmumug->Branch("mu2px", &mu2px, "mu2px/D");
  tZmumug->Branch("mu2py", &mu2py, "mu2py/D");
  tZmumug->Branch("mu2phi", &mu2phi, "mu2phi/D");

  tZmumug->Branch("phE", &phE, "phE/D");
  tZmumug->Branch("phE_true", &phE_true, "phE_true/D");
  tZmumug->Branch("phphi", &phphi, "phphi/D");

  TTree* tZmumu = tZmumug->CloneTree();
  tZmumu->SetName("tZmumu");
  tZmumu->SetTitle("Tree with FSRless events");

  //Fill FSR tree

  TF1* fPhotonMiscal = new TF1("fPhotonMiscal", "gausn", -2, 2);
  fPhotonMiscal->SetNpx(200.);
  fPhotonMiscal->SetParameters(1., 0.1, 0.1);

  long int nsamples = 1E4;

  for(long int i = 0; i < nsamples; ++i)
    {
      thisZmass = fZboson->GetRandom();
      mu1E = thisZmass/2 - (Mumass*Mumass/thisZmass); //4-momentum calculation assuming Z decays at rest
      mu1p = sqrt(mu1E*mu1E - Mumass*Mumass);

      phphi = fPhPhi->GetRandom();
      fPhE->SetRange(0., mu1E/2.);
      phE = fPhE->GetRandom();
      
      phpx = -phE*cos(phphi);
      phpy = phE*sin(phphi);

      mu2E = mu1E - phE;
      mu2px = -mu1p - phpx;
      mu2py = -phpy;
      mu2phi  = -atan(mu2py/mu2px);


      //cout << thisZmass << ", " << sqrt(pow(mu1E + mu2E + phE, 2) - pow(mu1p + mu2px + phpx, 2) - pow(mu2py + phpy, 2)) << endl;
      phE_true = phE;
      phE *= 1. + fPhotonMiscal->GetRandom();
      tZmumug->Fill();
    }


  nsamples *= 50;
  //Fill FSRless tree
  for(long int i = 0; i < nsamples; ++i)
    {
      thisZmass = fZboson->GetRandom();
      mu1E = thisZmass/2 - (Mumass*Mumass/thisZmass); //4-momentum calculation assuming Z decays at rest
      mu1p = sqrt(mu1E*mu1E - Mumass*Mumass);

      phphi = 0; 
      phE = 0;
      phE_true = 0;
      phpx = 0;
      phpy = 0;

      mu2E = mu1E;
      mu2px = -mu1p;
      mu2py = 0;
      mu2phi  = 0;


      // cout << thisZmass << ", " << sqrt(pow(mu1E + mu2E + phE, 2) - pow(mu1p + mu2px + phpx, 2) - pow(mu2py + phpy, 2)) << endl;

      tZmumu->Fill();
    }

  tZmumug->Write();
  tZmumu->Write();
  fFSR->Close();
}

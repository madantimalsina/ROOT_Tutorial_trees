#include <iostream>
#include <iomanip>

#include "CEST.h"
#include <random>
int main(int argc, char *argv[])
{
  double alpha = 0.9, beta = 7.1, gamma = -0.3, W = 13.6;
  double Emin = 0., Emax = 20.;
  CEST* c = new CEST(alpha, beta, gamma, W, Emin, Emax);

  long int nsamples = 1E3;
  long int samples_above_threshold = 0;
  int threshold = 3;

  double thisE = 0;
  double thisNe = 0;

  std::cout << std::setw(5) << "E/keV" << std::setw(5) << "Ne" << std::endl;
  for(long int i = 0; i < nsamples; ++i )
  {
    thisE = c->getE();
    thisNe = c->getNe(thisE);
    std::cout << std::setw(10) << thisE << std::setw(10) << thisNe << std::endl;
    if(thisNe > threshold)
      samples_above_threshold += 1;
  }

  std::cout << "Samples above threshold: " << samples_above_threshold << "/" << nsamples << std::endl;

  return 0;
}

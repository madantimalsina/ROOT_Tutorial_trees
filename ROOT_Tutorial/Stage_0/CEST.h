#include <random>

class CEST
{
  double alpha;
  double beta;
  double gamma;
  double W;
  double Emin;
  double Emax;
  std::default_random_engine generator;
  std::poisson_distribution<int> pois;
  std::uniform_real_distribution<double> flat;

  CEST();
 public:
  int getNe(double);
  double getQy(double);
  double getE();

  CEST(double, double, double, double, double, double);

};

CEST::CEST(double alpha, double beta, double gamma, double W, double Emin, double Emax)
{
  this->alpha = alpha;
  this->beta = beta;
  this->gamma = gamma;
  this->W = W;
  std::uniform_real_distribution<double>::param_type range(Emin, Emax);
  flat.param(range);

}

double CEST::getQy(double E)
{
  return (1./W)*(alpha + beta*E + gamma*E*E);
}

int CEST::getNe(double E)
{
  std::poisson_distribution<int>::param_type mean(getQy(E) * E);
  pois.param(mean);
  
  return pois(generator);
}

double CEST::getE()
{
  return flat(generator);
}

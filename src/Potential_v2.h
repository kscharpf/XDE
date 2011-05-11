#ifndef POTENTIAL_V2_H
#define POTENTIAL_V2_H

#include <vector>

#include "Random.h"
#include "Matrix.h"
#include "Utility_v2.h"

inline double potentialA(int Q,
			 const double *a,
			 double pA0,
			 double pA1,
			 double alphaA,
			 double betaA) {
  double pot = 0.0;

  unsigned int seed = 1;
  Random ran(seed);
  
  int q;
  for (q = 0; q < Q; q++) {
    if (a[q] == 0.0)
      pot += - log(pA0);
    else if (a[q] == 1.0)
      pot += - log(pA1);
    else {
      pot += - log(1.0 - pA0 - pA1);
      pot += ran.PotentialBeta(alphaA,betaA,a[q]);
    }
  }
  
  return pot;
}

		     

inline double potentialB(int Q,
			 const double *b,
			 double pB0,
			 double pB1,
			 double alphaB,
			 double betaB) {
  double pot = 0.0;

  unsigned int seed = 1;
  Random ran(seed);
  
  int q;
  for (q = 0; q < Q; q++) {
    if (b[q] == 0.0)
      pot += - log(pB0);
    else if (b[q] == 1.0)
      pot += - log(pB1);
    else {
      pot += - log(1.0 - pB0 - pB1);
      pot += ran.PotentialBeta(alphaB,betaB,b[q]);
    }
  }
  
  return pot;
}


inline double potentialNug(int Q,
			   const double *nug,
			   double gamma2,
			   const double *a,
			   const double *rho,
			   const double *tau2Rho,
			   const double *sigma2g) {
  double pot = 0.0;
  
  unsigned int seed = 1;
  Random ran(seed);
  
  vector<vector<double> > Sigma;
  makeSigma(Sigma,Q,gamma2,tau2Rho,a,sigma2g,rho);

  vector<vector<double> > SSigma;
  double determinant = inverse(Sigma,SSigma);

  vector<double> value(Q,0.0);
  int q;
  for (q = 0; q < Q; q++)
    value[q] = nug[q];

  pot += ran.PotentialMultiGaussian(SSigma,determinant,value);
  
  return pot;
}
			      


inline double potentialNu(int Q,
			  int G,
			  const double *nu,
			  double gamma2,
			  const double *a,
			  const double *rho,
			  const double *tau2Rho,
			  const double *sigma2) {
  double pot = 0.0;

  int g;
  for (g = 0; g < G; g++)
    pot += potentialNug(Q,nu + g * Q,gamma2,a,rho,tau2Rho,sigma2 + g * Q);
  
  return pot;
}


#endif
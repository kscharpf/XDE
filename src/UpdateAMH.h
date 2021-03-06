#ifndef UPDATEAMH_H
#define UPDATEAMH_H

#include "Update.h"

class UpdateAMH : public Update
{
 public:

  UpdateAMH(Structure *str,const Potential *model,double epsilon);
  ~UpdateAMH(void);

  int update(Random &ran);
  Update *copy(void) const;

 private:
  Structure *str;
  Potential *model;
};



inline UpdateAMH::UpdateAMH(Structure *str,const Potential *model,double epsilon) : Update(epsilon)
{
  this->str = str;
  this->model = model->copy();
  
  return;
}



inline UpdateAMH::~UpdateAMH(void)
{
  delete model;

  return;
}


inline Update *UpdateAMH::copy(void) const
{
  Update *u = new UpdateAMH(str,model,epsilon);

  return u;
}




inline int UpdateAMH::update(Random &ran)
{
  int nAccept = 0;

  int q;
  for (q = 0; q < str->Q; q++)
    {
      addTry();
  
      double oldValue = str->a[q];
      double p0 = 0.0;
      double p1 = 0.0;
      if (oldValue > 0.0 && oldValue < 1.0)
	{
	  if (str->pA0 > 0.0 && oldValue - epsilon < 0.0) p0 = (epsilon - oldValue) / (2.0 * epsilon);
	  if (str->pA1 > 0.0 && oldValue + epsilon > 1.0) p1 = (oldValue + epsilon - 1.0) / (2.0 * epsilon);
	}

      double newValue;
      double lower = 0.0;
      double upper = 0.0;
      double u = ran.Unif01();
      if (u < p0)
	newValue = 0.0;
      else if (u < p0 + p1)
	newValue = 1.0;
      else
	{
	  lower = oldValue - epsilon;
	  upper = oldValue + epsilon;
	  if (lower < 0.0) lower = 0.0;
	  if (upper > 1.0) upper = 1.0;
	  newValue = lower + (upper - lower) * ran.Unif01();
	}


      double p0Back = 0.0;
      double p1Back = 0.0;
      if (newValue > 0.0 && newValue < 1.0)
	{
	  if (str->pA0 > 0.0 && newValue - epsilon < 0.0) p0Back = (epsilon - newValue) / (2.0 * epsilon);
	  if (str->pA1 > 0.0 && newValue + epsilon > 1.0) p1Back = (newValue + epsilon - 1.0) / (2.0 * epsilon);
	}

      double lowerBack = 0.0;
      double upperBack = 1.0;
      if (oldValue > 0.0 && oldValue < 1.0)
	{
	  lowerBack = newValue - epsilon;
	  upperBack = newValue + epsilon;
	  if (lowerBack < 0.0) lowerBack = 0.0;
	  if (upperBack > 1.0) upperBack = 1.0;
	}


      double pot = 0.0;
      if (oldValue == 0.0)  // then (newValue > 0.0 && newValue < 1.0)
	{
	  pot -= - log(1.0);
	  pot -= - log(1.0 / (upper - lower));
	  pot += - log(p0Back);
	}
      else if (oldValue == 1.0) // then (newValue > 0.0 && newValue < 1.0)
	{
	  pot -= - log(1.0);
	  pot -= - log(1.0 / (upper - lower));
	  pot += - log(p1Back);
	}
      else  // then (oldValue > 0.0 && oldValue < 1.0)
	{
	  if (newValue == 0.0)
	    {
	      pot -= - log(p0);
	      pot += - log(1.0);
	      pot += - log(1.0 / (upperBack - lowerBack));
	    }
	  else if (newValue == 1.0)
	    {
	      pot -= - log(p1);
	      pot += - log(1.0);
	      pot += - log(1.0 / (upperBack - lowerBack));
	    }
	  else  // then (newValue > 0.0 && newValue < 1.0)
	    {
	      pot -= - log(1.0 - p0 - p1);
	      pot -= - log(1.0 / (upper - lower));
	      pot += - log(1.0 - p0Back - p1Back);
	      pot += - log(1.0 / (upperBack - lowerBack));
	    }
	}


      pot -= model->potential(ran);

      str->a[q] = newValue;

      pot += model->potential(ran);

      str->a[q] = oldValue;


      if (ran.Unif01() <= exp(- pot))
	{
	  str->a[q] = newValue;
	  addAccept();
	  nAccept++;
	}
    }

  return nAccept;
}


#endif

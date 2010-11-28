#ifndef Mtr_h
#define Mtr_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "dmdata.h"

/////////////////////////////////////////////////
// Metric
class TMtr{
private:
  TCRef CRef;
public:
  TMtr(){}
  virtual ~TMtr(){}
  static TPt<TMtr> Load(TSIn&){Fail; return NULL;}
  virtual void Save(TSOut&){Fail;}

  virtual double GetDist(const PValRet& LValRet, const PValRet& RValRet)=0;

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm);
  static TPt<TMtr> New(const PPp& Pp);

  friend TPt<TMtr>;
};
typedef TPt<TMtr> PMtr;

/////////////////////////////////////////////////
// Manhattan-Metric
class TMtrManhattan: public TMtr{
public:
  TMtrManhattan(): TMtr(){}
  TMtrManhattan(const PPp&): TMtr(){}

  double GetDist(const PValRet& LValRet, const PValRet& RValRet);

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){return new TPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Euclidean-Metric
class TMtrEucl: public TMtr{
public:
  TMtrEucl(): TMtr(){}
  TMtrEucl(const PPp&): TMtr(){}

  double GetDist(const PValRet& LValRet, const PValRet& RValRet);

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){return new TPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// Cosine-Metric
class TMtrCos: public TMtr{
public:
  TMtrCos(): TMtr(){}
  TMtrCos(const PPp&): TMtr(){}

  double GetDist(const PValRet& LValRet, const PValRet& RValRet);

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm){return new TPp(Nm, DNm);}
};

/////////////////////////////////////////////////
// VDM-Metric
class TMtrVdm: public TMtr{
private:
  PPrbEst PrbEst;
public:
  TMtrVdm(const PPrbEst& _PrbEst): TMtr(), PrbEst(_PrbEst){}
  TMtrVdm(const PPp& Pp):
    TMtr(), PrbEst(TPrbEst::New(Pp->GetPp(TTypeNm<TPrbEst>()))){}

  double GetDist(const PValRet& LValRet, const PValRet& RValRet);

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm);
};

#endif


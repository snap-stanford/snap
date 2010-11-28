#ifndef Mg_h
#define Mg_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include "md.h"
#include "mdtr.h"
#include "mtr.h"

/////////////////////////////////////////////////
// Model-Generator
class TMg{
private:
  TCRef CRef;
private:
  PDm Dm;
  int ClassN;
public:
  TMg(const PDm& _Dm=NULL, const int& _ClassN=-1): Dm(_Dm), ClassN(_ClassN){}
  TMg(const PPp& Pp, const PDm& _Dm=NULL, const int& _ClassN=-1);
  virtual ~TMg(){}
  static TPt<TMg> Load(TSIn&){Fail; return NULL;}
  virtual void Save(TSOut&){Fail;}

  PDm GetDm() const {return Dm;}
  int GetClassN() const {return ClassN;}
  virtual PMd GetMd(const PExSet& ExSet) const=0;
  virtual PMd GetMxMd() const {return GetMd(GetDm()->GetExSet());}

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm);
  static void GetSubPp(const PPp& Pp);
  static TPt<TMg> New(const PPp& Pp, const PDm& _Dm, const int& _ClassN);

  friend TPt<TMg>;
};
typedef TPt<TMg> PMg;

/////////////////////////////////////////////////
// Model-Generator-Prior
class TMgPrior: public TMg{
public:
  TMgPrior(const PDm& Dm, const int& ClassN): TMg(Dm, ClassN){}
  TMgPrior(const PPp& Pp, const PDm& Dm, const int& ClassN):
    TMg(Pp, Dm, ClassN){}

  PMd GetMd(const PExSet& ExSet) const;

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm);
};

/////////////////////////////////////////////////
// Model-Generator-Bayes
class TMgBayes: public TMg{
public:
  TMgBayes(const PDm& Dm, const int& ClassN): TMg(Dm, ClassN){}
  TMgBayes(const PPp& Pp, const PDm& Dm, const int& ClassN):
    TMg(Pp, Dm, ClassN){}

  PMd GetMd(const PExSet& ExSet) const;

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm);
};

/////////////////////////////////////////////////
// Model-Generator-Nearest-Neighbour
class TMgNNbr: public TMg{
private:
  PMtr Mtr;
  int Nbrs;
public:
  TMgNNbr(const PDm& Dm, const int& ClassN, const PMtr& _Mtr, const int& _Nbrs):
    TMg(Dm, ClassN), Mtr(_Mtr), Nbrs(_Nbrs){}
  TMgNNbr(const PPp& Pp, const PDm& Dm, const int& ClassN);

  PMd GetMd(const PExSet& ExSet) const;

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm);
};

/////////////////////////////////////////////////
// Model-Generator-Id3
class TMgId3: public TMg{
private:
  class TSplit;
  TSplit TMgId3::GetBestSplit(const PExSet& ExSet);
  PDNd GetDNd(const PExSet& ExSet, const PExSet& AllExSet) const;
public:
  TMgId3(const PDm& Dm, const int& ClassN): TMg(Dm, ClassN){}
  TMgId3(const PPp& Pp, const PDm& Dm, const int& ClassN):
    TMg(Pp, Dm, ClassN){}

  PMd GetMd(const PExSet& ExSet) const;

  static const TStr DNm;
  static PPp GetPp(const TStr& Nm, const TStr& DNm);
};

#endif


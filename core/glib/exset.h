/////////////////////////////////////////////////
// Example-Set
ClassTPV(TExSet, PExSet, TExSetV)//{
private:
  UndefCopy(TExSet);
public:
  TExSet(){}
  static PExSet New(const TStr& TypeNm);
  virtual ~TExSet(){}
  TExSet(TSIn&){}
  static PExSet Load(TSIn& SIn);
  virtual void Save(TSOut& SOut){GetTypeNm(*this).Save(SOut);}

  TExSet& operator=(const TExSet&){return *this;}

  // examples administration
  virtual int GetExs() const=0;
  virtual double GetExsWgt() const=0;
  virtual int GetExId(const int& ExP) const=0;
  virtual double GetExWgt(const int& ExP) const=0;
  virtual int AddEx(const int& ExId, const double& ExWgt=1)=0;
  virtual void DelEx(const int& ExP)=0;
  virtual int FFirstExP() const=0;
  virtual bool FNextExP(int& ExP) const=0;

  // example-set splitting
  virtual void SplitPerPrb(
   const double& LPrb, TRnd& Rnd, PExSet& LExSet, PExSet& RExSet)=0;
  virtual void SplitToFolds(
   const int& Folds, TRnd& Rnd, TExSetV& FoldV)=0;

  // set operations
  virtual PExSet GetIntrs(const PExSet& ExSet) const=0;
};

/////////////////////////////////////////////////
// Generic-Example-Set
class TGExSet: public TExSet{
private:
  TIntFltKdV ExIdWgtKdV;
  TFlt ExsWgt;
  void RecalcExsWgt();
public:
  TGExSet(const int& ExpectExs=0):
    TExSet(), ExIdWgtKdV(ExpectExs, 0), ExsWgt(0){}
  static PExSet New(const int& ExpectExs=0){
    return PExSet(new TGExSet(ExpectExs));}
  static void NewV(const int& ExSets, TExSetV& ExSetV);
  TGExSet(TSIn& SIn):
    TExSet(SIn), ExIdWgtKdV(SIn), ExsWgt(SIn){}
  void Save(TSOut& SOut){
    TExSet::Save(SOut); ExIdWgtKdV.Save(SOut); ExsWgt.Save(SOut);}

  TExSet& operator=(const TGExSet& ExSet){
    TExSet::operator=(ExSet);
    ExIdWgtKdV=ExSet.ExIdWgtKdV; ExsWgt=ExSet.ExsWgt;
    return *this;}

  // examples administration
  int GetExs() const {return ExIdWgtKdV.Len();}
  double GetExsWgt() const {return ExsWgt;}
  int GetExId(const int& ExP) const {return ExIdWgtKdV[ExP].Key;}
  double GetExWgt(const int& ExP) const {return ExIdWgtKdV[ExP].Dat;}
  int AddEx(const int& ExId, const double& ExWgt=1){
    ExsWgt+=ExWgt; return ExIdWgtKdV.AddSorted(TIntFltKd(ExId, ExWgt));}
  void DelEx(const int& ExP){
    ExsWgt-=GetExWgt(ExP); ExIdWgtKdV.Del(ExP);}
  int FFirstExP() const {return 0-1;}
  bool FNextExP(int& ExP) const {ExP++; return ExP<ExIdWgtKdV.Len();}

  // example-set splitting
  void SplitPerPrb(
   const double& LPrb, TRnd& Rnd, PExSet& LExSet, PExSet& RExSet);
  void SplitToFolds(
   const int& Folds, TRnd& Rnd, TExSetV& FoldV);

  // set operations
  PExSet GetIntrs(const PExSet& ExSet) const;
};

/////////////////////////////////////////////////
// Binary-Example-Set
class TBExSet: public TExSet{
private:
  PBSet ExIdBSet;
public:
  TBExSet(const int& ExIds):
    TExSet(), ExIdBSet(TBSet::New(ExIds)){}
  static PExSet New(const int& ExIds){
    return PExSet(new TBExSet(ExIds));}
  static void NewV(const int& ExSets, TExSetV& ExSetV);
  TBExSet(TSIn& SIn):
    TExSet(SIn), ExIdBSet(SIn){}
  void Save(TSOut& SOut){
    TExSet::Save(SOut); ExIdBSet.Save(SOut);}

  TExSet& operator=(const TBExSet& ExSet){
    TExSet::operator=(ExSet); ExIdBSet=ExSet.ExIdBSet; return *this;}

  // examples administration
  int GetExs() const {return ExIdBSet->Get1s();}
  double GetExsWgt() const {return ExIdBSet->Get1s();}
  int GetExId(const int& ExP) const {return ExP;}
  double GetExWgt(const int& ExP) const {return ExIdBSet->In(ExP)?1:0;}
  int AddEx(const int& ExId, const double& /*ExWgt*/=1){
    ExIdBSet->Incl(ExId); return ExId;}
  void DelEx(const int& ExP){ExIdBSet->Excl(ExP);}
  int FFirstExP() const {return 0-1;}
  bool FNextExP(int& ExP) const;

  // example-set splitting
  void SplitPerPrb(
   const double& LPrb, TRnd& Rnd, PExSet& LExSet, PExSet& RExSet);
  void SplitToFolds(
   const int& Folds, TRnd& Rnd, TExSetV& FoldV);

  // set operations
  PExSet GetIntrs(const PExSet& ExSet) const;
};


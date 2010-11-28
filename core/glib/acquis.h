/////////////////////////////////////////////////
// Acquis-Base
ClassTP(TAcquisBs, PAcquisBs)//{
private:
  UndefCopyAssign(TAcquisBs);
public:
  TAcquisBs(){}
  static PAcquisBs New(){
    return new TAcquisBs();}
  TAcquisBs(TSIn& SIn){}
  static PAcquisBs Load(TSIn& SIn){return new TAcquisBs(SIn);}
  void Save(TSOut& SOut) const {}

  // files
  static void SaveAcquisToCpd(
   const TStr& InAcquisFPath, const TStrV& AcquisLangNmV, const TStr& OutCpdFNm);
};


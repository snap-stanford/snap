/////////////////////////////////////////////////
// String-Cache
class TStrCache{
private:
  TStrH StrH;
  int Hits, Rqs;
public:
  TStrCache(const int& ExpectedStrs=0):
    StrH(ExpectedStrs), Hits(0), Rqs(0){}
  TStrCache(const TStrCache& StrCache):
    StrH(StrCache.StrH), Hits(StrCache.Hits), Rqs(StrCache.Rqs){}
  TStrCache(TSIn& SIn): StrH(SIn), Hits(0), Rqs(0){}
  void Save(TSOut& SOut){StrH.Save(SOut);}

  TStrCache& operator=(const TStrCache& StrCache){
    if (this!=&StrCache){StrH=StrCache.StrH; Hits=0; Rqs=0;} return *this;}

  void Clr(){StrH.Clr();}
  TStr GetStr(const TStr& Str);
};


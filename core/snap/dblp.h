#ifndef snap_dblp
#define snap_dblp

#include "Snap.h"

/////////////////////////////////////////////////
// Load DBLP XML
class TDblpLoader {
private:
  PSIn SIn;
  TXmlLx XmlLx;
  TStrSet StartTag;
  TChA TagNm, TagVal;
public:
  TChAV AuthorV;
  TChA Title;
  TInt Year;
public:
  TDblpLoader(const TStr& FNm);
  TStr GetFPosStr() const { return XmlLx.GetFPosStr(); }
  bool Next();
};


//

#endif

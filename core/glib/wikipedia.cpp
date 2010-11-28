/////////////////////////////////////////////////
// Includes
#include "wikipedia.h"

/////////////////////////////////////////////////
// Wikipedia-Document
PWikiDoc LoadXml(const PXmlDoc& XmlDoc){
  return NULL;
}

/////////////////////////////////////////////////
// Wikipedia-Base
PWikiBs TWikiBs::LoadBin(const TStr& FNm){
  return NULL;
}

void TWikiBs::SaveBin(const TStr& FNm){
}

PWikiBs TWikiBs::LoadXml(const TStr& FNm){
  PSIn SIn=TFIn::New(FNm);
  TXmlDoc::SkipTopTag(SIn);
  PXmlDoc XmlDoc; int XmlDocs=0;
  forever{
    // load xml tree
    XmlDocs++;
    if (XmlDocs%1000==0){
      printf("%d Docs\r", XmlDocs);
    }
    XmlDoc=TXmlDoc::LoadTxt(SIn);
    if (!XmlDoc->IsOk()){break;}
  }
  return NULL;
}



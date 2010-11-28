#include "stdafx.h"
#include "dblp.h"

/////////////////////////////////////////////////
// Load DBLP XML
TDblpLoader::TDblpLoader(const TStr& FNm) : SIn(TZipIn::IsZipFNm(FNm)?TZipIn::New(FNm):TFIn::New(FNm)), XmlLx(SIn,xspTruncate) {
  TStrV V; TStr("article|inproceedings|proceedings|book|incollection|phdthesis|mastersthesis|www").SplitOnAllCh('|', V);
  for (int i = 0; i < V.Len(); i++) { StartTag.AddKey(V[i]); }
  TStr("Agrave,A,Aacute,A,Acirc,A,Atilde,A,Auml,Ae,Aring,A,AElig,AE,Ccedil,C,Egrave,E,Eacute,E,Ecirc,E,Euml,E,Igrave,I,Iacute,I,Icirc,I,Iuml,Ie,ETH,E,Ntilde,N,Ograve,O,Oacute,O,Ocirc,O,Otilde,O,Ouml,Oe,Oslash,O,Ugrave,U,Uacute,U,Ucirc,U,Uuml,Ue,Yacute,Y,THORN,TH,szlig,s,agrave,a,aacute,a,acirc,a,atilde,a,auml,ae,aring,a,aelig,a,ccedil,c,egrave,e,eacute,e,ecirc,e,euml,e,igrave,i,iacute,i,icirc,i,iuml,ie,eth,e,ntilde,n,ograve,o,oacute,o,ocirc,o,otilde,o,ouml,oe,oslash,o,ugrave,u,uacute,u,ucirc,u,uuml,ue,yacute,y,thorn,t,yuml,ye,quot,\",reg,(R),micro,u,times,*").SplitOnAllCh(',', V);
  for (int i = 0; i < V.Len(); i+=2) {
    XmlLx.PutEntityVal(V[i], V[i+1]);
  }
}

bool TDblpLoader::Next() {
  while(XmlLx.GetSym() != xsyEof) {
    if (! (XmlLx.Sym==xsySTag && StartTag.IsKey(XmlLx.TagNm))) {
      while (XmlLx.GetSym()!=xsyEof && ! (XmlLx.Sym==xsySTag && StartTag.IsKey(XmlLx.TagNm))) { }
      if (XmlLx.Sym == xsyEof) { return false; }
    }
    AuthorV.Clr(false);
    Title.Clr();
    Year = -1;
    while (true) {
      TStrUtil::GetXmlTagNmVal2(XmlLx, TagNm, TagVal, true);
      if (TagNm == "author" || TagNm=="editor") { AuthorV.Add(TagVal); }
      else { break; }
    }
    int Found = 0;
    while (true) {
      if (TagNm == "title") { Title = TagVal; Found+=1; }
      if (TagNm == "year") { Year = TStr(TagVal).GetInt(); Found+=1; }
      if (Found == 2) { break; }
      if (! TStrUtil::GetXmlTagNmVal2(XmlLx, TagNm, TagVal, true)) { break; }
    } 
    return true;
  }
  return false;
}

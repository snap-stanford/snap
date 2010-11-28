/////////////////////////////////////////////////
// Includes
#include "roget.h"

/////////////////////////////////////////////////
// Roget-Base
void TRBase::LoadArtfl(const TStr& WebBaseFPath){
  PWebBase WebBase=PWebBase(new TWebMemBase(WebBaseFPath));
  int WebPgP=WebBase->FFirstWebPg(); int WebPgId;
  while (WebBase->FNextWebPg(WebPgP, WebPgId)){
    TStr UrlStr=WebBase->GetUrlStr(WebPgId);
    static TStr RgShStr="RG.sh"; if (!UrlStr.IsStrIn(RgShStr)){continue;}
//    if (!UrlStr.IsStrIn("RG.sh?^544\\")){continue;}

    PWebPg WebPg=WebBase->GetWebPg(WebPgId);
    PSIn SIn=TStrIn::New(WebPg->GetBodyStr());
    PHtmlDoc HtmlDoc=THtmlDoc::New(SIn, hdtAll);
    int TokN=0; PHtmlTok Tok; THtmlLxSym Sym; TStr Str;

    // move to <h2>
    do {HtmlDoc->GetTok(TokN++, Sym, Str);
    } while (!((Sym==hlsyBTag)&&(Str==THtmlTok::H2TagNm)));

    // parse "ddd[A|B]."
    TChA CtgNm; TChA CtgIdNm;
    HtmlDoc->GetTok(TokN++, Sym, Str);
    IAssert(Sym==hlsyNum); CtgNm+=Str; CtgIdNm+=Str;
    HtmlDoc->GetTok(TokN++, Sym, Str);
    if (Sym==hlsyStr){
      IAssert((Str=='A')||(Str=='B')); CtgNm+=Str; CtgIdNm+=Str;
      HtmlDoc->GetTok(TokN++, Sym, Str);
    }
    IAssert((Sym==hlsySSym)&&(Str=='.')); CtgNm+=Str;

    // parse to </h2>"
    TChA BracketStr;
    HtmlDoc->GetTok(TokN++, Sym, Str);
    while (!((Sym==hlsyETag)&&(Str==THtmlTok::H2TagNm))){
      if ((Sym==hlsySSym)&&(Str=='[')){
        HtmlDoc->GetTok(TokN++, Sym, Str);
        while (!((Sym==hlsySSym)&&(Str==']'))){
          if ((!BracketStr.Empty())&&(Sym==hlsyStr)){BracketStr+=' ';}
          BracketStr+=Str; HtmlDoc->GetTok(TokN++, Sym, Str);
        }
        BracketStr.Ins(0, " ["); BracketStr+=']';
      } else {
        if (Sym==hlsyStr){CtgNm+=' ';}
        CtgNm+=Str;
      }
      HtmlDoc->GetTok(TokN++, Sym, Str);
    }
    CtgNm+=BracketStr;
    TNotify::OnNotify(Notify, ntInfo, CtgNm);

    // parse words
    static TStr AdjStr="ADJ"; static TStr AdvStr="ADV";
    static TStr IntStr="INT"; static TStr PgStr="PAGE";
    static TStr PhrStr="PHR"; static TStr PrefStr="PREF";
    static TStr PronStr="PRON";
    HtmlDoc->GetTok(TokN++, Sym, Str);
    IAssert((Sym==hlsyStr)&&((Str=='N')||(Str==AdvStr)));
    while (TokN<HtmlDoc->GetToks()){
      if (Sym==hlsyStr){
        if (Str==PhrStr){break;}
        if ((Str!='N')&&(Str!='V')&&(Str!=AdjStr)&&(Str!=AdvStr)&&
         (Str!=IntStr)&&(Str!=PrefStr)&&(Str!=PronStr)){
          TChA WordStr;
          do {
            if (!WordStr.Empty()){WordStr+=' ';} WordStr+=Str;
            HtmlDoc->GetTok(TokN++, Sym, Str);
          } while (Sym==hlsyStr);
//          TNotify::OnNotify(Notify, ntInfo, WordStr);
        } else {
          HtmlDoc->GetTok(TokN++, Sym, Str);
        }
      } else
      if (Sym==hlsySSym){
        TStr ExpectStr;
        if (Str=='('){ExpectStr=')';}
        else if (Str=='['){ExpectStr=']';}
        else if (Str=='{'){ExpectStr='}';}
        else if (Str=='"'){ExpectStr='"';}
        if (!ExpectStr.Empty()){
          do {HtmlDoc->GetTok(TokN++, Sym, Str);
          } while (!((Sym==hlsySSym)&&(Str==ExpectStr)));
        }
        HtmlDoc->GetTok(TokN++, Sym, Str);
      } else {
        HtmlDoc->GetTok(TokN++, Sym, Str);
      }
    }
  }
}

TRBase::TRBase(const PNotify& _Notify):
  CtgV(2000, 0), WordToCtgIdH(1000), Notify(_Notify){
  LoadArtfl("RogetARTFL");
}


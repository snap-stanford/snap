#ifndef cgi_h
#define cgi_h

/////////////////////////////////////////////////
// Includes
#include "base.h"

/////////////////////////////////////////////////
// Cgi-Input
class TCgiIn{
private:
  TStrStrVH KeyNmToValVH;
  TStrV KeyNmV;
public:
  TCgiIn();
  ~TCgiIn(){}

  TCgiIn& operator=(const TCgiIn&){Fail; return *this;}

  int GetKeys() const {return KeyNmV.Len();}
  int GetKeyN(const TStr& KeyNm) const {return KeyNmV.SearchForw(KeyNm);}
  TStr GetKeyNm(const int& KeyN) const {return KeyNmV[KeyN];}
  bool IsKey(const TStr& KeyNm) const { return KeyNmToValVH.IsKey(KeyNm); } //J: 

  int GetVals(const int& KeyN) const {
    return KeyNmToValVH.GetDat(KeyNmV[KeyN]).Len();}
  int GetVals(const TStr& KeyNm) const {
    return KeyNmToValVH.GetDat(KeyNm).Len();}

  TStr GetVal(const int& KeyN, const int& ValN=0) const {
    return KeyNmToValVH.GetDat(KeyNmV[KeyN])[ValN];}
  TStr GetVal(const TStr& KeyNm, const int& ValN=0) const {
    if (KeyNmToValVH.IsKey(KeyNm)){return KeyNmToValVH.GetDat(KeyNm)[ValN];}
    else {return TStr();}}
  int GetValInt(const TStr& KeyNm, const int& DfVal=0) const {
    int Int; if (GetVal(KeyNm).IsInt(Int)){return Int;} else {return DfVal;}}

  TStr GetAcceptStr() const {return Env.GetVarVal("HTTP_ACCEPT");}
  TStr GetCookieStr() const {return Env.GetVarVal("HTTP_COOKIE");}
  TStr GetUserAgentNm() const {return Env.GetVarVal("HTTP_USER_AGENT");}
  TStr GetHostNm() const {return Env.GetVarVal("HTTP_HOST");}
  TStr GetRemoteHostNm() const {return Env.GetVarVal("REMOTE_HOST");}
  TStr GetRemoteAddrStr() const {return Env.GetVarVal("REMOTE_ADDR");}
  TStr GetQueryStr() const {return Env.GetVarVal("QUERY_STRING");}

  void GetCookieNmValKdV(TStrKdV& CookieNmValKdV) const;
  TStr GetCookieVal(const TStr& CookieNm) const;
};

/////////////////////////////////////////////////
// Cgi-Output
typedef enum {ccsDf, ccsLatin1, ccsLatin2} TCgiOutChSet;
typedef enum {chDf, chLeft, chCenter, chRight} TCgiOutHAlign;
typedef enum {cvDf, cvTop, cvMiddle, cvBottom} TCgiOutVAlign;

class TCgiOut{
private:
  bool HtmlMimeP;
  TStr BaseUrlStr;
  UndefDefaultCopyAssign(TCgiOut);
public:
  TCgiOut(const TStr& Title, const TCgiOutChSet& ChSet=ccsDf,
   const TStr& CssUrl=TStr(), const TStr& BgUrl=TStr());
  TCgiOut(const TStr& MimeTypeNm, const PSIn& SIn);
  ~TCgiOut();

  void PutBaseUrlStr(const TStr& _BaseUrlStr){BaseUrlStr=_BaseUrlStr;}
  TStr GetBaseUrlStr() const {return BaseUrlStr;}

  void Put(const TStr& Str){printf("%s", Str.CStr());}
  void PutLn(const TStr& Str){printf("%s\n", Str.CStr());}

  void PutH(const TStr& HStr, const int& HN=1, const TStr& AlignStr="left"/*center*/){
    IAssert((1<=HN)&&(HN<=6)); printf("<H%d ALIGN=\"%s\">%s</H%d>\n",
     HN, AlignStr.CStr(), HStr.CStr(), HN);}
  TStr GetA(const TStr& UrlStr, const TStr& CapStr){
    return TStr("<a href=\"")+UrlStr()+"\">"+CapStr+"</a>";}
  void PutA(const TStr& UrlStr, const TStr& CapStr){
    printf("%s", GetA(UrlStr, CapStr).CStr());}
  void PutBR(){printf("<BR>\n");}
  void PutP(){printf("<P>\n");}
  void PutHR(const int& Size=1){printf("<HR SIZE=%d>\n", Size);}
  void PutFontSize(const int& Size=1){printf("<FONT SIZE=%d>\n", Size);}
  void PutFontSizeEnd(){printf("</FONT>");}

  void PutB(){printf("<B>");}
  void PutBEnd(){printf("</B>");}
  void PutI(){printf("<I>");}
  void PutIEnd(){printf("</I>");}
  void PutBlink(){printf("<Blink>");}
  void PutBlinkEnd(){printf("</Blink>");}
  void PutSmall(){printf("<Small>");}
  void PutSmallEnd(){printf("</Small>");}

  void PutForm(const TStr& Method, const TStr& Url){
    printf("<FORM METHOD=\"%s\" ACTION=\"%s\">\n", Method.CStr(), Url.CStr());}
  void PutFormEnd(){printf("</FORM>\n");}
  void PutFormText(
   const TStr& Hd, const TStr& Name, const TStr& Val=TStr(),
   const int& MxLen=-1, const int& Size=-1);
  void PutFormPwd(
   const TStr& Hd, const TStr& Name, const TStr& Val=TStr(),
   const int& MxLen=-1, const int& Size=-1);
  void PutFormHidden(const TStr& Name, const TStr& Val);
  void PutFormSubmit(const TStr& Val){
    printf("<INPUT TYPE=\"SUBMIT\" VALUE=\"%s\">\n", Val.CStr());}
  void PutFormReset(const TStr& Val){
    printf("<INPUT TYPE=\"RESET\" VALUE=\"%s\">\n", Val.CStr());}
  void PutFormSelect(
   const TStr& Hd, const TStr& Name, const TStrPrV& ValCapStrV);
  void PutFormRadio(const TStr& Hd, const TStr& Name, const TStr& Val,
   const bool& IsChecked=false);

  void PutFormTextArea(const TStr& Name, const int& Rows, const int& Cols){
    printf("<TEXTAREA NAME=\"%s\" ROWS=%d COLS=%d>\n",
     Name.CStr(), Rows, Cols);}
  void PutFormTextAreaEnd(){printf("</TEXTAREA>\n");}

  void PutTb(
   const TStr& Name=TStr(), const TCgiOutHAlign& HAlign=chDf,
   const int& Width=-1, const int& Border=-1,
   const int& CellSpc=-1, const int& CellPad=-1);
  void PutTbEnd(){printf("\n</TABLE>\n");}
  void PutTbRow(
   const TCgiOutHAlign& HAlign=chDf, const TCgiOutVAlign& VAlign=cvDf);
  void PutTbRowEnd(){printf("</TR>");}
  void PutTbColHd(const TStr& ColStr, const TCgiOutHAlign& HAlign=chDf);
  void PutTbColVal(const TStr& ColStr, const TCgiOutHAlign& HAlign=chDf);

  static TStr GetHAlignStr(const TCgiOutHAlign& HAlign);
  static TStr GetVAlignStr(const TCgiOutVAlign& VAlign);

  static TStr GetUrlArgStr(const TChA& ChA);
};

/////////////////////////////////////////////////
// Cgi-Plain-Output
class TCgiPlainOut{
public:
  TCgiPlainOut(){}
  ~TCgiPlainOut(){}

  TCgiPlainOut& operator=(const TCgiPlainOut&){Fail; return *this;}

  void Put(const TStr& Str);// {printf("%s", Str.CStr());}
};

/////////////////////////////////////////////////
// Cgi-Wml-Output
class TCgiWmlOut{
private:
  bool WmlDeclP;
public:
  TCgiWmlOut(const bool& DoCache=false, const bool& _WmlDeclP=true);
  ~TCgiWmlOut();

  TCgiWmlOut& operator=(const TCgiWmlOut&){Fail; return *this;}

  void Put(const TStr& Str) const {printf("%s", Str.CStr());}
  void PutWml(const TStr& Str) const {printf("%s", GetWmlStr(Str).CStr());}
  void Put(const int& Int) const {printf("%d", Int);}
  void PutLn(const TStr& Str=TStr()) const {printf("%s\n", Str.CStr());}
  void PutLnBr(const TStr& Str=TStr()) const {printf("%s <br/>\n", Str.CStr());}

  void Open_Template() const {printf("<template>\n");}
  void Close_Template() const {printf("</template>\n");}
  void PutPrevTpl() const {
    Open_Template(); Put_Do_Prev(); Close_Template();}

  void Open_Card(const TStr& Id, const TStr& Title) const {
    printf("<card id=\"%s\" title=\"%s\" newcontext=\"false\">\n", Id.CStr(), Title.CStr());}
  void Close_Card() const {printf("</card>\n");}

  void Open_P() const {printf("<p>\n");}
  void Open_PCenter() const {printf("<p align=\"center\">\n");}
  void Close_P() const {printf("</p>\n");}
  void Put_PCenter(const TStr& Str) const {
    Open_PCenter(); Put(Str); Close_P();}
  void Put_P(const TStr& Str) const {
    Open_P(); Put(Str); Close_P();}

  void Open_A(const TStr& Url) const {printf("<a href=\"%s\">", Url.CStr());}
  void Close_A() const {printf("</a>");}
  void Put_A(const TStr& Url, const TStr& Str) const {
    Open_A(Url); Put(Str); Close_A();}
  void Put_P_A(const TStr& Url, const TStr& Str) const {
    Open_P(); Open_A(Url); Put(Str); Close_A(); Close_P();}
  void Put_PCenter_A(const TStr& Url, const TStr& Str) const {
    Open_PCenter(); Open_A(Url); Put(Str); Close_A(); Close_P();}
  void Put_PCenter_B_A(const TStr& Url, const TStr& Str) const {
    Open_PCenter(); Open_B(); Open_A(Url); Put(Str);
    Close_A(); Close_B(); Close_P();}
  void Put_PCenter_Wml(const TStr& Str) const {
    Open_PCenter(); Put_B_Wml(Str); Close_P();}
  void Put_PCenter_B_Wml(const TStr& Str) const {
    Open_PCenter(); PutWml(Str); Close_P();}

  void Put_InputText(
   const TStr& Hd, const TStr& Nm, const int& Sz, const int& MxLen) const;
  void Put_InputFormated(
   const TStr& Hd, const TStr& Nm, const TStr& FormatStr,
   const int& Sz=10, const int& MxLen=10) const;
  void Put_Select(const TStr& Hd, const TStr& INm, const TStrV& OptStrV) const;
  void Put_P_InputText(
   const TStr& Hd, const TStr& Nm, const int& Sz, const int& MxLen) const {
    Open_P(); Put_InputText(Hd, Nm, Sz, MxLen); Close_P();}


  void Put_Do_Go(const TStr& Type, const TStr& Lbl, const TStr& HRef) const;
  void Put_Do_Prev(const TStr& Lbl="Back") const;

  void Put_NbSpace() const {printf("&nbsp;");}
  void Put_Br() const {printf("<br/>\n");}

  void Open_B() const {printf("<b>");}
  void Close_B() const {printf("</b>");}
  void Open_I() const {printf("<i>");}
  void Close_I() const {printf("</i>");}
  void Open_Small() const {printf("<small>");}
  void Close_Small() const {printf("</small>");}
  void Put_B_Wml(const TStr& Str) const {Open_B(); PutWml(Str); Close_B();}

  static TStr GetWmlStr(const TStr& Str);
};

#endif

/////////////////////////////////////////////////
// Includes
#include "stdafx.h"
#include "cgi.h"

/////////////////////////////////////////////////
// Cgi-Input
class TCgiInChRet{
  PSIn SIn;
  int Chs, ChN;
public:
  TCgiInChRet(const PSIn& _SIn, const int& _Chs):
    SIn(_SIn), Chs(_Chs), ChN(0){}
  TCgiInChRet& operator=(const TCgiInChRet&){Fail; return *this;}
  bool Eof(){return ChN==Chs;}
  char GetCh(){IAssert(ChN<Chs); ChN++; return SIn->GetCh();}
};

TCgiIn::TCgiIn():
  KeyNmToValVH(100), KeyNmV(100, 0){
  TStr RqMth=Env.GetVarVal("REQUEST_METHOD");

  PSIn SIn; int Chs=-1;
  if (RqMth.GetUc()=="GET"){
    TStr QueryStr=Env.GetVarVal("QUERY_STRING");
    SIn=TStrIn::New(QueryStr); Chs=QueryStr.Len();}
  else if (RqMth.GetUc()=="POST"){
    SIn=TSIn::StdIn; Chs=Env.GetVarVal("CONTENT_LENGTH").GetInt();}
  else {printf("Unknown CGI method!"); return;}
  TCgiInChRet ChRet(SIn, Chs);

  TChA KeyNm; TChA ValStr(10000);
  while (!ChRet.Eof()){
    char Ch; KeyNm.Clr(); ValStr.Clr();
    while ((!ChRet.Eof())&&((Ch=ChRet.GetCh())!='=')){
      switch (Ch){
        case '%':{
          char Ch1='0'; if (!ChRet.Eof()){Ch1=ChRet.GetCh();}
          char Ch2='0'; if (!ChRet.Eof()){Ch2=ChRet.GetCh();}
          KeyNm.AddCh(char(16*TCh::GetHex(Ch1)+TCh::GetHex(Ch2)));} break;
        case '+': KeyNm.AddCh(' '); break;
        case '&': KeyNm.AddCh('&'); break; // Fail; break;
        default: KeyNm.AddCh(Ch);
      }
    }
    IAssert(Ch=='=');
    while ((!ChRet.Eof())&&((Ch=ChRet.GetCh())!='&')){
      switch (Ch){
        case '%':{
          char Ch1='0'; if (!ChRet.Eof()){Ch1=ChRet.GetCh();}
          char Ch2='0'; if (!ChRet.Eof()){Ch2=ChRet.GetCh();}
          ValStr.AddCh(char(16*TCh::GetHex(Ch1)+TCh::GetHex(Ch2)));} break;
        case '+': ValStr.AddCh(' '); break;
        case '&': Fail; break;
        default: ValStr.AddCh(Ch);
      }
    }
    KeyNmToValVH.AddDat(KeyNm).Add(ValStr);
    KeyNmV.AddUnique(KeyNm);
  }
}

void TCgiIn::GetCookieNmValKdV(TStrKdV& CookieNmValKdV) const {
  CookieNmValKdV.Clr();
  TStr CookieStr=GetCookieStr().GetTrunc();
  while (!CookieStr.Empty()){
    TStr NmValStr; CookieStr.SplitOnCh(NmValStr, ';', CookieStr);
    TStr Nm; TStr Val; NmValStr.SplitOnCh(Nm, ';', Val);
    CookieNmValKdV.Add(TStrKd(Nm, Val));
  }
}

TStr TCgiIn::GetCookieVal(const TStr& CookieNm) const {
  TStrKdV CookieNmValKdV; GetCookieNmValKdV(CookieNmValKdV);
  int CookieN;
  if (CookieNmValKdV.IsIn(TStrKd(CookieNm), CookieN)){
    return CookieNmValKdV[CookieN].Dat;}
  else {return TStr();}
}

/////////////////////////////////////////////////
// Cgi-Output
TCgiOut::TCgiOut(
 const TStr& Title, const TCgiOutChSet& ChSet,
 const TStr& CssUrl, const TStr& BgUrl):
  HtmlMimeP(true){
  // content-type
  printf("Content-Type: text/html\r\n");
  //printf("Cache-Control: no-cache\r\n");
  printf("\r\n");
  // head
  printf("<HTML>\n<HEAD>\n  <TITLE>%s</TITLE>\n", Title.CStr());
  // charracter-set
  switch (ChSet){
    case ccsDf: break;
    case ccsLatin1: printf("  <META http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n"); break;
    case ccsLatin2: printf("  <META http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-2\">\n"); break;
    default: Fail;
  }
  printf("</HEAD>\n");
  // style-sheet
  if (!CssUrl.Empty()){
    printf("  <LINK href=\"%s\" type=\"text/css\" rel=\"stylesheet\">\n", CssUrl.CStr());
  }
  // body
  if (BgUrl.Empty()){printf("<BODY>\n");}
  else if (BgUrl[0]=='#'){printf("<BODY BGCOLOR=\"%s\">\n", BgUrl.CStr());}
  else {printf("<BODY BACKGROUND=\"%s\">\n", BgUrl.CStr());}
}

TCgiOut::TCgiOut(const TStr& MimeTypeStr, const PSIn& SIn):
  HtmlMimeP(false){
//  printf("Content-Type: %s\r\n\r\n", MimeTypeStr.CStr());
//  printf("Connection: keep-alive\r\n");
//  printf("Content-Type: audio/x-wav\r\n");
//  printf("Accept-Ranges: bytes\r\n");
//  printf("Last-Modified: Sun, 04 Oct 1998 03:03:58 GMT\r\n");
//  printf("Content-Length: 832124\r\n");
//  printf("\r\n");

  TChA HdChA;
  HdChA+="Content-Type: "; HdChA+=MimeTypeStr; HdChA+="\r\n";
  HdChA+="Accept-Ranges: bytes\r\n";
  HdChA+="Connection: close\r\n";
  HdChA+="Content-Length: "; HdChA+=TInt::GetStr(SIn->Len()); HdChA+="\r\n";
  HdChA+="\r\n";
  HANDLE StdOut=GetStdHandle(STD_OUTPUT_HANDLE);
  TMIn HdMIn(HdChA);
  TMIn DataMIn(*SIn);

  DWORD Chs;
  WriteFile(StdOut, HdMIn.GetBfAddr(), HdMIn.Len(), &Chs, NULL);
  WriteFile(StdOut, DataMIn.GetBfAddr(), DataMIn.Len(), &Chs, NULL);
}

TCgiOut::~TCgiOut(){
  if (HtmlMimeP){
    printf("\n</BODY>\n</HTML>");}
}

void TCgiOut::PutFormText(
 const TStr& Hd, const TStr& Name, const TStr& Val,
 const int& MxLen, const int& Size){
  printf("%s", Hd.CStr());
  printf("<INPUT TYPE=\"TEXT\" NAME=\"%s\"", Name.CStr());
  if (!Val.Empty()){printf(" VALUE=\"%s\"", Val.CStr());}
  if (MxLen!=-1){printf(" MAXLENGTH=%d", MxLen);}
  if (Size!=-1){printf(" SIZE=%d", Size);}
  printf(">");
}

void TCgiOut::PutFormPwd(
 const TStr& Hd, const TStr& Name, const TStr& Val,
 const int& MxLen, const int& Size){
  printf("%s", Hd.CStr());
  printf("<INPUT TYPE=\"PASSWORD\" NAME=\"%s\"", Name.CStr());
  if (!Val.Empty()){printf(" VALUE=\"%s\"", Val.CStr());}
  if (MxLen!=-1){printf(" MAXLENGTH=%d", MxLen);}
  if (Size!=-1){printf(" SIZE=%d", Size);}
  printf(">");
}

void TCgiOut::PutFormHidden(const TStr& Name, const TStr& Val){
  printf("<INPUT TYPE=\"HIDDEN\" NAME=\"%s\"", Name.CStr());
  if (!Val.Empty()){printf(" VALUE=\"%s\"", Val.CStr());}
  printf(">");
}

void TCgiOut::PutFormRadio(
 const TStr& Hd, const TStr& Name, const TStr& Val, const bool& IsChecked){
  printf("<INPUT TYPE=\"RADIO\"");
  printf(" NAME=\"%s\"", Name.CStr());
  printf(" VALUE=\"%s\"", Val.CStr());
  if (IsChecked){printf(" CHECKED");}
  printf(">");
  printf("%s", Hd.CStr());
}

void TCgiOut::PutFormSelect(
 const TStr& Hd, const TStr& Name, const TStrPrV& ValCapStrV){
  printf("%s", Hd.CStr());
  printf("<SELECT NAME=\"%s\">\n", Name.CStr());
  for (int ValCapStrN=0; ValCapStrN<ValCapStrV.Len(); ValCapStrN++){
    TStr Val=ValCapStrV[ValCapStrN].Val1;
    TStr Cap=ValCapStrV[ValCapStrN].Val2;
    printf("  <OPTION VALUE=\"%s\">%s\n", Val.CStr(), Cap.CStr());
  }
  printf("</select>\n");
}

void TCgiOut::PutTb(
 const TStr& Name, const TCgiOutHAlign& HAlign,
 const int& Width, const int& Border,
 const int& CellSpc, const int& CellPad){
  TChA ChA;
  ChA+="<TABLE";
  if (HAlign!=chDf){ChA+=" ALIGN="; ChA+=GetHAlignStr(HAlign);}
  if (Width!=-1){ChA+=" WIDTH=\""; ChA+=TInt::GetStr(Width); ChA+="%\"";}
  if (Border!=-1){ChA+=" BORDER="; ChA+=TInt::GetStr(Border);}
  if (CellSpc!=-1){ChA+=" CELLSPACING="; ChA+=TInt::GetStr(CellSpc);}
  if (CellPad!=-1){ChA+=" CELLPADDING="; ChA+=TInt::GetStr(CellPad);}
  ChA+=">";
  printf("%s\n", ChA.CStr());
  if (!Name.Empty()){
    printf("<CAPTION>%s</CAPTION>\n", Name.CStr());}
}

void TCgiOut::PutTbRow(
 const TCgiOutHAlign& HAlign, const TCgiOutVAlign& VAlign){
  TChA ChA;
  ChA+="<TR";
  if (HAlign!=chDf){ChA+=" ALIGN="; ChA+=GetHAlignStr(HAlign);}
  if (VAlign!=chDf){ChA+=" VALIGN="; ChA+=GetVAlignStr(VAlign);}
  ChA+=">";
  printf("\n%s", ChA.CStr());
}

void TCgiOut::PutTbColHd(const TStr& ColStr, const TCgiOutHAlign& HAlign){
  TChA ChA;
  ChA+="<TH";
  if (HAlign!=chDf){ChA+=" ALIGN="; ChA+=GetHAlignStr(HAlign);}
  ChA+=">";
  printf("%s %s ", ChA.CStr(), ColStr.CStr());
}

void TCgiOut::PutTbColVal(const TStr& ColStr, const TCgiOutHAlign& HAlign){
  TChA ChA;
  ChA+="<TD";
  if (HAlign!=chDf){ChA+=" ALIGN="; ChA+=GetHAlignStr(HAlign);}
  ChA+=">";
  printf("%s %s ", ChA.CStr(), ColStr.CStr());
}

TStr TCgiOut::GetHAlignStr(const TCgiOutHAlign& HAlign){
  switch (HAlign){
    case chLeft: return "LEFT";
    case chCenter: return "CENTER";
    case chRight: return "RIGHT";
    default: Fail; return TStr();
  }
}

TStr TCgiOut::GetVAlignStr(const TCgiOutVAlign& VAlign){
  switch (VAlign){
    case cvTop: return "TOP";
    case cvMiddle: return "MIDDLE";
    case cvBottom: return "BOTTOM";
    default: Fail; return TStr();
  }
}

TStr TCgiOut::GetUrlArgStr(const TChA& ChA){
  TChA ArgChA;
  for (int ChN=0; ChN<ChA.Len(); ChN++){
    char Ch=ChA[ChN];
    if (TCh::IsAlNum(Ch)||/*(Ch==':')||*/
     (Ch=='/')||(Ch=='.')||(Ch=='_')||(Ch=='-')){
      ArgChA+=Ch;
    } else {
      ArgChA+='%'; ArgChA+=TStr(Ch).GetHex();
    }
  }
  return ArgChA;
}

/////////////////////////////////////////////////
// Cgi-Plain-Output
void TCgiPlainOut::Put(const TStr& Str){
  HANDLE StdOut=GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD Chs;
  WriteFile(StdOut, Str.CStr(), Str.Len(), &Chs, NULL);
}

/////////////////////////////////////////////////
// Cgi-Wml-Output
TCgiWmlOut::TCgiWmlOut(const bool& DoCache, const bool& _WmlDeclP):
  WmlDeclP(_WmlDeclP){
  printf("Set-Cookie: id=123\r\n");
  if (!DoCache){
    printf("Cache-Control: no-cache\r\n");}
  printf("Content-Type: text/vnd.wap.wml\r\n");
  printf("\r\n");
  if (WmlDeclP){
    printf("<?xml version=\"1.0\"?>\r\n");
    printf("<!DOCTYPE wml PUBLIC \"-//WAPFORUM//DTD WML 1.1//EN\" \"http://www.wapforum.org/DTD/wml_1.1.xml\">\r\n");
    printf("<wml>\r\n");
  }
}

TCgiWmlOut::~TCgiWmlOut(){
  if (WmlDeclP){
    printf("</wml>");}
}

void TCgiWmlOut::Put_InputText(
 const TStr& Hd, const TStr& Nm, const int& Sz, const int& MxLen) const {
  printf("%s<input name=\"%s\" type=\"text\"",
   Hd.CStr(), Nm.CStr());
  printf(" size=\"%d\" maxlength=\"%d\"/>\n", Sz, MxLen);
}

void TCgiWmlOut::Put_InputFormated(
 const TStr& Hd, const TStr& Nm, const TStr& FormatStr,
 const int& Sz, const int& MxLen) const {
  printf("%s<input name=\"%s\" type=\"text\" format=\"%s\"",
   Hd.CStr(), Nm.CStr(), FormatStr.CStr());
  printf(" size=\"%d\" maxlength=\"%d\"/>\n", Sz, MxLen);
}

void TCgiWmlOut::Put_Select(const TStr& Hd, const TStr& INm, const TStrV& OptStrV) const {
  printf("%s<select iname=\"%s\">\n", Hd.CStr(), INm.CStr());
  for (int OptStrN=0; OptStrN<OptStrV.Len(); OptStrN++){
    printf("<option>%s</option>\n", OptStrV[OptStrN].CStr());
  }
  printf("</select>\n");
}

void TCgiWmlOut::Put_Do_Go(
 const TStr& Type, const TStr& Lbl, const TStr& HRef) const {
  printf("<do type=\"%s\" name=\"Tralala\" label=\"%s\">\n", Type.CStr(), Lbl.CStr());
  printf("  <go href=\"%s\"/>\n", HRef.CStr());
  printf("</do>\n");
}

void TCgiWmlOut::Put_Do_Prev(const TStr& Lbl) const {
  printf("<do type=\"prev\" label=\"%s\">\n", Lbl.CStr());
  printf("  <prev/>\n");
  printf("</do>\n");
}

TStr TCgiWmlOut::GetWmlStr(const TStr& Str){
  TChA ChA=Str; TChA WmlChA;
  for (int ChN=0; ChN<ChA.Len(); ChN++){
    char Ch=ChA[ChN];
    switch (Ch){
      case '"': WmlChA+="&quot;"; break;
      case '&': WmlChA+="&amp;"; break;
      case '<': WmlChA+="&lt;"; break;
      case '>': WmlChA+="&gt;"; break;
      default:
        if ((' '<=Ch)&&(Ch<='~')){WmlChA+=Ch;}
        else {WmlChA+="&#"; WmlChA+=TInt::GetStr(uchar(Ch)); WmlChA+=';';}
    }
  }
  return WmlChA;
}


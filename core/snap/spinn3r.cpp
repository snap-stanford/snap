#include "stdafx.h"
#include "spinn3r.h"

/////////////////////////////////////////////////
// Spinn3r RSS feed item (feed.getDelta api call)
// Load <item>
void TSpinn3rFeedItem::Clr() {
  PostTitleStr.Clr();
  PostUrlStr.Clr();
  PubTm = TSecTm();
  BlogUrlStr.Clr();
  BlogTitleStr.Clr();
  BlogDesc.Clr();
  BlogLang.Clr();
  ContentStr.Clr();
  LinkV.Clr(false);
}

bool TSpinn3rFeedItem::LoadItem(TXmlLx& XmlLx) {
  static const TSecTm BegOfTm(2007,10,30, 0, 0, 0);
  try {
    EAssert(XmlLx.TagNm == "item");
    PostTitleStr = TStrUtil::GetXmlTagVal(XmlLx, "title");
    // PostUrlStr;
    if (! TStrUtil::GetNormalizedUrl(TStrUtil::GetXmlTagVal(XmlLx, "link").ToLc(), TChA(), PostUrlStr)) {
      TExcept::Throw(TStr("Bad post url:")+PostUrlStr); }
    TStrUtil::GetXmlTagVal(XmlLx, "guid");
    const TChA T = TStrUtil::GetXmlTagVal(XmlLx, "pubDate");
    EAssert(T.IsSuffix("GMT")); // Sat, 10 Nov 2007 00:00:01 GMT
    PubTm = TSecTm(atoi(T.GetSubStr(12,15).CStr()), TTmInfo::GetMonthN(T.GetSubStr(8,10).CStr()),
      atoi(T.GetSubStr(5,6).CStr()), atoi(T.GetSubStr(17,18).CStr()),
      atoi(T.GetSubStr(20,21).CStr()), atoi(T.GetSubStr(23,24).CStr()));
    EAssert(PubTm > BegOfTm);
    BlogUrlStr = TStrUtil::GetXmlTagVal(XmlLx, "dc:source").ToLc();
    BlogTitleStr = TStrUtil::GetXmlTagVal(XmlLx, "weblog:title");
    // take care of empty URLs
    if (BlogUrlStr.Empty()) {
      const int Slash = PostUrlStr.SearchCh('/', 7);
      if (Slash > 7) { BlogUrlStr = PostUrlStr.GetSubStr(0, Slash-1); }
      else { BlogUrlStr = PostUrlStr; }
    }
    if (BlogUrlStr.Empty()) { TExcept::Throw("Empty blog url"); }
    if (PostUrlStr.Empty() || PostUrlStr.CountCh('/') < 3) { // post url has to be http://site/...
      TExcept::Throw(TStr::Fmt("Bad post url: %s\n", PostUrlStr.CStr())); }
    if (BlogTitleStr.Empty()) { BlogTitleStr="No Blog Title"; }
    if (PostTitleStr.Empty()) { PostTitleStr="No Post Title"; }
    // parse the rest
    BlogDesc = TStrUtil::GetXmlTagVal(XmlLx, "weblog:description");
    BlogLang = TStrUtil::GetXmlTagVal(XmlLx, "dc:lang");
    // skip till description
    // old: while (! (XmlLx.GetSym()==xsySTag && XmlLx.TagNm=="description")) { }
    while (XmlLx.GetSym()!=xsyEof) {
      if (XmlLx.Sym==xsySTag && XmlLx.TagNm=="description") { break; }
      if (XmlLx.Sym==xsyETag && XmlLx.TagNm=="item") { return false; }
    }
    if (XmlLx.Sym == xsyEof) { return false; }
    const TXmlLxSym NextSym = XmlLx.GetSym();
    ContentStr = XmlLx.TxtChA.ToLc();
    if (NextSym == xsyStr) { EAssert(XmlLx.GetSym() == xsyETag); }
    else { EAssert(NextSym == xsyETag); } // end tag
    EAssert(XmlLx.TagNm == "description");
    while (! (XmlLx.GetSym()==xsyETag && XmlLx.TagNm=="item")) { }
    // parse post links
    LinkV.Clr(false);
    const int Len = ContentStr.Len();
    int LinkBeg = 0, LinkEnd=0;
    while (true) {
      LinkBeg = ContentStr.SearchStr("href=", LinkBeg);
      if (LinkBeg == -1) { break; } // search till first ' ', '"' or '''
      LinkBeg += 5;
      for (LinkEnd=LinkBeg+2; LinkEnd<Len && ContentStr[LinkEnd]!='"' && ContentStr[LinkEnd]!=' ' && ContentStr[LinkEnd]!='\''; LinkEnd++) { }
      if (ContentStr[LinkBeg]=='"' || ContentStr[LinkBeg]=='\'' || ContentStr[LinkBeg]==' ') { LinkBeg++; }
      if (ContentStr.IsPrefix("&quot")) { LinkBeg += 5; }
      if (ContentStr.IsSuffix("&quot")) { LinkEnd -= 5; }
      TChA LinkStr;  TStrUtil::GetNormalizedUrl(ContentStr.GetSubStr(LinkBeg, LinkEnd-1), BlogUrlStr, LinkStr);
      if (! LinkStr.IsPrefix("http://")) { continue; }
      LinkBeg = LinkEnd;
      LinkV.Add(LinkStr);
    }
  }
  catch (PExcept Except){
    //ErrNotify(Except->GetStr());
    LinkV.Clr(true);
    return false;
  }
  return true;
}

/*void TSpinn3rFeedItem::ProcessPosts(const TStr& XmlFNmWc, int LoadN) {
  TFFile FFile(XmlFNmWc);  TStr FNm;  PSIn SIn;
  if (LoadN < 0) { LoadN = TInt::Mx; }
  TExeTm _ExeTm;
  StartProcess();
  for (int f = 1; FFile.Next(FNm); f++) {
    printf("*** FILE:  %s\n", FNm.GetFMid().CStr());
    PSIn SIn = TZipIn::IsZipExt(FNm.GetFExt()) ? PSIn(new TZipIn(FNm)) : PSIn(new TFIn(FNm));
    TXmlLx XmlLx(SIn, xspTruncate);
    StartProcFile(FNm);
    int _PostCnt=0, _GoodPostCnt=0, _LinkPostCnt=0, _LinksCnt=0; // per file counters
    while(XmlLx.GetSym() != xsyEof) {
      if (! (XmlLx.Sym==xsySTag && XmlLx.TagNm=="item")) {
        while (XmlLx.GetSym()!=xsyEof && ! (XmlLx.Sym==xsySTag && XmlLx.TagNm=="item")) { }
        if (XmlLx.Sym == xsyEof) { break; }
      }
      const bool IsGoodPost = LoadItem(XmlLx);
      if (IsGoodPost) {
        if (! LinkV.Empty()) { LinkPostCnt++;  _LinkPostCnt++; }
        LinksCnt += LinkV.Len();  _LinksCnt += LinkV.Len();
        GoodPostCnt++;  _GoodPostCnt++; }
      PostCnt++;  _PostCnt++;
      ProcessPost(IsGoodPost);
      if (_PostCnt % Kilo(10) == 0) {
        printf("\r%dk total: %dk [%s] ", _PostCnt/Kilo(1), PostCnt/Kilo(1), _ExeTm.GetStr()); }
      if (PostCnt >= LoadN) { break; }
    }
    printf("\n================================================================\n");
    printf("  file:  %s\n", FNm.GetFMid().CStr());
    printf("  time:  %s   ", _ExeTm.GetStr());
    printf("total: %s [%s]\n", TotExeTm.GetStr(), TExeTm::GetCurTm());
    printf("  all posts:        %d   total: %d\n", _PostCnt, PostCnt());
    printf("  good posts:       %d   total: %d\n", _GoodPostCnt, GoodPostCnt());
    printf("  posts with links: %d   total: %d\n", _LinkPostCnt, LinkPostCnt());
    printf("  links in posts:   %d   total: %d\n", _LinksCnt, LinksCnt());
    EndProcFile(FNm);
    fflush(stdout);  _ExeTm.Tick();
    if (PostCnt >= LoadN) { break; }
  }
  EndProcess();
}*/

bool TSpinn3rFeedItem::Next() {
  if (SIn.Empty() || SIn->Eof()) {
    printf("  new file");
    if (! FFile.Next(CurFNm)) { return false; }
    printf(" %s\n", CurFNm.GetFMid().CStr());
    SIn = TZipIn::IsZipExt(CurFNm.GetFExt()) ? PSIn(new TZipIn(CurFNm)) : PSIn(new TFIn(CurFNm));
    if (XmlLxPt != NULL) { delete XmlLxPt; }
    XmlLxPt = new TXmlLx(SIn, xspTruncate);
    //StartProcFile(CurFNm);
  }
  TXmlLx& XmlLx = *XmlLxPt;
  while(XmlLx.GetSym() != xsyEof) {
    if (! (XmlLx.Sym==xsySTag && XmlLx.TagNm=="item")) {
      while (XmlLx.GetSym()!=xsyEof && ! (XmlLx.Sym==xsySTag && XmlLx.TagNm=="item")) { }
      if (XmlLx.Sym == xsyEof) { return Next(); }
    }
    const bool IsGoodPost = LoadItem(XmlLx);
    if (IsGoodPost) { return true; }
  }
  return Next();
}

/////////////////////////////////////////////////
// Spinn3r full post item (permalink.getDelta api call)
// Load <item>
TSpinn3rFullItem::TSpinn3rFullItem(TSIn& SIn) : XmlLxPt(NULL) {
  Load(SIn);
}

TSpinn3rFullItem::TSpinn3rFullItem(const TSpinn3rFullItem& Item) : XmlLxPt(NULL) {
  *this = Item;
}

void TSpinn3rFullItem::Save(TSOut& SOut) const {
  PostTitleStr.Save(SOut);
  PostUrlStr.Save(SOut);
  PubTm.Save(SOut);
  BlogUrlStr.Save(SOut);
  BlogTitleStr.Save(SOut);
  BlogDesc.Save(SOut);
  BlogLang.Save(SOut);
  PageHtmlStr.Save(SOut);
  ContentStr.Save(SOut);
  LinkV.Save(SOut);
}

void TSpinn3rFullItem::Load(TSIn& SIn) {
  PostTitleStr.Load(SIn);
  PostUrlStr.Load(SIn);
  PubTm.Load(SIn);
  BlogUrlStr.Load(SIn);
  BlogTitleStr.Load(SIn);
  BlogDesc.Load(SIn);
  BlogLang.Load(SIn);
  PageHtmlStr.Load(SIn);
  ContentStr.Load(SIn);
  LinkV.Load(SIn);
}

TSpinn3rFullItem& TSpinn3rFullItem::operator = (const TSpinn3rFullItem& Item) {
  PostTitleStr = Item.PostTitleStr;
  PostUrlStr = Item.PostUrlStr;
  PubTm = Item.PubTm;
  BlogUrlStr = Item.BlogUrlStr;
  BlogTitleStr = Item.BlogTitleStr;
  BlogDesc = Item.BlogDesc;
  BlogLang = Item.BlogLang;
  PageHtmlStr = Item.PageHtmlStr;
  ContentStr = Item.ContentStr;
  LinkV = Item.LinkV;
  return *this;
}

void TSpinn3rFullItem::Clr() {
  PostTitleStr.Clr();
  PostUrlStr.Clr();
  PubTm = TSecTm();
  BlogUrlStr.Clr();
  BlogTitleStr.Clr();
  BlogDesc.Clr();
  BlogLang.Clr();
  PageHtmlStr.Clr();
  ContentStr.Clr();
  LinkV.Clr(false);
}

//See: http://code.google.com/p/spinn3r-client/wiki/Spinn3rFields
bool TSpinn3rFullItem::LoadItem(TXmlLx& XmlLx) {
  static const TSecTm BegOfTm(2008,07, 25, 0, 0, 0);
  static const TSecTm EndOfTm(2010,10, 1, 0, 0, 0);
  Clr();
  try {
    EAssert(XmlLx.TagNm == "item");
    PostTitleStr = TStrUtil::GetXmlTagVal(XmlLx, "title");
    if (! TStrUtil::GetNormalizedUrl(TStrUtil::GetXmlTagVal(XmlLx, "link").ToLc(), TChA(), PostUrlStr)) {
      TExcept::Throw(TStr("Bad post url:")+PostUrlStr); }
    TStrUtil::GetXmlTagVal(XmlLx, "guid");
    const TChA T = TStrUtil::GetXmlTagVal(XmlLx, "pubDate");  // Sat, 10 Nov 2007 00:00:01 GMT
    EAssert(T.IsSuffix("GMT"));
    PubTm = TSecTm(atoi(T.GetSubStr(12,15).CStr()), TTmInfo::GetMonthN(T.GetSubStr(8,10).CStr()),
      atoi(T.GetSubStr(5,6).CStr()), atoi(T.GetSubStr(17,18).CStr()),
      atoi(T.GetSubStr(20,21).CStr()), atoi(T.GetSubStr(23,24).CStr()));
    EAssert(PubTm > BegOfTm);
    BlogUrlStr = TStrUtil::GetXmlTagVal(XmlLx, "dc:source").ToLc();
    TChA TagNm; TStrUtil::GetXmlTagNmVal(XmlLx, TagNm, BlogTitleStr);
    if (TagNm != "weblog:title") { // after Dec5 there is a new tag <source:resource>
      BlogTitleStr = TStrUtil::GetXmlTagVal(XmlLx, "weblog:title"); }
    // take care of empty URLs
    if (BlogUrlStr.Empty()) {
      const int Slash = PostUrlStr.SearchCh('/', 7);
      if (Slash > 7) { BlogUrlStr = PostUrlStr.GetSubStr(0, Slash-1); }
      else { BlogUrlStr = PostUrlStr; }
    }
    if (BlogUrlStr.Empty()) { TExcept::Throw("Empty blog url"); }
    if (PostUrlStr.Empty() || PostUrlStr.CountCh('/') < 3) { // post url has to be http://site/...
      TExcept::Throw(TStr::Fmt("Bad post url: %s\n", PostUrlStr.CStr())); }
    if (BlogTitleStr.Empty()) { BlogTitleStr="No Blog Title"; }
    if (PostTitleStr.Empty()) { PostTitleStr="No Post Title"; }
    // parse the rest
    BlogDesc = TStrUtil::GetXmlTagVal(XmlLx, "weblog:description");
    BlogLang = TStrUtil::GetXmlTagVal(XmlLx, "dc:lang");
    // is tweet?
    if (! PostUrlStr.IsPrefix("http://twitter.com/")) { //normal post
      while (XmlLx.GetSym()!=xsyEof) {
        if (XmlLx.Sym==xsySTag && XmlLx.TagNm=="post:body") { 
          const TXmlLxSym NextSym = XmlLx.GetSym();
          ContentStr = XmlLx.TxtChA.ToLc();
          if (NextSym == xsyStr) { EAssert(XmlLx.GetSym() == xsyETag); }
          else { EAssert(NextSym == xsyETag); } // empty tag
          EAssert(XmlLx.TagNm == "post:body");
        }
        if (XmlLx.Sym==xsySTag && XmlLx.TagNm=="post:content_extract") { 
          const TXmlLxSym NextSym = XmlLx.GetSym();
          ContentStr = XmlLx.TxtChA.ToLc();
          if (NextSym == xsyStr) { EAssert(XmlLx.GetSym() == xsyETag); }
          else { EAssert(NextSym == xsyETag); } // empty tag
          EAssert(XmlLx.TagNm == "post:content_extract");
          break;
        }
        if (XmlLx.Sym==xsyETag && XmlLx.TagNm=="item") { 
          if (ContentStr.Len() > 5 && ContentStr.SearchCh('<')==-1) {
            //printf("\n***CONTENT: %s\n  %s\n", PostTitleStr.CStr(), ContentStr.CStr());
            break; 
          } 
          return false;
        } 
      }
    } else { // Tweet
      while (XmlLx.GetSym()!=xsyEof) {
        if (XmlLx.Sym==xsySTag && XmlLx.TagNm=="post:title") { 
          const TXmlLxSym NextSym = XmlLx.GetSym();
          ContentStr = XmlLx.TxtChA.ToLc();
          if (NextSym == xsyStr) { EAssert(XmlLx.GetSym() == xsyETag); }
          else { EAssert(NextSym == xsyETag); } // empty tag
          EAssert(XmlLx.TagNm == "post:title");
        }
        if (XmlLx.Sym==xsyETag && XmlLx.TagNm=="item") { break; }
      }
    }
    if (XmlLx.Sym == xsyEof) { return false; }
    // parse post links
    LinkV.Clr(false);
    const int Len = ContentStr.Len();
    int LinkBeg = 0, LinkEnd=0;
    TChA LinkStr;
    while (true) {
      LinkBeg = ContentStr.SearchStr("http://", LinkBeg);
      if (LinkBeg == -1) { break; } // search till first ' ', '"' or '''
      //LinkBeg += 5;
      for (LinkEnd=LinkBeg+2; LinkEnd<Len && ContentStr[LinkEnd]!='"' && ContentStr[LinkEnd]!='<' && ContentStr[LinkEnd]!=' ' && ContentStr[LinkEnd]!='\''; LinkEnd++) { }
      //if (LinkEnd >= Len) { break; }
      if (ContentStr[LinkBeg]=='"' || ContentStr[LinkBeg]=='\'' || ContentStr[LinkBeg]==' ') { LinkBeg++; }
      if (ContentStr.IsPrefix("&quot")) { LinkBeg += 5; }
      if (ContentStr.IsSuffix("&quot")) { LinkEnd -= 5; }
      TStrUtil::GetNormalizedUrl(ContentStr.GetSubStr(LinkBeg, LinkEnd-1), BlogUrlStr, LinkStr);
      LinkBeg = LinkEnd;
      LinkV.Add(LinkStr);
    }
    // atom:published
    while (XmlLx.GetSym()!=xsyEof) {
      if (XmlLx.Sym==xsySTag && XmlLx.TagNm=="atom:published") { break; }
      if (XmlLx.Sym==xsyETag && XmlLx.TagNm=="item") { return true; }
    }
    if (XmlLx.Sym == xsyEof) { return false; }
    const TXmlLxSym NextSym2 = XmlLx.GetSym();
    const TChA T2 = XmlLx.TxtChA.ToLc();//TStrUtil::GetXmlTagVal(XmlLx, "atom:published");  // Sat, 10 Nov 2007 00:00:01 GMT
    if (NextSym2 == xsyStr) { EAssert(XmlLx.GetSym() == xsyETag); }
    else { EAssert(NextSym2 == xsyETag); } // empty tag
    EAssert(XmlLx.TagNm == "atom:published");
    /*if (! T2.Empty()) { // RSS published time (too noisy)
      //printf("T2: %s\n", T2.CStr());
      TSecTm PubTm2 = TSecTm(atoi(T2.GetSubStr(0,3).CStr()), atoi(T2.GetSubStr(5,6).CStr()),
        atoi(T2.GetSubStr(8,9).CStr()), atoi(T2.GetSubStr(11,12).CStr()),
        atoi(T2.GetSubStr(14,15).CStr()), atoi(T2.GetSubStr(17,18).CStr()));
      static int X=0,Y=0;
	    if (PubTm2 > BegOfTm && PubTm2 < EndOfTm && fabs(double(PubTm2.GetAbsSecs())-double(PubTm.GetAbsSecs())) < 2*24*3600.0) {
        //printf("    %s\t%s\n", T2.CStr(), T.CStr());
        X++;
        PubTm = PubTm2;
      } else {
        //printf("T2: %s\t%s\n", T2.CStr(), T.CStr());
        Y++;
        printf("%d/%d = %f\n", Y, X, double(Y)/double(X+Y));
      }
      //printf("  %s\n", PubTm2.GetStr().CStr());
    } // */
    // skip the rest
    while (! (XmlLx.GetSym()==xsyETag && XmlLx.TagNm=="item")) { }
    //printf("E");
  }
  catch (PExcept Except){
    //printf("  %s\n", Except->GetStr().CStr());
    //SaveToErrLog(Except->GetStr().CStr());
    LinkV.Clr(true);
    return false;
  }
  return true;
}

void TSpinn3rFullItem::ProcessPosts(const TStr& XmlFNmWc, int LoadN) {
  TFFile FFile(XmlFNmWc);  TStr FNm;  PSIn SIn;
  if (LoadN < 0) { LoadN = TInt::Mx; }
  TExeTm _ExeTm;
  StartProcess();
  for (int f = 1; FFile.Next(FNm); f++) {
    printf("*** FILE:  %s\n", FNm.GetFMid().CStr());
    PSIn SIn = TZipIn::IsZipExt(FNm.GetFExt()) ? PSIn(new TZipIn(FNm)) : PSIn(new TFIn(FNm));
    TXmlLx XmlLx(SIn, xspTruncate);
    StartProcFile(FNm);
    int _PostCnt=0, _GoodPostCnt=0, _LinkPostCnt=0, _LinksCnt=0; // per file counters
    while(XmlLx.GetSym() != xsyEof) {
      if (! (XmlLx.Sym==xsySTag && XmlLx.TagNm=="item")) {
        while (XmlLx.GetSym()!=xsyEof && ! (XmlLx.Sym==xsySTag && XmlLx.TagNm=="item")) { }
        if (XmlLx.Sym == xsyEof) { break; }
      }
      const bool IsGoodPost = LoadItem(XmlLx);
      if (IsGoodPost) {
        if (! LinkV.Empty()) { LinkPostCnt++;  _LinkPostCnt++; }
        LinksCnt += LinkV.Len();  _LinksCnt += LinkV.Len();
        GoodPostCnt++;  _GoodPostCnt++; }
      PostCnt++;  _PostCnt++;
      ProcessPost(IsGoodPost);
      if (_PostCnt % Kilo(10) == 0) {
        //printf("\r%dk total: %d [%s] ", _PostCnt/Kilo(1), PostCnt/Kilo(1), _ExeTm.GetStr());
      }
      if (PostCnt >= LoadN) { break; }
    }
    printf("\n================================================================\n");
    printf("  file:  %s\n", FNm.GetFMid().CStr());
    printf("  time:  %s   ", _ExeTm.GetStr());
    printf("total: %s [%s]\n", TotExeTm.GetStr(), TExeTm::GetCurTm());
    printf("  all posts:        %d   total: %d\n", _PostCnt, PostCnt());
    printf("  good posts:       %d   total: %d\n", _GoodPostCnt, GoodPostCnt());
    printf("  posts with links: %d   total: %d\n", _LinkPostCnt, LinkPostCnt());
    printf("  links in posts:   %d   total: %d\n", _LinksCnt, LinksCnt());
    EndProcFile(FNm);
    fflush(stdout);  _ExeTm.Tick();
    if (PostCnt >= LoadN) { break; }
  }
  EndProcess();
}

bool TSpinn3rFullItem::Next() {
  if (SIn.Empty() || SIn->Eof()) {
	  if (FFile.Empty()) {
	    if (SIn.Empty()) { CurFNm = InFNmWc; }
	    else { return false; }
	  } else {
      if (! FFile->Next(CurFNm)) { return false; }
    }
    printf("\n*** %s\n", CurFNm.GetFMid().CStr());
    if (PostCnt>0) {
      printf("  posts %d, good %d, wLinks %d, Links %d [%s]\n\n",
        PostCnt(), GoodPostCnt(), LinkPostCnt(), LinksCnt(), ExeTm.GetStr());
    }
    SIn = TZipIn::IsZipExt(CurFNm.GetFExt()) ? PSIn(new TZipIn(CurFNm)) : PSIn(new TFIn(CurFNm));
    if (XmlLxPt != NULL) { delete XmlLxPt; }
    XmlLxPt = new TXmlLx(SIn, xspTruncate);
  }
  TXmlLx& XmlLx = *XmlLxPt;
  while(XmlLx.GetSym() != xsyEof) {
    if (! (XmlLx.Sym==xsySTag && XmlLx.TagNm=="item")) {
      while (true) { // try reading next symbol
        try {
          if (! (XmlLx.GetSym()!=xsyEof && ! (XmlLx.Sym==xsySTag && XmlLx.TagNm=="item"))) { break; }
        } catch (PExcept Except){ }
      }
      if (XmlLx.Sym == xsyEof) { return Next(); }
    }
    const bool IsGoodPost = LoadItem(XmlLx);
    //printf("%s: %s\n", IsGoodPost?"GOD":"BAD", ContentStr.CStr());
    PostCnt++;
    if (IsGoodPost) {
      GoodPostCnt++;  LinksCnt += LinkV.Len();
      if (! LinkV.Empty()) { LinkPostCnt++;  }
      return true;
    }
  }
  return Next();
}


/////////////////////////////////////////////////
// Spinner Quote Extractor
TQuoteExtractor::TQuoteExtractor(const bool& SaveContent) : DoSave(SaveContent), FOut(NULL) {
  if (! SaveContent) { printf("\n!!! Not saving the post content with quotes !!!\n"); }
}

TQuoteExtractor::~TQuoteExtractor() {
  if (FOut != NULL) { fclose(FOut);  FOut=NULL; }
  DumpStat();
}

void TQuoteExtractor::GetInsideQuotes() {
  char prev;
  QuoteV.Clr();
  for (char *e = PostText.CStr(); *e; ) {
    // beginning "
    while (*e && *e != '"') { e++; }
    if (! *e) { return; }
    prev = *(e-1);
    if (! TCh::IsWs(prev) && prev!='(') { e++;
    continue; }
    char* b = e+1;  e++;
    // end "
    while (*e && *e != '"') { e++; }
    prev = *(e-1);
    char next = *(e+1);
    if ((! (TCh::IsAlNum(prev) || prev=='.' || prev==',' || prev=='!' || prev=='?' || prev==':')) ||
      (! (TCh::IsWs(next) || next=='.' || next==',' || next=='!' || next=='?' || next==':'))) {
      // end quote is probably start of a next quote, continue till 1st line break (double space)
      char *newE = b;
      while(*newE && (*newE!=' ' || *(newE-1)!=' ')) { newE++; }
      if (newE < e && uint(newE-b) < 300) { e = newE;  }
      else { // quote longer than 300 chars, take only first sentence
        newE = b;
        while(*newE && *newE!='.' && *(newE)!='!'  && *(newE)!='?') { newE++; }
        if (newE < e) { e = newE; }
      }
    }
    if (uint(e-b) > 2) { // skip quotes shorter than 3 chars
      prev = *e;  *e=0;
      int alNum = 0;
      for (char *bb = b; *bb; bb++) {
        if (TCh::IsAlNum(*bb)) { alNum++; } }
      if (alNum > 0) { QuoteV.Add(b); }
      *e=prev;
    }
    e++;
  }
}

bool TQuoteExtractor::Next() {
  const bool IsOk = TSpinn3rFullItem::Next();
  PostText.Clr();  QuoteV.Clr();
  if (! IsOk) { return false; }
  if (ContentStr.Empty()) { NoContentCnt++;  return true; }
  TStrUtil::RemoveHtmlTags(ContentStr, PostText);
  if (! TStrUtil::IsLatinStr(PostText, 0.8)) { NoLatinCnt++;  return true; }
  GoodLatinCnt++;
  GetInsideQuotes();
  //PostText = TStrUtil::GetCleanStr(PostText);
  PostText = TStrUtil::GetCleanStr(PostText); // keep punctuations
  if (! QuoteV.Empty()) { QuotePostCnt++; }
  QuotesCnt += QuoteV.Len();
  for (int q = 0; q < QuoteV.Len(); q++) {
    QuoteV[q] = TStrUtil::GetCleanWrdStr(QuoteV[q]); }
  return true;
}

void TQuoteExtractor::ProcessPost(const bool& GoodItem) {
  if (! GoodItem) { BadPostCnt++;  return; }
  if (ContentStr.Empty()) { NoContentCnt++;  return; }
  TStrUtil::RemoveHtmlTags(ContentStr, PostText);
  if (! TStrUtil::IsLatinStr(PostText, 0.8)) { NoLatinCnt++;  return; }
  GoodLatinCnt++;
  GetInsideQuotes();
  PostText = TStrUtil::GetCleanWrdStr(PostText);
  if (! QuoteV.Empty()) { QuotePostCnt++; }
  QuotesCnt += QuoteV.Len();
  for (int q = 0; q < QuoteV.Len(); q++) {
    QuoteV[q] = TStrUtil::GetCleanWrdStr(QuoteV[q]); }
  OnQuotesExtracted(*this); // callback function to get the contents of TQuoteExtractor
  if (DoSave) {
    SaveToXml();
    SaveToBin();
  }
  //if (GoodLatinCnt % 10000 == 0) { DumpStat(); ExeTm.Tick(); }
}

void TQuoteExtractor::StartProcFile(const TStr& FNm) {
  // fullSpinnr-2008-09-01a.txt.gz --> fullSpinnr-2008-09-01
  CurInFNm = FNm;
  TStr FNmMid = FNm.GetFMid();
  int lastNumCh = FNmMid.Len()-1;
  while(lastNumCh>0 && ! TCh::IsNum(FNmMid[lastNumCh])) { lastNumCh--; }
  if (lastNumCh == 0) { lastNumCh = FNmMid.SearchCh('.')-1; }
  const TStr NewFNmPref = FNmMid.GetSubStr(0, lastNumCh);
  if (DoSave && NewFNmPref != OutFNmPref) {
    OutFNmPref = NewFNmPref;
    if (FOut != NULL) { fclose(FOut); }
    FOut = fopen(TStr::Fmt("%s.content", OutFNmPref.CStr()).CStr(), "wt");
    BinFOut = PSOut(new TFOut(TStr::Fmt("%s.contentBin", OutFNmPref.CStr())));
  }
}

void TQuoteExtractor::DumpStat() {
  printf("== %s\n", XmlLxPt!=NULL?XmlLxPt->GetFPosStr().CStr():CurInFNm.CStr());
  printf("    %d all posts, %d good posts, %d posts with links, %d links\n", PostCnt(), GoodPostCnt(), LinkPostCnt(), LinksCnt());
  printf("    %d bad posts, %d no content posts, %d no latin posts ==\n", BadPostCnt(), NoContentCnt(), NoLatinCnt());
  printf("    %d good latin posts, %d posts with quotes, %d uotes, %.2f quotes/post\n", GoodLatinCnt(), QuotePostCnt(), QuotesCnt(), double(QuotesCnt)/double(GoodLatinCnt));
  printf("    %.1f s/1000 posts, total time: %s\n\n", (float)ExeTm.GetSecs(), TotExeTm.GetStr());
}

void TQuoteExtractor::SaveToXml() {
  fprintf(FOut, "<post>\n");
  fprintf(FOut, "<pubDate>%s</pubDate>\n", TXmlLx::GetXmlStrFromPlainStr(PubTm.GetDtTmSortStr()).CStr());
  fprintf(FOut, "<postUrl>%s</postUrl>\n", TXmlLx::GetXmlStrFromPlainStr(PostUrlStr).CStr());
  fprintf(FOut, "<postTitle>%s</postTitle>\n", TXmlLx::GetXmlStrFromPlainStr(PostTitleStr).CStr());
  fprintf(FOut, "<blogUrl>%s</blogUrl>\n", TXmlLx::GetXmlStrFromPlainStr(BlogUrlStr).CStr());
  fprintf(FOut, "<blogTitle>%s</blogTitle>\n", TXmlLx::GetXmlStrFromPlainStr(BlogTitleStr).CStr());
  fprintf(FOut, "<content>%s</content>\n", TXmlLx::GetXmlStrFromPlainStr(PostText).CStr());
  for (int q = 0; q < QuoteV.Len(); q++) {
    fprintf(FOut, "<q>%s</q>\n", TXmlLx::GetXmlStrFromPlainStr(QuoteV[q]).CStr()); }
  for (int l = 0; l < LinkV.Len(); l++) {
    fprintf(FOut, "<l>%s</l>\n", TXmlLx::GetXmlStrFromPlainStr(LinkV[l]).CStr()); }
  fprintf(FOut, "</post>\n\n");
}

void TQuoteExtractor::SaveToBin() {
  PubTm.Save(*BinFOut);
  PostUrlStr.Save(*BinFOut);
  PostTitleStr.Save(*BinFOut);
  BlogUrlStr.Save(*BinFOut);
  BlogTitleStr.Save(*BinFOut);
  PostText.Save(*BinFOut);
  QuoteV.Save(*BinFOut);
  LinkV.Save(*BinFOut);
}


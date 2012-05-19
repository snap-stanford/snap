/////////////////////////////////////////////////
// Graph Utilities
void TGUtil::GetCdf(const TIntPrV& PdfV, TIntPrV& CdfV) {
  CdfV = PdfV;
  for (int i = 1; i < CdfV.Len(); i++) {
    CdfV[i].Val2 = CdfV[i-1].Val2 + CdfV[i].Val2; }
}

void TGUtil::GetCdf(const TFltPrV& PdfV, TFltPrV& CdfV) {
  CdfV = PdfV;
  for (int i = 1; i < CdfV.Len(); i++) {
    CdfV[i].Val2 = CdfV[i-1].Val2 + CdfV[i].Val2; }
}

void TGUtil::GetCdf(const TIntFltKdV& PdfV, TIntFltKdV& CdfV) {
  CdfV = PdfV;
  for (int i = 1; i < CdfV.Len(); i++) {
    CdfV[i].Dat = CdfV[i-1].Dat + CdfV[i].Dat; }
}

TIntPrV TGUtil::GetCdf(const TIntPrV& PdfV) {
  TIntPrV CdfV;
  GetCdf(PdfV, CdfV);
  return CdfV;
}

TFltPrV TGUtil::GetCdf(const TFltPrV& PdfV) {
  TFltPrV CdfV;
  GetCdf(PdfV, CdfV);
  return CdfV;
}

void TGUtil::GetCCdf(const TIntPrV& PdfV, TIntPrV& CCdfV) {
  CCdfV = PdfV;
  for (int i = CCdfV.Len()-2; i >= 0; i--) {
    CCdfV[i].Val2 = CCdfV[i+1].Val2 + CCdfV[i].Val2; }
}

void TGUtil::GetCCdf(const TFltPrV& PdfV, TFltPrV& CCdfV) {
  CCdfV = PdfV;
  for (int i = CCdfV.Len()-2; i >= 0; i--) {
    CCdfV[i].Val2 = CCdfV[i+1].Val2 + CCdfV[i].Val2; }
}

void TGUtil::GetCCdf(const TIntFltKdV& PdfV, TIntFltKdV& CCdfV) {
  CCdfV = PdfV;
  for (int i = CCdfV.Len()-2; i >= 0; i--) {
    CCdfV[i].Dat = CCdfV[i+1].Dat + CCdfV[i].Dat; }
}

TIntPrV TGUtil::GetCCdf(const TIntPrV& PdfV) {
  TIntPrV CCdfV;
  GetCCdf(PdfV, CCdfV);
  return CCdfV;
}

TFltPrV TGUtil::GetCCdf(const TFltPrV& PdfV) {
  TFltPrV CCdfV;
  GetCCdf(PdfV, CCdfV);
  return CCdfV;
}

void TGUtil::GetPdf(const TIntPrV& CdfV, TIntPrV& PdfV) {
  PdfV = CdfV;
  for (int i = PdfV.Len()-1; i > 0; i--) {
    PdfV[i].Val2 = PdfV[i].Val2 - PdfV[i-1].Val2; }
}

void TGUtil::GetPdf(const TFltPrV& CdfV, TFltPrV& PdfV) {
  PdfV = CdfV;
  for (int i = PdfV.Len()-1; i > 0; i--) {
    PdfV[i].Val2 = PdfV[i].Val2 - PdfV[i-1].Val2; }
}

void TGUtil::GetPdf(const TIntFltKdV& CdfV, TIntFltKdV& PdfV) {
  PdfV = CdfV;
  for (int i = PdfV.Len()-1; i > 0; i--) {
    PdfV[i].Dat = PdfV[i].Dat - PdfV[i-1].Dat; }
}

void TGUtil::Normalize(TFltPrV& PdfV) {
  double Sum = 0.0;
  for (int i = 0; i < PdfV.Len(); i++) {
    Sum += PdfV[i].Val2; }
  if (Sum <= 0.0) { return; }
  for (int i = 0; i < PdfV.Len(); i++) {
    PdfV[i].Val2 /= Sum; }
}

void TGUtil::Normalize(TIntFltKdV& PdfV) {
  double Sum = 0.0;
  for (int i = 0; i < PdfV.Len(); i++) {
    Sum += PdfV[i].Dat; }
  if (Sum <= 0.0) { return; }
  for (int i = 0; i < PdfV.Len(); i++) {
    PdfV[i].Dat /= Sum; }
}

void TGUtil::MakeExpBins(const TFltPrV& XYValV, TFltPrV& ExpXYValV, const double& BinFactor, const double& MinYVal) {
  TGnuPlot::MakeExpBins(XYValV, ExpXYValV, BinFactor, MinYVal);
}

void TGUtil::MakeExpBins(const TFltKdV& XYValV, TFltKdV& ExpXYValV, const double& BinFactor, const double& MinYVal) {
  TGnuPlot::MakeExpBins(XYValV, ExpXYValV, BinFactor, MinYVal);
}

void TGUtil::MakeExpBins(const TFltV& YValV, TFltV& ExpYValV, const double& BinFactor) {
  ExpYValV.Clr(true);
  int prevI=0;
  for (int i = 0; i < YValV.Len(); ) {
    ExpYValV.Add(YValV[i]);
    i = int(i*BinFactor);
    if (i==prevI) { i++; }
    prevI = i;
  }
}

void TGUtil::MakeExpBins(const TIntV& YValV, TIntV& ExpYValV, const double& BinFactor) {
  ExpYValV.Clr(true);
  int prevI=0;
  for (int i = 0; i < YValV.Len(); ) {
    ExpYValV.Add(YValV[i]);
    i = int(i*BinFactor);
    if (i==prevI) { i++; }
    prevI = i;
  }
}

/////////////////////////////////////////////////
// String helper functions and utilities
// get <TagNm>TagVal</TagNm>
TChA& TStrUtil::GetXmlTagVal(TXmlLx& XmlLx, const TChA& TagNm) {
  static TChA TagVal;
  EAssertR(XmlLx.GetSym() == xsySTag, TagNm);
  EAssertR(TagNm == XmlLx.TagNm.CStr(), TagNm);
  const TXmlLxSym NextSym = XmlLx.GetSym();
  TagVal = XmlLx.TxtChA;
  if (NextSym == xsyStr) {
    EAssertR(XmlLx.GetSym() == xsyETag, TagNm);
  } else {
    EAssertR(NextSym == xsyETag, TagNm); // empty tag
    //printf("  token: %s empty! %s\n", XmlLx.TagNm.CStr(), XmlLx.GetFPosStr().CStr());
  }
  EAssertR(XmlLx.TagNm == TagNm, TagNm);
  return TagVal;
}

// get <TagNm>TagVal</TagNm>
void TStrUtil::GetXmlTagNmVal(TXmlLx& XmlLx, TChA& TagNm, TChA& TagVal) {
  EAssertR(XmlLx.GetSym() == xsySTag, TagNm);
  TagNm = XmlLx.TagNm;
  const TXmlLxSym NextSym = XmlLx.GetSym();
  TagVal = XmlLx.TxtChA;
  if (NextSym == xsyStr) {
    EAssertR(XmlLx.GetSym() == xsyETag, TagNm);
  } else {
    EAssertR(NextSym == xsyETag, TagNm); // empty tag
    //printf("  token: %s empty! %s\n", XmlLx.TagNm.CStr(), XmlLx.GetFPosStr().CStr());
  }
}

// get <TagNm>*</TagNm> (can be many tags inbetween
bool TStrUtil::GetXmlTagNmVal2(TXmlLx& XmlLx, TChA& TagNm, TChA& TagVal, const bool& TakeTagNms) {
  if (XmlLx.GetSym() != xsySTag) {
    return false; }
  TagVal.Clr();
  TagNm = XmlLx.TagNm;
  //const TXmlLxSym NextSym = XmlLx.GetSym();
  while (XmlLx.Sym != xsyETag || XmlLx.TagNm != TagNm.CStr()) {
    if (TakeTagNms) {
      TagVal += XmlLx.TxtChA; }
    else if (XmlLx.Sym == xsyStr) {
      TagVal += XmlLx.TxtChA; }
    XmlLx.GetSym();
  }
  return true;
  //if (NextSym == xsyStr) {
  //  EAssertR(XmlLx.GetSym() == xsyETag, TagNm);
  //} else {
  //  EAssertR(NextSym == xsyETag, TagNm); // empty tag
  //  printf("  token: %s empty! %s\n", XmlLx.TagNm.CStr(), XmlLx.GetFPosStr().CStr());
  //}
}


// http://www.ijs.si/fdfd/blah.html --> www.ijs.si
TChA TStrUtil::GetDomNm(const TChA& UrlChA) {
  int EndSlash = UrlChA.SearchCh('/', 7)-1; // skip starting http://
  if (EndSlash > 0) {
    const int BegSlash = UrlChA.SearchChBack('/', EndSlash);
    if (BegSlash > 0) { return UrlChA.GetSubStr(BegSlash+1, EndSlash).ToLc(); }
    else { return UrlChA.GetSubStr(0, UrlChA.SearchCh('/', 0)-1).ToLc(); }
  } else {
    if (UrlChA.IsPrefix("http://")) { return UrlChA.GetSubStr(7, UrlChA.Len()-1).ToLc(); }
    EndSlash = UrlChA.SearchCh('/', 0);
    if (EndSlash > 0) { return UrlChA.GetSubStr(0, EndSlash-1).ToLc(); }
    else { return TChA(UrlChA).ToLc(); }
  }
}
// get domain name and also strip starting www.
TChA TStrUtil::GetDomNm2(const TChA& UrlChA) {
  TChA Dom = GetDomNm(UrlChA);
  if (Dom.IsPrefix("www.")) { return Dom.GetSubStr(4, TInt::Mx); }
  else { return Dom; }
}

int GetNthOccurence(const TChA& Url, const int& Count, const char Ch='/') {
  const char *c = Url.CStr();
  int cnt = 0;
  while (*c && cnt != Count) {
    if (*c == Ch) { cnt++; }
    c++;
  }
  return int(c-Url.CStr()-1);
}

// get website (GetDomNm2 or blog url)
TChA TStrUtil::GetWebsiteNm(const TChA& PostUrlStr) {
  TChA DomNm = TStrUtil::GetDomNm2(PostUrlStr);
  // http://blog.myspace.com/index.cfm?fuseaction=blog.view&friendid=141560&blogid=420009539
  if (DomNm == "blog.myspace.com") {
    return PostUrlStr.GetSubStr(7, GetNthOccurence(PostUrlStr, 2, '&')-1);
  }
  // For these websites take the domain name and 1st directory: http://blogs.msdn.com/squasta
  // http://blogs.msdn.com/squasta/archive/2008/08/11/annonces-microsoft-au-black-hat-2008.aspx
  // http://ameblo.jp/baptism/entry-10126216277.html
  // http://xfruits.com/fcuignet/?id=8793&clic=249862689&url=http%3a%2f%2fnews.google.com%2fnews%2furl%3fsa%3dt%26ct%3dfr%2f9-0%26fd%3dr%26url%3dhttp%3a%2f%2fwww.investir-en-tunisie.net%2fnews%2farticle.php%253fid%253d5026%26cid%3d1241943065%26ei%3doy6gslh9jzycxahkjfxucw%26usg%3dafqjcnen_bczqldodsyga6zps2axphxl3q
  // http://scienceblogs.com/grrlscientist/2008/08/reader_comments.php
  // http://blogs.sun.com/geertjan/entry/wicket_in_action_undoubtedly_the
  // http://blog.wired.com/gadgets/2008/08/apple-sells-60.html
  // http://weblogs.asp.net/mehfuzh/archive/2008/08/11/linqextender-1-4-enhanced-object-tracking.aspx
  // http://blogs.technet.com/plitpromicrosoftcom/archive/2008/08/11/nowa-karta-sim.aspx
  // http://blogs.guardian.co.uk/greenslade/2008/08/murdoch_aims_to_boost_subscrib.html
  // http://blogs.clarin.com/quimeykiltru/2008/8/11/mentira-mentira-creo
  // http://blogs.sun.com/geertjan/entry/wicket_in_action_undoubtedly_the
  // http://blog.wired.com/gadgets/2008/08/apple-sells-60.html
  // http://weblogs.asp.net/mehfuzh/archive/2008/08/11/linqextender-1-4-enhanced-object-tracking.aspx
  // http://blogs.technet.com/plitpromicrosoftcom/archive/2008/08/11/nowa-karta-sim.aspx
  // http://blogs.guardian.co.uk/greenslade/2008/08/murdoch_aims_to_boost_subscrib.html
  // http://blogs.clarin.com/quimeykiltru/2008/8/11/mentira-mentira-creo
  // http://blogs.zdnet.com/hardware/?p=2391
  // http://blogs.citypages.com/sports/2008/08/ufc_87_seek_and.php
  // http://voices.washingtonpost.com/achenblog/2008/08/no_medal_for_bush.html
  // http://blog.tv2.dk/ole.mork/entry254689.html
  // http://blogs.menomoneefallsnow.com/in_the_race/archive/2008/08/11/sometimes-it-s-about-how-you-play-the-game.asp
  // http://weblogs.baltimoresun.com/entertainment/midnight_sun/blog/2008/08/heidis_bad_break_with_dubai_pa.html
  // http://eonline.com/uberblog/b23076_youtubular_from_rickrolled_barackrolled.html?sid=rss_topstories&utm_source=eo
  if (DomNm=="blogs.msdn.com" || DomNm=="ameblo.jp" || DomNm=="xfruits.com" || DomNm=="scienceblogs.com" || DomNm=="blogs.sun.com"
    || DomNm=="blog.wired.com" || DomNm=="weblogs.asp.net" || DomNm=="blogs.technet.com" || DomNm=="blogs.guardian.co"
    || DomNm=="blogs.clarin.com" || DomNm=="blogs.sun.com" || DomNm=="blog.wired.com" || DomNm=="weblogs.asp.net"
    || DomNm=="blogs.technet.com" || DomNm=="blogs.guardian.com" || DomNm=="blogs.clarin.com" || DomNm=="blogs.zdnet.com"
    || DomNm=="blogs.citypages.com" || DomNm=="voices.washingtonpost.com" || DomNm=="blog.tv2.dk"
    || DomNm=="blogs.menomoneefallsnow.com" || DomNm=="weblogs.baltimoresun.com" || DomNm=="eonline.com") {
      return PostUrlStr.GetSubStr(7, GetNthOccurence(PostUrlStr, 4)-1);
  }
  // http://digg.com/submit?phase=2&amp;url=http://socialitelife.celebuzz.com/archive/2008/07/31/and_then_a_hero_came_along.php&amp;title=and
  // http://digg.com/general_sciences/mental_images_are_like_pictures_slide_show
  if (DomNm == "digg.com") {
    if (PostUrlStr.IsPrefix("http://digg.com/submit?")) {
      const int Url = PostUrlStr.SearchStr(";url=");
      if (Url != -1) {
        return GetWebsiteNm(PostUrlStr.GetSubStr(Url+5, PostUrlStr.SearchCh('&', Url+5))); }
    } else {
      return PostUrlStr.GetSubStr(7, GetNthOccurence(PostUrlStr, 4)-1); }
  }
  // For these websites take the domain name and 2 directories: http://bbc.co.uk/blogs/thereporters/
  // http://bbc.co.uk/blogs/thereporters/markdevenport/2008/08/back_to_porridge.html
  // http://nydailynews.com/blogs/subwaysquawkers/2008/08/anaheim-is-no-magic-kingdom-fo.html
  // http://newsbusters.org/blogs/p-j-gladnick/2008/08/11/sf-chronicle-writer-predicts-global-warming-shellfish-invas
  // http://nydailynews.com/blogs/subwaysquawkers/2008/08/anaheim-is-no-magic-kingdom-fo.html
  if (PostUrlStr.IsPrefix("http://nydailynews.com/blogs/") || PostUrlStr.IsPrefix("http://bbc.co.uk/blogs/")
    || PostUrlStr.IsPrefix("http://nydailynews.com/blogs/") || PostUrlStr.IsPrefix("http://newsbusters.org/blogs/")) {
    return PostUrlStr.GetSubStr(7, GetNthOccurence(PostUrlStr, 5)-1);
  }
  // http://feeds.feedburner.com/~r/adesblog/ ~3/361711640
  if (DomNm=="feeds.feedburner.com") {
    return PostUrlStr.GetSubStr(7, GetNthOccurence(PostUrlStr, 5)-1);
  }
  // http://groups.google.com/group/news.admin.net-abuse.sightings/browse_thread/thread/8452c47949453216/f07daa509b90295c?show_docid=f07daa509b90295c
  if (DomNm=="groups.google.com") {
    return PostUrlStr.GetSubStr(7, GetNthOccurence(PostUrlStr, 5)-1);
  }
  // http://news.google.com/news/url?sa=t&ct=us/20-0&fd=r&url=http://www.theobserver.ca/articledisplay.aspx%3fe%3d1151495&cid=0&ei=yswgsjpndpbi8atc9knacw&usg=afqjcnhrbg-nc9z6ymtqfkear3_npwqqxa
  if (DomNm=="news.google.com") { // redirect
    const int UrlPos = PostUrlStr.SearchStr("&url=");
    if (UrlPos != -1) {
      return GetWebsiteNm(PostUrlStr.GetSubStr(UrlPos+5, PostUrlStr.SearchCh('&', UrlPos+5))); }
  }
  // http://bloggrevyen.no/go/110340/http://blog.christergulbrandsen.com/2008/08/11/is-nationalism-the-only-way-to-de
  if (DomNm == "bloggrevyen.no") { // redirect
    const int Http2 = PostUrlStr.SearchStr("/http://");
    if (Http2!=-1) {
      return GetWebsiteNm(PostUrlStr.GetSubStr(Http2+1, PostUrlStr.Len()-1)); }
  }
  //http://us.rd.yahoo.com/dailynews/rss/search/urgent+care/sig=11phgb4tu/*http%3a//www.newswise.com/articles/view/543340/?sc=rsmn
  //http://ca.rd.yahoo.com/dailynews/rss/topstories/*http://ca.news.yahoo.com/s/reuters/080801/n_top_news/news_afgha
  if (DomNm.IsSuffix(".rd.yahoo.com")) {
    const int Http2 = PostUrlStr.SearchStr("/*");
    if (Http2!=-1) {
      return GetWebsiteNm(PostUrlStr.GetSubStr(Http2+9, PostUrlStr.Len()-1)); }
  }
  return DomNm;
}

/// Quick URL nomalization: Remove ending /, /index.html, etc. and strip starting www.
bool TStrUtil::GetNormalizedUrl(const TChA& UrlIn, const TChA& BaseUrl, TChA& UrlOut) {
  UrlOut = UrlIn;
  if (StripEnd(UrlIn, "/", UrlOut)) {}
  else if (StripEnd(UrlIn, "/index.html", UrlOut)) {}
  else if (StripEnd(UrlIn, "/index.htm", UrlOut)) {}
  else if (StripEnd(UrlIn, "/index.php", UrlOut)) {}
  if (! (UrlOut.IsPrefix("http://") || UrlOut.IsPrefix("ftp://"))) {
    // if UrlIn is relative url, try combine it with BaseUrl
    if (UrlIn.Empty() || ! (BaseUrl.IsPrefix("http://") || BaseUrl.IsPrefix("ftp://"))) {
      //printf("** Bad URL: base:'%s' url:'%s'\n", BaseUrl.CStr(), UrlIn.CStr());
      return false; }
    TChA Out;
    if (! GetNormalizedUrl(BaseUrl, TChA(), Out)) { return false; }
    if (UrlIn[0] != '/') { Out.AddCh('/'); }
    Out += UrlOut;
    UrlOut = Out;
  }
  // http://www. --> http://
  if (UrlOut.IsPrefix("http://www.")) {
    UrlOut = TChA("http://") + UrlOut.GetSubStr(11, TInt::Mx);
  }
  UrlOut.ToLc();
  return true;
}

bool TStrUtil::StripEnd(const TChA& Str, const TChA& SearchStr, TChA& NewStr) {
  const int StrLen = Str.Len();
  const int SearchStrLen = SearchStr.Len();
  if (StrLen < SearchStrLen) { return false; }
  for (int i = 0; i < SearchStrLen; i++) {
    if (Str[StrLen-i-1] != SearchStr[SearchStrLen-i-1]) { return false; }
  }
  NewStr = Str.GetSubStr(0, StrLen-SearchStrLen-1);
  return true;
}

TChA TStrUtil::GetShorStr(const TChA& LongStr, const int MaxLen) {
  if (LongStr.Len() < MaxLen) { return LongStr; }
  TChA Str = LongStr.GetSubStr(0, MaxLen-1);
  Str += "...";
  return Str;
}

// space separated sequence of words, remove all punctuations, etc.
TChA TStrUtil::GetCleanWrdStr(const TChA& ChA) {
  char *b = (char *) ChA.CStr();
  while (*b && ! TCh::IsAlNum(*b)) { b++; }
  if (*b == 0) { return TChA(); }
  TChA OutChA(ChA.Len());
  char *e = b, tmp;
  while (*e) {
    b = e;
    while (*e && (TCh::IsAlNum(*e) || ((*e=='\'' || *e=='-') && TCh::IsAlNum(*(e+1))))) { e++; }
    if (b < e) {
      tmp = *e; *e=0;
      OutChA += b;  OutChA.AddCh(' ');
      *e = tmp;
    }
    while (*e && ! TCh::IsAlNum(*e)) { e++; }
    if (! *e) { break; }
  }
  OutChA.DelLastCh();  OutChA.ToLc();
  return OutChA;
}

// space seprated sequence of words (includes all non-blank characters, i.e., punctuations)
TChA TStrUtil::GetCleanStr(const TChA& ChA) {
  char *b = (char *) ChA.CStr();
  while (*b && ! TCh::IsAlNum(*b)) { b++; }
  if (*b == 0) { return TChA(); }
  TChA OutChA(ChA.Len());
  char *e = b;
  bool ws=false;
  while (*e) {
    while (*e && TCh::IsWs(*e)) { e++; ws=true; }
    if (! *e) { break; }
    if (ws) { OutChA.AddCh(' '); ws=false; }
    OutChA.AddCh(*e);
    e++;
  }
  //OutChA.ToLc();
  return OutChA;
}
int TStrUtil::CountWords(const TChA& ChA) {
  return CountWords(ChA.CStr());
}

int TStrUtil::CountWords(const char* CStr) {
  int WrdCnt = 1;
  for (const char *c = CStr; *c; c++) {
    if (TCh::IsWs(*c)) { WrdCnt++; }
  }
  return WrdCnt;
}

int TStrUtil::CountWords(const TChA& ChA, const TStrHash<TInt>& StopWordH) {
  TChA Tmp;
  TVec<char *> WrdV;
  SplitWords(Tmp, WrdV);
  int SWordCnt = 0;
  for (int w = 0; w < WrdV.Len(); w++) {
    if (StopWordH.IsKey(WrdV[w])) { SWordCnt++; }
  }
  return WrdV.Len() - SWordCnt;
}

int TStrUtil::SplitWords(TChA& ChA, TVec<char *>& WrdV, const bool& SplitOnWs) {
  WrdV.Clr(false);
  WrdV.Add(ChA.CStr());
  for (char *c = (char *) ChA.CStr(); *c; c++) {
    if ((SplitOnWs && *c == ' ') || (! SplitOnWs && ! TCh::IsAlNum(*c))) {
      *c = 0;
      if (! WrdV.Empty() && strlen(WrdV.Last()) == 0) { WrdV.DelLast(); }
      WrdV.Add(c+1);
    }
  }
  return WrdV.Len();
}

int TStrUtil::SplitOnCh(TChA& ChA, TVec<char *>& WrdV, const char& Ch, const bool& SkipEmpty) {
  WrdV.Clr(false);
  WrdV.Add(ChA.CStr());
  for (char *c = (char *) ChA.CStr(); *c; c++) {
    if (*c == Ch) {
      *c = 0;
      if (SkipEmpty && ! WrdV.Empty() && strlen(WrdV.Last()) == 0) { WrdV.DelLast(); }
      WrdV.Add(c+1);
    }
  }
  if (SkipEmpty && ! WrdV.Empty() && strlen(WrdV.Last()) == 0) { WrdV.DelLast(); }
  return WrdV.Len();
}

int TStrUtil::SplitLines(TChA& ChA, TVec<char *>& LineV, const bool& SkipEmpty) {
  LineV.Clr(false);
  LineV.Add(ChA.CStr());
  bool IsChs=false;
  for (char *c = (char *) ChA.CStr(); *c; c++) {
    if (*c == '\n') {
      if (c > ChA.CStr() && *(c-1)=='\r') { *(c-1)=0; } // \r\n
      *c=0;
      if (SkipEmpty) {
        if (IsChs) { LineV.Add(c+1); }
      } else {
        LineV.Add(c+1);
      }
      IsChs=false;
    } else {
      IsChs=true;
    }
  }
  return LineV.Len();
}

int TStrUtil::SplitSentences(TChA& ChA, TVec<char *>& SentenceV) {
  SentenceV.Clr();
  const char *B = ChA.CStr();
  const char *E = B+ChA.Len();
  char *c = (char *) B;
  while (*c && TCh::IsWs(*c)) { c++; }
  if (*c) { SentenceV.Add(c); } else { return 0; }
  for (; c < E; c++) {
    if (c<E && (*c == '.' || *c == '!' || *c == '?') && ! TCh::IsAlNum(*(c+1))) { // end of sentence
      if (c<E && *(c+1)=='"') { *c='"';  c++; } // blah." --> blah"
      if (c>=E) { continue; }
      *c=0;  c++;
      char *e = c-1;
      while (e>B && *e!='"' && ! TCh::IsAlNum(*e)) { *e=0; e--; } // skip trailing non-alpha-num chars
      while (c<E && ! (TCh::IsAlNum(*c) || (*c=='"' && TCh::IsAlNum(*(c+1))))) { c++; } // sentence starts with AlNum or "AlNum
      if (c<E) { SentenceV.Add(c); }
    }
  }
  return SentenceV.Len();
}

void TStrUtil::RemoveHtmlTags(const TChA& HtmlStr, TChA& TextStr) {
  TextStr.Clr();
  char *StrB, *StrE;
  // use full page html: skip till <body>
  //PageHtmlStr = "<script fdsfs>  fsdfsd </script> jure";
  /*if (UseFullHtml) {
    StrB = PageHtmlStr.CStr();
    StrE = StrB+PageHtmlStr.Len();
    char * NewB = strstr(StrB, "<body>");
    if (NewB != NULL) { StrB = NewB+6; }
    char * NewE = strstr(StrB, "body>");
    if (NewE != NULL) {
      while (true) {
        char *E=strstr(NewE+4, "body>");
        if (E == NULL) { break; }  NewE = E; }
      StrE = NewE;
    }
  } else {  // only extracted post html*/
  StrB = (char *) HtmlStr.CStr();
  StrE = (char *) StrB+HtmlStr.Len(); //}
  for (char *e = StrB; e < StrE; ) {
    char* b = e;
    while (e<StrE && *e != '<') { e++; }
    // copy text
    char tmp=*e;  *e = 0;
    TextStr+= b; TextStr.AddCh(' ');  *e = tmp;
    if (e >= StrE) { return; }
    // if start of a comment: skip
    if (e[1]=='!' && e[2]=='-' && e[3]=='-') { // comment
      e += 3;
      while(e<StrE && !(*(e-2)=='-' && *(e-1)=='-' && *e=='>')) { e++; }
      e++;  continue;
    }
    // if "<script" then skip
    if (e[1]=='s' && e[2]=='c' && e[3]=='r' && e[4]=='i' && e[5]=='p' && e[6]=='t') {
      e += 5;
      while(e<StrE && !(*(e-6)=='s' && *(e-5)=='c' && *(e-4)=='r' && *(e-3)=='i' && *(e-2)=='p' && *(e-1)=='t' && *e=='>')) { e++; }
      e++;  continue;
    }
    // skip to end of tag
    while (e < StrE && *e != '>') { e++; }
    if (e>=StrE) { return; }
    e++;
  }
}

bool TStrUtil::IsLatinStr(const TChA& Str, const double& MinAlFrac) {
  int AlNumCnt=0, ChCnt=0;
  for (const char *c = Str.CStr(); *c; c++) {
    if (TCh::IsWs(*c)) { continue; }
    if (*c > 0 && TCh::IsAlNum(*c)) { AlNumCnt++; }
    ChCnt++;
  }
  if (double(AlNumCnt)/double(ChCnt) > MinAlFrac) { return true; }
  return false;
}

void TStrUtil::GetWIdV(const TStrHash<TInt>& StrH, const char *CStr, TIntV& WIdV) {
  const int NotWId = -1;
  TChA ChA(CStr);
  TVec<char *> WrdV;
  TInt WId;
  TStrUtil::SplitWords(ChA, WrdV);
  WIdV.Clr(false);
  for (int w = 0; w < WrdV.Len(); w++) {
    if (StrH.IsKeyGetDat(WrdV[w], WId)) { WIdV.Add(WId); }
    else { WIdV.Add(NotWId); }
  }
}

// and words to StrH and get a vector of word ids
void TStrUtil::GetAddWIdV(TStrHash<TInt>& StrH, const char *CStr, TIntV& WIdV) {
  TChA ChA(CStr);
  TVec<char *> WrdV;
  TInt WId;
  TStrUtil::SplitWords(ChA, WrdV);
  WIdV.Clr(false);
  for (int w = 0; w < WrdV.Len(); w++) {
    WIdV.Add(StrH.AddDatId(WrdV[w]));
  }
}

// Parse time in various formats:
//   10:16, 16 Sep 2004
//   10:20, 2004 Sep 16
//   2005-07-07 20:30:35
//   23:24:07, 2005-07-10
//   9 July 2005 14:38
//   21:16, July 9, 2005
//   06:02, 10 July 2005
bool TStrUtil::GetTmFromStr(const char* TmStr, TSecTm& Tm) {
  static TStrV MonthV1, MonthV2;
  if (MonthV1.Empty()) {
    TStr("january|february|march|april|may|june|july|august|september|october|november|december").SplitOnAllCh('|', MonthV1);
    TStr("jan|feb|mar|apr|may|jun|jul|aug|sep|oct|nov|dec").SplitOnAllCh('|', MonthV2);
  }
  TChA Tmp(TmStr);
  Tmp.ToLc();
  TVec<char *> WrdV;
  const char* End = Tmp.CStr()+Tmp.Len();
  int Col = -1, Cols=0;
  for (char *b = Tmp.CStr(); b <End; ) {
    WrdV.Add(b);
    while (*b && ! (*b==' ' || *b=='-' || *b==':' || *b==',')) { b++; }
    if (*b==':') { if(Col==-1) { Col=WrdV.Len(); } Cols++;  }
    *b=0; b++;
    while (*b && (*b==' ' || *b=='-' || *b==':' || *b==',')) { b++; }
  }
  if (Cols == 2) {
    if (Col+1 >= WrdV.Len()) { return false; }
    WrdV.Del(Col+1);
  }
  if (Col<1) { return false; }
  const int Hr = atoi(WrdV[Col-1]);
  const int Min = atoi(WrdV[Col]);
  WrdV.Del(Col);  WrdV.Del(Col-1);
  if (WrdV.Len() != 3) { return false; }
  int y=0,m=1,d=2, Mon=-1;
  if (TCh::IsAlpha(WrdV[0][0])) {
    y=2; m=0; d=1;
  } else if (TCh::IsAlpha(WrdV[1][0])) {
    y=2; m=1; d=0;
  } else if (TCh::IsAlpha(WrdV[2][0])) {
    y=0; m=2; d=1;
  } else {
    y=0; m=1; d=2;
    Mon = atoi(WrdV[m]);
  }
  int Day = atoi(WrdV[d]);
  if (Mon <= 0) { Mon = MonthV1.SearchForw(WrdV[m])+1; }
  if (Mon <= 0) { Mon = MonthV2.SearchForw(WrdV[m])+1; }
  if (Mon == 0) { return false; }
  int Year = atoi(WrdV[y]);
  if (Day > Year) { ::Swap(Day, Year); }
  //printf("%d-%02d-%02d  %02d:%02d\n", Year, Mon, Day, Hr, Min);
  Tm = TSecTm(Year, Mon, Day, Hr, Min, 0);
  return true;
}

// Standardize first and lastnames into <last_name>_<first name innitial>
TStr TStrUtil::GetStdName(TStr AuthorName) {
  TStr StdName;
  AuthorName.ToLc();
  AuthorName.ChangeChAll('\n', ' ');
  AuthorName.ChangeChAll('.', ' ');
  // if there is a number in the name, remove it and everything after it
  int i, pos = 0;
  while (pos<AuthorName.Len() && (AuthorName[pos]!='#' && !TCh::IsNum(AuthorName[pos]))) {
    pos++; }
  if (pos < AuthorName.Len()) {
    AuthorName = AuthorName.GetSubStr(0, pos-1).ToTrunc(); }
  if (AuthorName.Empty()) { return TStr::GetNullStr(); }

  // replace everything after '('
  int b = AuthorName.SearchCh('(');
  if (b != -1) {
    AuthorName = AuthorName.GetSubStr(0, b-1).ToTrunc(); }
  // skip if contains ')'
  if (AuthorName .SearchCh(')')!=-1) { return TStr::GetNullStr(); }
  // skip if it is not a name
  if (AuthorName .SearchStr("figures")!=-1 || AuthorName .SearchStr("macros")!=-1
   || AuthorName .SearchStr("univ")!=-1 || AuthorName .SearchStr("institute")!=-1) {
    return TStr::GetNullStr();
  }
  // remove all non-letters (latex tags, ...)
  TChA NewName;
  for (i = 0; i < AuthorName.Len(); i++) {
    const char Ch = AuthorName[i];
    if (TCh::IsAlpha(Ch) || TCh::IsWs(Ch) || Ch=='-') { NewName += Ch; }
  }
  StdName = NewName;  StdName.ToTrunc();
  TStrV AuthNmV; StdName.SplitOnWs(AuthNmV);
  // too short -- not a name
  if (! AuthNmV.Empty() && AuthNmV.Last() == "jr") AuthNmV.DelLast();
  if (AuthNmV.Len() < 2) return TStr::GetNullStr();

  const TStr LastNm = AuthNmV.Last();
  if (! TCh::IsAlpha(LastNm[0]) || LastNm.Len() == 1) return TStr::GetNullStr();

  IAssert(isalpha(AuthNmV[0][0]));
  return TStr::Fmt("%s_%c", LastNm.CStr(), AuthNmV[0][0]);
}

void TStrUtil::GetStdNameV(TStr AuthorNames, TStrV& StdNameV) {
  AuthorNames.ChangeChAll('\n', ' ');
  AuthorNames.ToLc();
  // split into author names
  TStrV AuthV, TmpV, Tmp2V;
  // split on 'and'
  AuthorNames.SplitOnStr(" and ", TmpV);
  int i;
  for (i = 0; i < TmpV.Len(); i++) {
    TmpV[i].SplitOnAllCh(',', Tmp2V);  AuthV.AddV(Tmp2V); }
  // split on '&'
  TmpV = AuthV;  AuthV.Clr();
  for (i = 0; i < TmpV.Len(); i++) {
    TmpV[i].SplitOnAllCh('&', Tmp2V);  AuthV.AddV(Tmp2V); }
  // split on ','
  TmpV = AuthV;  AuthV.Clr();
  for (i = 0; i < TmpV.Len(); i++) {
    TmpV[i].SplitOnAllCh(',', Tmp2V);  AuthV.AddV(Tmp2V); }
  // split on ';'
  TmpV = AuthV;  AuthV.Clr();
  for (i = 0; i < TmpV.Len(); i++) {
    TmpV[i].SplitOnAllCh(';', Tmp2V);  AuthV.AddV(Tmp2V); }
  // standardize names
  StdNameV.Clr();
  //printf("\n*** %s\n", AuthorNames.CStr());
  for (i = 0; i < AuthV.Len(); i++) {
    TStr StdName = GetStdName(AuthV[i]);
    if (! StdName.Empty()) {
      //printf("\t%s  ==>  %s\n", AuthV[i].CStr(), StdName.CStr());
      StdNameV.Add(StdName);
    }
  }
}

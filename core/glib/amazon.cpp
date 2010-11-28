/////////////////////////////////////////////////
// Includes
#include "amazon.h"

#include "net.h"

/////////////////////////////////////////////////
// Amazon-Item
TStr TAmazonItem::GetStr() const {
  TChA ChA;
  ChA+=ItemId;
  ChA+=' ';
  ChA+=TitleStr;
  for (int AuthorNmN=0; AuthorNmN<AuthorNmV.Len(); AuthorNmN++){
    if (AuthorNmN==0){ChA+=": ";} else {ChA+=", ";}
    ChA+=AuthorNmV[AuthorNmN];
  }
  return ChA;
}

PAmazonItem TAmazonItem::New(const PXmlDoc& XmlDoc){
  // create item
  PAmazonItem AmazonItem=PAmazonItem(new TAmazonItem());
  // item-id
  AmazonItem->ItemId=XmlDoc->GetTagTok("AmazonItem|ItemId")->GetTokStr(false);
  // title
  AmazonItem->TitleStr=XmlDoc->GetTagTok("AmazonItem|Title")->GetTokStr(false);
  // authors
  TXmlTokV AuthorNmTokV;
  XmlDoc->GetTagTokV("AmazonItem|Authors|Name", AuthorNmTokV);
  for (int AuthorNmTokN=0; AuthorNmTokN<AuthorNmTokV.Len(); AuthorNmTokN++){
    PXmlTok AuthorNmTok=AuthorNmTokV[AuthorNmTokN];
    TStr AuthorNm=AuthorNmTok->GetTokStr(false);
    AmazonItem->AuthorNmV.Add(AuthorNm);
  }
  // x-sell item-ids
  TXmlTokV NextItemIdTokV;
  XmlDoc->GetTagTokV("AmazonItem|XSell|ItemId", NextItemIdTokV);
  for (int ItemIdTokN=0; ItemIdTokN<NextItemIdTokV.Len(); ItemIdTokN++){
    PXmlTok NextItemIdTok=NextItemIdTokV[ItemIdTokN];
    TStr NextItemId=NextItemIdTok->GetTokStr(false);
    AmazonItem->NextItemIdV.Add(NextItemId);
  }
  // return item
  return AmazonItem;
}

PXmlDoc TAmazonItem::GetXmlDoc() const {
  // create item xml document
  // create item
  PXmlTok ItemXmlTok=TXmlTok::New(xsyTag, "AmazonItem");
  //ItemXmlTok->AddArg("ItemId", ItemId);
  // add item-id
  PXmlTok ItemIdTxtXmlTok=TXmlTok::New(xsyStr, ItemId);
  PXmlTok ItemIdXmlTok=TXmlTok::New(xsyTag, "ItemId");
  ItemIdXmlTok->AddSubTok(ItemIdTxtXmlTok);
  ItemXmlTok->AddSubTok(ItemIdXmlTok);
  // add title
  PXmlTok TitleTxtXmlTok=TXmlTok::New(xsyStr, TitleStr);
  PXmlTok TitleXmlTok=TXmlTok::New(xsyTag, "Title");
  TitleXmlTok->AddSubTok(TitleTxtXmlTok);
  ItemXmlTok->AddSubTok(TitleXmlTok);
  // add authors
  PXmlTok AuthorsXmlTok=TXmlTok::New(xsyTag, "Authors");
  ItemXmlTok->AddSubTok(AuthorsXmlTok);
  for (int AuthorNmN=0; AuthorNmN<GetAuthorNms(); AuthorNmN++){
    PXmlTok NameXmlTok=TXmlTok::New(xsyTag, "Name");
    AuthorsXmlTok->AddSubTok(NameXmlTok);
    PXmlTok NameTxtXmlTok=TXmlTok::New(xsyStr, GetAuthorNm(AuthorNmN));
    NameXmlTok->AddSubTok(NameTxtXmlTok);
  }
  // add x-sell
  PXmlTok XSellXmlTok=TXmlTok::New(xsyTag, "XSell");
  ItemXmlTok->AddSubTok(XSellXmlTok);
  for (int NextItemId=0; NextItemId<GetNextItemIds(); NextItemId++){
    PXmlTok ItemIdXmlTok=TXmlTok::New(xsyTag, "ItemId");
    XSellXmlTok->AddSubTok(ItemIdXmlTok);
    PXmlTok ItemIdTxtXmlTok=TXmlTok::New(xsyStr, GetNextItemId(NextItemId));
    ItemIdXmlTok->AddSubTok(ItemIdTxtXmlTok);
  }
  // create item document
  PXmlDoc ItemXmlDoc=TXmlDoc::New(ItemXmlTok);
  // return xml document
  return ItemXmlDoc;
}

PAmazonItem TAmazonItem::GetFromWebPg(const PWebPg& WebPg){
  TStr UrlStr=WebPg->GetUrlStr();
  TStr ItemId=TAmazonItem::GetItemId(WebPg->GetUrl());
  TStr HtmlStr=WebPg->GetHttpBodyAsStr();
  PSIn HtmlSIn=TStrIn::New(HtmlStr);
  THtmlLx HtmlLx(HtmlSIn);
  THtmlLxSym Sym; TChA ChA;

  // move to title
  while (HtmlLx.GetSym()!=hsyEof){
    Sym=HtmlLx.Sym; ChA=HtmlLx.ChA;
    if ((Sym==hsyBTag)&&(ChA=="<FONT>")){
      TStr FaceArg=HtmlLx.GetArg("FACE", "");
      TStr SizeArg=HtmlLx.GetArg("SIZE", "");
      if ((FaceArg=="verdana,arial,helvetica")&&(SizeArg.Empty())){break;}
    }
  }
  // extract title
  TChA TitleChA;
  while (HtmlLx.GetSym()!=hsyEof){
    Sym=HtmlLx.Sym; ChA=HtmlLx.ChA;
    if ((Sym==hsyETag)&&(ChA=="<FONT>")){break;}
    if (!TitleChA.Empty()){TitleChA+=HtmlLx.GetPreSpaceStr();}
    TitleChA+=ChA;
  }
  TStr TitleStr=TitleChA;
  //printf("'%s'\n", TitleStr.CStr());
  // extract authors
  TStrV AuthorNmV;
  TChA AuthorNmChA;
  while (HtmlLx.GetSym()!=hsyEof){
    Sym=HtmlLx.Sym; ChA=HtmlLx.ChA;
    if ((Sym==hsyBTag)&&(ChA=="<A>")){
      do {
        HtmlLx.GetSym();
        Sym=HtmlLx.Sym; ChA=HtmlLx.ChA;
        if (Sym==hsyStr){
          if (!AuthorNmChA.Empty()){AuthorNmChA+=HtmlLx.GetPreSpaceStr();}
          AuthorNmChA+=ChA;
        }
      } while (!((Sym==hsyETag)&&(ChA=="<A>")));
      AuthorNmV.Add(AuthorNmChA); AuthorNmChA.Clr();
    }
    if ((Sym==hsyETag)&&(ChA=="<FONT>")){break;}
  }
  for (int AuthorNmN=0; AuthorNmN<AuthorNmV.Len(); AuthorNmN++){
    //printf("'%s'\n", AuthorNmV[AuthorNmN].CStr());
  }
  // move to x-sell
  TStrQ PrevStrQ(3);
  while (HtmlLx.GetSym()!=hsyEof){
    Sym=HtmlLx.Sym; ChA=HtmlLx.ChA;
    if (Sym==hsyStr){
      PrevStrQ.Push(ChA);
      if ((PrevStrQ.Len()==3)&&(PrevStrQ[0]=="Customers")
       &&(PrevStrQ[1]=="who")&&(PrevStrQ[2]=="bought")){break;}
    } else {
      PrevStrQ.Clr();
    }
  }
  // extract x-sell pointers
  TStrV NextItemIdV;
  while (HtmlLx.GetSym()!=hsyEof){
    Sym=HtmlLx.Sym; ChA=HtmlLx.ChA;
    if ((Sym==hsyBTag)&&(ChA=="<A>")){
      TStr RelUrlStr=HtmlLx.GetArg("HREF");
      PUrl Url=TUrl::New(RelUrlStr, UrlStr);
      TStr NextItemId=TAmazonItem::GetItemId(Url);
      NextItemIdV.Add(NextItemId);
    }
    if ((Sym==hsyETag)&&(ChA=="<UL>")){break;}
  }
  for (int NextItemIdN=0; NextItemIdN<NextItemIdV.Len(); NextItemIdN++){
    //printf("'%s'\n", NextItemIdV[NextItemIdN].CStr());
  }

  // construct item object
  PAmazonItem AmazonItem=PAmazonItem(new
   TAmazonItem(ItemId, TitleStr, AuthorNmV, NextItemIdV));
  return AmazonItem;
}

/////////////////////////////////////////////////
// Amazon-Item-Base
void TAmazonItemBs::AddItemBs(const PAmazonItemBs& AmazonItemBs){
  for (int ItemIdN=0; ItemIdN<AmazonItemBs->GetItems(); ItemIdN++){
    PAmazonItem Item=AmazonItemBs->GetItem(ItemIdN);
    AddItem(Item);
  }
}

void TAmazonItemBs::GetVoidItemIdV(TStrV& VoidItemIdV) const {
  VoidItemIdV.Clr();
  for (int ItemIdN=0; ItemIdN<GetItems(); ItemIdN++){
    PAmazonItem Item=GetItem(ItemIdN);
    for (int NextItemIdN=0; NextItemIdN<Item->GetNextItemIds(); NextItemIdN++){
      TStr NextItemId=Item->GetNextItemId(NextItemIdN);
      if (!IsItem(NextItemId)){VoidItemIdV.AddUnique(NextItemId);}
    }
  }
}

PGraph TAmazonItemBs::GetGraph() const {
  // create graph
  PGraph Graph=TGGraph::New();
  // vertices
  for (int ItemIdN=0; ItemIdN<GetItems(); ItemIdN++){
    PAmazonItem Item=GetItem(ItemIdN);
    TStr VrtxNm=Item->GetStr();
    PVrtx Vrtx=PVrtx(new TGVrtx(ItemIdN, VrtxNm));
    Graph->AddVrtx(Vrtx);
  }
  // edges
  for (int SrcItemIdN=0; SrcItemIdN<GetItems(); SrcItemIdN++){
    PVrtx SrcVrtx=Graph->GetVrtx(SrcItemIdN);
    PAmazonItem SrcItem=GetItem(SrcItemIdN);
    for (int NextItemIdN=0; NextItemIdN<SrcItem->GetNextItemIds(); NextItemIdN++){
      TStr DstItemId=SrcItem->GetNextItemId(NextItemIdN);
      if (IsItem(DstItemId)){
        int DstItemIdN=GetItemIdN(DstItemId);
        PVrtx DstVrtx=Graph->GetVrtx(DstItemIdN);
        PEdge Edge=PEdge(new TGEdge(SrcVrtx, DstVrtx));
        Graph->AddEdge(Edge);
      }
    }
    if (SrcItemIdN%1000==0){printf("%d items converted\r", SrcItemIdN);}
  }
  printf("%d items converted\n", GetItems());
  // return graph
  return Graph;
}

PAmazonItemBs TAmazonItemBs::LoadXml(const TStr& FNm, const int& MxItems){
  // create item base
  PAmazonItemBs AmazonItemBs=TAmazonItemBs::New();
  // open xml file
  PSIn XmlSIn=TFIn::New(FNm);
  // read & save items to item base
  forever {
    PXmlDoc XmlDoc=TXmlDoc::LoadTxt(XmlSIn);
    if (!XmlDoc->IsOk()){break;}
    PAmazonItem AmazonItem=TAmazonItem::New(XmlDoc);
    AmazonItemBs->AddItem(AmazonItem);
    if (AmazonItemBs->GetItems()%1000==0){
      printf("%d items loaded\r", AmazonItemBs->GetItems());}
    if ((MxItems!=-1)&&(AmazonItemBs->GetItems()>=MxItems)){break;}
  }
  printf("%d items loaded\n", AmazonItemBs->GetItems());
  // return item base
  return AmazonItemBs;
}

void TAmazonItemBs::SaveXml(const TStr& FNm){
  PSOut XmlSOut=TFOut::New(FNm);
  for (int ItemIdN=0; ItemIdN<GetItems(); ItemIdN++){
    PAmazonItem Item=GetItem(ItemIdN);
    PXmlDoc XmlDoc=Item->GetXmlDoc();
    XmlDoc->SaveTxt(XmlSOut); XmlSOut->PutDosLn();
  }
}

/////////////////////////////////////////////////
// Amazon-Page-Fetch
TAmazonFetch::TAmazonFetch():
  TWebPgFetch(), ItemItemIdH(100000), XmlSOut(){
  PutMxConns(30);
  XmlSOut=TFOut::New("amazon.xml");
}

PAmazonFetch TAmazonFetch::New(const TStr& FNm){
  // create fetch
  PAmazonFetch AmazonFetch=New();
  // check & load file
  if (!FNm.Empty()){
    PSIn XmlSIn=TFIn::New(FNm);
    forever {
      PXmlDoc XmlDoc=TXmlDoc::LoadTxt(XmlSIn);
      if (!XmlDoc->IsOk()){break;}
      TStr ItemId=XmlDoc->GetTagTok("AmazonItem|ItemId")->GetTokStr(false);
      AmazonFetch->AddItem(ItemId);
      if (AmazonFetch->GetItems()%1000==0){
        printf("%d items loaded\r", AmazonFetch->GetItems());}
    }
    printf("%d items loaded\n", AmazonFetch->GetItems());
  }
  // return fetch
  return AmazonFetch;
}

PAmazonFetch TAmazonFetch::New(const PAmazonItemBs& AmazonItemBs){
  // create fetch
  PAmazonFetch AmazonFetch=New();
  // add items from item-base
  for (int ItemIdN=0; ItemIdN<AmazonItemBs->GetItems(); ItemIdN++){
    TStr ItemId=AmazonItemBs->GetItemId(ItemIdN);
    AmazonFetch->AddItem(ItemId);
  }
  // return fetch
  return AmazonFetch;
}

void TAmazonFetch::FetchItemIdV(const TStrV& ItemIdV){
  for (int ItemIdN=0; ItemIdN<ItemIdV.Len(); ItemIdN++){
    TStr UrlStr=TAmazonItem::GetUrlStr(ItemIdV[ItemIdN]);
    FetchUrl(UrlStr);
  }
}

void TAmazonFetch::OnFetch(const int&, const PWebPg& WebPg){
  PAmazonItem AmazonItem=TAmazonItem::GetFromWebPg(WebPg);
  if (!IsItem(AmazonItem->GetItemId())){
    printf("'%s'\n", AmazonItem->GetTitleStr().CStr());
    AddItem(AmazonItem->GetItemId());
    PXmlDoc ItemXmlDoc=AmazonItem->GetXmlDoc();
    ItemXmlDoc->SaveTxt(XmlSOut); XmlSOut->PutDosLn(); XmlSOut->Flush();
    for (int ItemIdN=0; ItemIdN<AmazonItem->GetNextItemIds(); ItemIdN++){
      TStr ItemId=AmazonItem->GetNextItemId(ItemIdN);
      if (!IsItem(ItemId)){
        TStr UrlStr=TAmazonItem::GetUrlStr(ItemId);
        FetchUrl(UrlStr);
      }
    }
  }
  printf("Active:%i Queue:%i Pages:%i\r",
   GetConnUrls(), GetWaitUrls(), GetItems());
  if (GetConnUrls()==0){TSysMsg::Quit();}
}

void TAmazonFetch::OnError(const int&, const TStr& MsgStr){
  printf("*** Error: %s\n", MsgStr.CStr());
  printf("Active:%i Queue:%i Pages:%i\r",
   GetConnUrls(), GetWaitUrls(), GetItems());
  if (GetConnUrls()==0){TSysMsg::Quit();}
}


/////////////////////////////////////////////////
// Includes
#include "tagcloud.h"

PTagCloud TTagCloud::GetFromDocStrWgtPrV(const TStrFltPrV& DocStrWgtPrV, 
 const int& TopWords=25, const double& TopWordsWgtSumPrc=1.0){
  PTagCloud TagCloud=TTagCloud::New();
  // create bow
  PSwSet SwSet=TSwSet::GetSwSet(swstEn523);
  PStemmer Stemmer=TStemmer::GetStemmer(stmtPorter);
  PBowDocBs BowDocBs=TBowDocBs::New(SwSet, Stemmer, NULL);
  TIntFltPrV DocIdWgtPrV(DocStrWgtPrV.Len(), 0);
  for (int DocN=0; DocN<DocStrWgtPrV.Len(); DocN++){
    TStr DocNm=TInt(DocN).GetStr();
    TStr DocStr=DocStrWgtPrV[DocN].Val1;
    double DocWgt=DocStrWgtPrV[DocN].Val2;
    int DocId=BowDocBs->AddHtmlDoc(DocNm, TStrV(), DocStr);
    DocIdWgtPrV.Add(TIntFltPr(DocId, DocWgt));
  }
  // create word-weights
  TBowWordWgtType WordWgtType=bwwtNrmTFIDF; // define weighting
  PBowDocWgtBs BowDocWgtBs=TBowDocWgtBs::New(BowDocBs, WordWgtType, 0, 0);
  // create concept vector
  PBowSim BowSim=TBowSim::New(bstCos); // similarity object
  TIntV AllDIdV; BowDocBs->GetAllDIdV(AllDIdV);
  PBowSpV ConceptSpV=TBowClust::GetConceptSpV(BowDocWgtBs, BowSim, AllDIdV, 1, DocIdWgtPrV);
  // get word-vector
  ConceptSpV->GetWordStrWgtPrV(BowDocBs, TopWords, TopWordsWgtSumPrc, TagCloud->WordStrWgtPrV);
  // word placement
  TagCloud->PlaceWords();
  // return
  return TagCloud;
}

void TTagCloud::PlaceWords(){
  int Words=WordStrWgtPrV.Len();
  TGksRectV RectV(Words);
  // assign rectangles to words
  for (int WordN=0; WordN<Words; WordN++){
    RectV[WordN].X1=0; RectV[WordN].Y1=0;
    RectV[WordN].X2=WordStrWgtPrV[WordN].Val1.Len(); RectV[WordN].Y2=1;
  }
  // reduce size of rectangles to appropriate size
  forever {
    double AreaSum=0; double MxY=0;
    for (int WordN=0; WordN<Words; WordN++){
      RectV[WordN].X2*=0.9; RectV[WordN].Y2=0.9;
      AreaSum+=RectV[WordN].X2*RectV[WordN].Y2;
      MxY=(WordN==0) ? RectV[WordN].Y2 : TFlt::GetMx(MxY, RectV[WordN].Y2);
    }
    if ((AreaSum<0.25)&&(MxY<1)){
      break;}
  }
}

void TTagCloud::Dump(){
  // print tag-cloud
  for (int WordN=0; WordN<WordStrWgtPrV.Len(); WordN++){
    printf("[%s:%.2f] ", WordStrWgtPrV[WordN].Val1.CStr(), WordStrWgtPrV[WordN].Val2);
  }
}
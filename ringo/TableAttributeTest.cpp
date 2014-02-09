#include "snap.h"
//#include "table.h"
#include <cmath>

int main(){
  TTableContext Context;
  TIntIntH IH;
  TIntFltH FH;
  for(int i = 0; i < 1000; i++){
    IH.AddDat(i, i*i);
    FH.AddDat(i, sqrt((double)i));
  }
  TTable Sq("Sq", IH, "number", "square", Context);
  TTable Rt("Rt", FH, "number", "root", Context);
  PTable Tj = Sq.Join("number", Rt, "number");
  TStrV S;
  S.Add("Sq.number");
  S.Add("Sq.square");
  S.Add("Rt.root");
  Tj->ProjectInPlace(S);
  Tj->ColMod("Sq.number", 10, "Mod10");
  Tj->SelectAtomicIntConst("Mod10", 9, EQ);
  Tj->ColMod("Sq.number", 3, "Mod3");
  Tj->ClassifyAtomicIntConst("Mod3", 0, EQ, "Label");
  Tj->SaveSS("../../testfiles/attr_test_out.txt");


  Schema AnimalS;
  AnimalS.Add(TPair<TStr,TAttrType>("Animal", atStr));
  AnimalS.Add(TPair<TStr,TAttrType>("Size", atStr));
  AnimalS.Add(TPair<TStr,TAttrType>("Location", atStr));
  AnimalS.Add(TPair<TStr,TAttrType>("Number", atInt));
  TIntV RelevantCols;
  RelevantCols.Add(0);
  RelevantCols.Add(1);
  RelevantCols.Add(2);
  // create table
  PTable T = TTable::LoadSS("Animals", AnimalS, "../../testfiles/animals.txt", Context, RelevantCols, '\t', true);
  T->SaveSS("../../testfiles/attr_test_out_1.txt");
  PTable Tsz = T->SelfJoin("Size");
  TStrV UV;
  UV.Add("Animals_1.Animal");
  UV.Add("Animals_2.Animal");
  Tsz->Unique(UV, false);
  Tsz->SetSrcCol("Animals_1.Animal");
  Tsz->SetDstCol("Animals_2.Animal");
  PNEANet G = TSnap::ToGraph(Tsz, aaLast);
  TIntFltH PR;
  TSnap::GetPageRank(G, PR);
  TTable AnimalRank("AnimalRank", PR, "animal", "rank", Context, true);
  AnimalRank.ClassifyAtomicStrConst("animal", "Lio", SUPERSTR, "label");
  AnimalRank.SaveSS("../../testfiles/attr_test_out_pr.txt");
}

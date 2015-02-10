#include "stdafx.h"
#include <Snap.h>
#include <stdio.h>
#include <omp.h>
#include "benchmark-util.cpp"
#include "multimodal.cpp"

PSOut StdOut = TStdOut::New();

int main(int argc, char* argv[])
{
  TEnv Env(argc, argv);
  TStr PrefixPath = Env.GetArgs() > 1 ? Env.GetArg(1) : TStr("");

  int nExps = 10;

  double ts1 = Tick();
  TTableContext Context;
  TVec<TPair<PTable,TStr> > NodeTblV = TVec<TPair<PTable,TStr> >();
  TVec<TQuad<PTable,TStr,TStr,TBool> > EdgeTblV = TVec<TQuad<PTable,TStr,TStr,TBool> >();
  Schema NodeSchema = Schema();
  Schema EdgeSchema = Schema();
  LoadFlickrTables(PrefixPath, Context, NodeTblV, NodeSchema, EdgeTblV, EdgeSchema);

  TVec<TPair<TStr,TStr> > RandNV;
  LoadNodeList(PrefixPath, TStr("rand_node_ids_all.tsv"), RandNV);
  StdOut->PutStrFmtLn("RandNV.Len() = %d", RandNV.Len());

  THash<TStr,TStrH> NStrH;
  TIntStrH NIdH;
  CreateIdHashes(NodeTblV, NStrH, NIdH);

  double ts2 = Tick();

  double PreprocessTime = 0;
  double ConstructTime = 0;

  PSVNet Graph;
  for (int i = 0; i < nExps; i++) {
    double ts3 = Tick();
    TVec<TVec<TTriple<TStr,TStr,TBool> > > RandNbrVV;
    CreateNborList(NodeTblV, EdgeTblV, RandNV, RandNbrVV);
    double ts4 = Tick();
    Graph = LoadGraphMNetRandom<PSVNet>(NodeTblV, EdgeTblV, RandNV, RandNbrVV, NStrH, NIdH);
    double ts5 = Tick();
    PreprocessTime += (ts4-ts3);
    ConstructTime += (ts5-ts4);
  }

  PSOut FeaturesOut = TFOut::New(PrefixPath + "features.txt");
  FeaturesOut->PutStrFmtLn("Photo %d", NodeTblV[0].GetVal1()->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Users %d", NodeTblV[1].GetVal1()->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Tags %d", NodeTblV[2].GetVal1()->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Comments %d", NodeTblV[3].GetVal1()->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Locations %d", NodeTblV[4].GetVal1()->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Photo - Owner %d", EdgeTblV[0].GetVal1()->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Photo - Comment %d", EdgeTblV[1].GetVal1()->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Photo - Location %d", EdgeTblV[2].GetVal1()->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Comment - User %d", EdgeTblV[3].GetVal1()->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Photo - Tag %d", EdgeTblV[4].GetVal1()->GetNumRows().Val);
//  FeaturesOut->PutStrFmtLn("Photo - Tagger %d", PPhotoTaggerTbl->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Tagger - Tag %d", EdgeTblV[5].GetVal1()->GetNumRows().Val);
  FeaturesOut->PutStrFmtLn("Total number of nodes = %d", Graph->GetNodes());
  FeaturesOut->PutStrFmtLn("Total number of edges = %d", Graph->GetEdges());

  PSOut TimeOut = TFOut::New(PrefixPath + TStr("time.txt"), true);
  TimeOut->PutStrFmtLn("===== Random Construction - PSVNet =====");
  TimeOut->PutStrLn(Env.GetCmLn());
  TimeOut->PutStrFmtLn("Input Time = %f", GetCPUTimeUsage(ts1, ts2));
  TimeOut->PutStrFmtLn("Preprocessing Time = %f", PreprocessTime/nExps);
  TimeOut->PutStrFmtLn("Construction Time = %f", ConstructTime/nExps);

	return 0;
}

#include "stdafx.h"
#include <Snap.h>
#include <stdio.h>
#include <omp.h>
#include "benchmark-util.cpp"
#include "multimodal.cpp"

PSOut StdOut = TStdOut::New();

template <class PGraph>
void DelNodeExp(const PGraph& Graph, const TStrV& DeletedNStrV, const TStrIntH& NStrH) {
  for (int i = 0; i < DeletedNStrV.Len(); i++) {
    TStr NStr = DeletedNStrV[i];
    TInt NId = NStrH.GetDat(NStr);
    Graph->DelNode(NId);
  }
}

int main(int argc, char* argv[]) {
  TEnv Env(argc, argv);
  TStr PrefixPath = Env.GetArgs() > 1 ? Env.GetArg(1) : TStr("");

  double ts1 = Tick();
  TTableContext Context;
  TVec<PTable> NodeTblV = TVec<PTable>();
  TVec<TPair<PTable, int> > EdgeTblV = TVec<TPair<PTable, int> >();
  Schema NodeSchema = Schema();
  Schema EdgeSchema = Schema();
  LoadFlickrTables(PrefixPath, Context, NodeTblV, NodeSchema, EdgeTblV, EdgeSchema);

  double ts2 = Tick();

  TStrV DeletedNStrV;
  LoadMixedTypeNodeList(PrefixPath, TStr("rand_node_ids_deleted.tsv"), DeletedNStrV);

  double ts3 = Tick();

  TStrIntH NStrH;
  TIntStrH NIdH;
  CreateIdHashes(NodeTblV, NStrH, NIdH);

  double ts4 = Tick();

  double convertTime = 0;
  double deleteTime = 0;
  int nExps = 10;

  for (int i = 0; i < nExps; i++) {
    double t1 = Tick();
    //PNEANet Graph = LoadGraph<PNEANet>(NodeTblV, EdgeTblV, NStrH, NIdH);
    PMVNet Graph = LoadGraphMNet<PMVNet>(NodeTblV, EdgeTblV, NStrH, NIdH);
    StdOut->PutStrFmtLn("Size %d-%d", Graph->GetNodes(), Graph->GetEdges());
    double t2 = Tick();
    DelNodeExp(Graph, DeletedNStrV, NStrH);
    double t3 = Tick();
    StdOut->PutStrFmtLn("Size %d-%d", Graph->GetNodes(), Graph->GetEdges());
    convertTime += (t2-t1);
    deleteTime += (t3-t2);
  }

  PSOut TimeOut = TFOut::New(PrefixPath + TStr("time.txt"), true);
  TimeOut->PutStrFmtLn("Loading Graph Tables = %f s", GetCPUTimeUsage(ts1, ts2));
  TimeOut->PutStrFmtLn("Loading Deleted Nodes = %f s", GetCPUTimeUsage(ts2, ts3));
  TimeOut->PutStrFmtLn("Preprocessing = %f s", GetCPUTimeUsage(ts3, ts4));
  TimeOut->PutStrFmtLn("Conversion = %f s", convertTime/nExps);
  TimeOut->PutStrFmtLn("Node Deletion = %f s", deleteTime/nExps);

  return 0;
}

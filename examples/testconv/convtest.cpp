#include "stdafx.h"
#include <Snap.h>
#include <stdio.h>
#include <omp.h>

PSOut StdOut = TStdOut::New();

void test_ints(){
  TTableContext context;
  Schema schema;
  schema.Add(TPair<TStr,TAttrType>("src",atInt));
  schema.Add(TPair<TStr,TAttrType>("dst",atInt));
  TStr wikifilename = "/dfs/scratch0/viswa/wiki_Vote.txt";

  PTable wikitable = TTable::LoadSS(schema, wikifilename, &context, '\t', TBool(false));

  TTableContext nodecontext;
  Schema nodeschema;
  nodeschema.Add(TPair<TStr,TAttrType>("nid",atInt));
  nodeschema.Add(TPair<TStr,TAttrType>("nattr",atInt));
  TStr nodefilename("/dfs/scratch0/viswa/fromlfs/snap-dev1/examples/testconv/nodelist");
  PTable nodetable = TTable::LoadSS(nodeschema, nodefilename, &nodecontext, '\t', TBool(false));
 
 
  printf("Loaded the table!\n");
#if 1
  PUNGraph pungraph = TSnap::ToGraph<PUNGraph>(wikitable,schema[0].GetVal1(),schema[1].GetVal1(),aaFirst);
  printf("Made the TUNGraph of %d nodes and %d edges.\n",(*pungraph).GetNodes(),(*pungraph).GetEdges());
/*
  for (TUNGraph::TNodeI node_i = pungraph->BegNI(); node_i < pungraph->EndNI(); node_i++) {
    printf("%d\t1\n", node_i.GetId());
  }
*/
  PNGraph pngraph = TSnap::ToGraph<PNGraph>(wikitable,schema[0].GetVal1(),schema[1].GetVal1(),aaFirst);
  printf("Made the TNGraph of %d nodes and %d edges.\n",(*pngraph).GetNodes(),(*pngraph).GetEdges());
  PNGraphMP pngraphmp = TSnap::ToGraphMP<PNGraphMP>(wikitable,schema[0].GetVal1(),schema[1].GetVal1());
  printf("Made the TNGraphMP of %d nodes and %d edges.\n",(*pngraphmp).GetNodes(),(*pngraphmp).GetEdges());

//  PNGraphMP pngraphmp_2 = TSnap::ToGraphMP3<PNGraphMP>(wikitable,schema[0].GetVal1(),schema[1].GetVal1());
//  printf("Made the TNGraphMP of %d nodes and %d edges with MP2.\n",(*pngraphmp_2).GetNodes(),(*pngraphmp_2).GetEdges());

  printf("Tested graph conversion with ints.\n");
#endif

  TVec<TStr> attrv;
  attrv.Add(schema[1].GetVal1());
//  PNEANet pneanet = TSnap::ToNetwork<PNEANet>(wikitable,schema[0].GetVal1(),schema[1].GetVal1(), attrv, aaFirst);
//  printf("Made the PNEANet of %d nodes and %d edges.\n", (*pneanet).GetNodes(),(*pneanet).GetEdges());
  PNEANetMP pneanetmp = TSnap::ToNetworkMP<PNEANetMP>(wikitable,schema[0].GetVal1(),schema[1].GetVal1(), attrv, aaFirst);
  printf("Made the PNEANetMP of %d nodes and %d edges.\n", (*pneanetmp).GetNodes(),(*pneanetmp).GetEdges());
  //  PNEANetMP pneanetmp2 = TSnap::ToNetworkMP2<PNEANetMP>(wikitable,schema[0].GetVal1(),schema[1].GetVal1(),aaFirst);
//  printf("Made the PNEANetMP of %d nodes and %d edges with MP2.\n", (*pneanetmp2).GetNodes(),(*pneanetmp).GetEdges());

  TVec<TStr> eattrv;
  eattrv.Add(schema[1].GetVal1());
  TVec<TStr> nattrv;
  nattrv.Add(nodeschema[1].GetVal1());
  PNEANet pneanet_attrs = TSnap::ToNetwork<PNEANet>(wikitable, schema[0].GetVal1(), schema[1].GetVal1(), eattrv, nodetable, nodeschema[0].GetVal1(), nattrv, aaFirst);
  printf("Made the PNEANet of %d nodes and %d edges with one edge and one node attr.\n", (*pneanet_attrs).GetNodes(),(*pneanet_attrs).GetEdges());

  PNEANetMP pneanetmp_attrs = TSnap::ToNetworkMP<PNEANetMP>(wikitable, schema[0].GetVal1(), schema[1].GetVal1(), eattrv, nodetable, nodeschema[0].GetVal1(), nattrv, aaFirst);
  printf("Made the PNEANetMP of %d nodes and %d edges with one edge and one node attr.\n", (*pneanetmp_attrs).GetNodes(),(*pneanetmp_attrs).GetEdges());
  printf("Tested network conversions with ints.\n");

  
}

void test_strs(){
  TTableContext context;
  Schema schema;
  schema.Add(TPair<TStr,TAttrType>("src",atStr));
  schema.Add(TPair<TStr,TAttrType>("dst",atStr));
  TStr wikifilename = "/dfs/scratch0/viswa/wiki_Vote.txt";

  PTable wikitable = TTable::LoadSS(schema, wikifilename, &context, '\t', TBool(false));
  printf("Loaded the table!\n");
  /*
  PUNGraph pungraph = TSnap::ToGraph<PUNGraph>(wikitable,schema[0].GetVal1(),schema[1].GetVal1(),aaFirst);
  printf("Made the TUNGraph of %d nodes and %d edges.\n",(*pungraph).GetNodes(),(*pungraph).GetEdges());
  PNGraph pngraph = TSnap::ToGraph<PNGraph>(wikitable,schema[0].GetVal1(),schema[1].GetVal1(),aaFirst);
  printf("Made the TNGraph of %d nodes and %d edges.\n",(*pngraph).GetNodes(),(*pngraph).GetEdges());
  PNGraphMP pngraphmp = TSnap::ToGraphMP<PNGraphMP>(wikitable,schema[0].GetVal1(),schema[1].GetVal1());
  printf("Made the TNGraphMP of %d nodes and %d edges.\n",(*pngraphmp).GetNodes(),(*pngraphmp).GetEdges());
//  PNGraphMP pngraphmp_2 = TSnap::ToGraphMP3<PNGraphMP>(wikitable,schema[0].GetVal1(),schema[1].GetVal1());
//  printf("Made the TNGraphMP of %d nodes and %d edges with MP2.\n",(*pngraphmp_2).GetNodes(),(*pngraphmp_2).GetEdges());

  printf("Tested graph conversion with strings.\n");

  TVec<TStr> attrv;
  attrv.Add(schema[1].GetVal1());
  PNEANet pneanet = TSnap::ToNetwork<PNEANet>(wikitable,schema[0].GetVal1(),schema[1].GetVal1(), attrv, aaFirst);
  printf("Made the PNEANet of %d nodes and %d edges.\n", (*pneanet).GetNodes(),(*pneanet).GetEdges());
  PNEANetMP pneanetmp = TSnap::ToNetworkMP<PNEANetMP>(wikitable,schema[0].GetVal1(),schema[1].GetVal1(), attrv, aaFirst);
  printf("Made the PNEANetMP of %d nodes and %d edges.\n", (*pneanetmp).GetNodes(),(*pneanet).GetEdges());
//  PNEANetMP pneanetmp2 = TSnap::ToNetworkMP2<PNEANetMP>(wikitable,schema[0].GetVal1(),schema[1].GetVal1(),aaFirst);
//  printf("Made the PNEANetMP of %d nodes and %d edges with MP2.\n", (*pneanetmp2).GetNodes(),(*pneanetmp).GetEdges());

  printf("Tested network conversions with strings.\n");
*/ 
}



int main(int argc, char* argv[]) {
  TEnv Env(argc, argv);
  TStr PrefixPath = Env.GetArgs() > 1 ? Env.GetArg(1) : TStr("");
  test_ints();
  test_strs();

 return 0;
}

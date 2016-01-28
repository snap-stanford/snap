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
  printf("Loaded the table!\n");
  PUNGraph pungraph = TSnap::ToGraph<PUNGraph>(wikitable,schema[0].GetVal1(),schema[1].GetVal1(),aaFirst);
  printf("Made the TUNGraph of %d nodes and %d edges.\n",(*pungraph).GetNodes(),(*pungraph).GetEdges());
  PNGraph pngraph = TSnap::ToGraph<PNGraph>(wikitable,schema[0].GetVal1(),schema[1].GetVal1(),aaFirst);
  printf("Made the TNGraph of %d nodes and %d edges.\n",(*pngraph).GetNodes(),(*pngraph).GetEdges());
  PNGraphMP pngraphmp = TSnap::ToGraphMP<PNGraphMP>(wikitable,schema[0].GetVal1(),schema[1].GetVal1());
  printf("Made the TNGraphMP of %d nodes and %d edges.\n",(*pngraphmp).GetNodes(),(*pngraphmp).GetEdges());

//  PNGraphMP pngraphmp_2 = TSnap::ToGraphMP3<PNGraphMP>(wikitable,schema[0].GetVal1(),schema[1].GetVal1());
//  printf("Made the TNGraphMP of %d nodes and %d edges with MP2.\n",(*pngraphmp_2).GetNodes(),(*pngraphmp_2).GetEdges());

  printf("Tested graph conversion with ints.\n");

  TVec<TStr> emptyattrv;
  PNEANet pneanet = TSnap::ToNetwork<PNEANet>(wikitable,schema[0].GetVal1(),schema[1].GetVal1(),emptyattrv, emptyattrv,emptyattrv,aaFirst);
  printf("Made the PNEANet of %d nodes and %d edges.\n", (*pneanet).GetNodes(),(*pneanet).GetEdges());

  PNEANetMP pneanetmp = TSnap::ToNetworkMP<PNEANetMP>(wikitable,schema[0].GetVal1(),schema[1].GetVal1(),aaFirst);
  printf("Made the PNEANetMP of %d nodes and %d edges.\n", (*pneanetmp).GetNodes(),(*pneanet).GetEdges());

  PNEANetMP pneanetmp2 = TSnap::ToNetworkMP2<PNEANetMP>(wikitable,schema[0].GetVal1(),schema[1].GetVal1(),aaFirst);
  printf("Made the PNEANetMP of %d nodes and %d edges with MP2.\n", (*pneanetmp2).GetNodes(),(*pneanetmp).GetEdges());

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
  PUNGraph pungraph = TSnap::ToGraph<PUNGraph>(wikitable,schema[0].GetVal1(),schema[1].GetVal1(),aaFirst);
  printf("Made the TUNGraph of %d nodes and %d edges.\n",(*pungraph).GetNodes(),(*pungraph).GetEdges());
  PNGraph pngraph = TSnap::ToGraph<PNGraph>(wikitable,schema[0].GetVal1(),schema[1].GetVal1(),aaFirst);
  printf("Made the TNGraph of %d nodes and %d edges.\n",(*pngraph).GetNodes(),(*pngraph).GetEdges());
  PNGraphMP pngraphmp = TSnap::ToGraphMP<PNGraphMP>(wikitable,schema[0].GetVal1(),schema[1].GetVal1());
  printf("Made the TNGraphMP of %d nodes and %d edges.\n",(*pngraphmp).GetNodes(),(*pngraphmp).GetEdges());
//  PNGraphMP pngraphmp_2 = TSnap::ToGraphMP3<PNGraphMP>(wikitable,schema[0].GetVal1(),schema[1].GetVal1());
//  printf("Made the TNGraphMP of %d nodes and %d edges with MP2.\n",(*pngraphmp_2).GetNodes(),(*pngraphmp_2).GetEdges());

  printf("Tested graph conversion with strings.\n");

  TVec<TStr> emptyattrv;
  PNEANet pneanet = TSnap::ToNetwork<PNEANet>(wikitable,schema[0].GetVal1(),schema[1].GetVal1(),emptyattrv, emptyattrv,emptyattrv,aaFirst);
  printf("Made the PNEANet of %d nodes and %d edges.\n", (*pneanet).GetNodes(),(*pneanet).GetEdges());
  PNEANetMP pneanetmp = TSnap::ToNetworkMP<PNEANetMP>(wikitable,schema[0].GetVal1(),schema[1].GetVal1(),aaFirst);
  printf("Made the PNEANetMP of %d nodes and %d edges.\n", (*pneanetmp).GetNodes(),(*pneanet).GetEdges());
  PNEANetMP pneanetmp2 = TSnap::ToNetworkMP2<PNEANetMP>(wikitable,schema[0].GetVal1(),schema[1].GetVal1(),aaFirst);
  printf("Made the PNEANetMP of %d nodes and %d edges with MP2.\n", (*pneanetmp2).GetNodes(),(*pneanetmp).GetEdges());

  printf("Tested network conversions with strings.\n");
 
}



int main(int argc, char* argv[]) {
  TEnv Env(argc, argv);
  TStr PrefixPath = Env.GetArgs() > 1 ? Env.GetArg(1) : TStr("");
  test_ints();
  test_strs();

 return 0;
}

#include "stdafx.h"

void PrintGraphStat(const PNGraph& G) {
  PNGraph WCC = TSnap::GetMxWcc(G);
  PNGraph SCC = TSnap::GetMxScc(G);
  TFltPrV DegCCfV;
  int ClosedTriads, OpenTriads, FullDiam;
  double EffDiam;
  printf("Nodes\t%d\n", G->GetNodes());
  printf("Edges\t%d\n", G->GetEdges());
  printf("Nodes in largest WCC\t%d (%.3f)\n", WCC->GetNodes(), WCC->GetNodes()/double(G->GetNodes()));
  printf("Edges in largest WCC\t%d (%.3f)\n", WCC->GetEdges(), WCC->GetEdges()/double(G->GetEdges()));
  printf("Nodes in largest SCC\t%d (%.3f)\n", SCC->GetNodes(), SCC->GetNodes()/double(G->GetNodes()));
  printf("Edges in largest SCC\t%d (%.3f)\n", SCC->GetEdges(), SCC->GetEdges()/double(G->GetEdges()));
  const double CCF = TSnap::GetClustCf(G, DegCCfV, ClosedTriads, OpenTriads);
  printf("Average clustering coefficient\t%.4f\n", CCF);
  printf("Number of triangles\t%d\n", ClosedTriads);
  printf("Fraction of closed triangles\t%.4g\n", ClosedTriads/double(ClosedTriads+OpenTriads));
  TSnap::GetBfsEffDiam(G, 1000, false, EffDiam, FullDiam);
  printf("Diameter (longest shortest path)\t%d\n", FullDiam);
  printf("90-percentile effective diameter\t%.2g\n", EffDiam);
}


template<class PGraph> 
void PrintGraphStatTable(const PGraph& G, TStr OutFNm, TStr Desc="") {
  TFltPrV DegCCfV;
  int ClosedTriads, FullDiam, OpenTriads;
  double EffDiam;
  TSnap::PrintInfo(G, OutFNm);
  TExeTm ExeTm; printf("C");
  const double CCF = TSnap::GetClustCf(G, DegCCfV, ClosedTriads, OpenTriads);
  printf("[%s]D", ExeTm.GetStr());
  TSnap::GetBfsEffDiam(G, 1000, false, EffDiam, FullDiam);
  printf("[%s]CC", ExeTm.GetStr());
  PGraph WCC = TSnap::GetMxWcc(G);
  PGraph SCC = TSnap::GetMxScc(G);
  printf("[%s]\n", ExeTm.GetStr());
  FILE* F = stdout;
  if (! OutFNm.Empty()) {
    F = fopen(TStr::Fmt("%s.html", OutFNm.CStr()).CStr(), "wt"); }
  fprintf(F, "\n");
  fprintf(F, "<table id=\"datatab\" summary=\"Dataset statistics\">\n");
  fprintf(F, "  <tr> <th colspan=\"2\">Dataset statistics</th> </tr>\n");
  fprintf(F, "  <tr><td>Nodes</td> <td>%d</td></tr>\n", G->GetNodes());
  fprintf(F, "  <tr><td>Edges</td> <td>%d</td></tr>\n", G->GetEdges());
  fprintf(F, "  <tr><td>Nodes in largest WCC</td> <td>%d (%.3f)</td></tr>\n", WCC->GetNodes(), WCC->GetNodes()/double(G->GetNodes()));
  fprintf(F, "  <tr><td>Edges in largest WCC</td> <td>%d (%.3f)</td></tr>\n", WCC->GetEdges(), WCC->GetEdges()/double(G->GetEdges()));
  fprintf(F, "  <tr><td>Nodes in largest SCC</td> <td>%d (%.3f)</td></tr>\n", SCC->GetNodes(), SCC->GetNodes()/double(G->GetNodes()));
  fprintf(F, "  <tr><td>Edges in largest SCC</td> <td>%d (%.3f)</td></tr>\n", SCC->GetEdges(), SCC->GetEdges()/double(G->GetEdges()));
  fprintf(F, "  <tr><td>Average clustering coefficient</td> <td>%.4f</td></tr>\n", CCF);
  fprintf(F, "  <tr><td>Number of triangles</td> <td>%d</td></tr>\n", ClosedTriads);
  fprintf(F, "  <tr><td>Fraction of closed triangles</td> <td>%.4g</td></tr>\n", ClosedTriads/double(ClosedTriads+OpenTriads));
  fprintf(F, "  <tr><td>Diameter (longest shortest path)</td> <td>%d</td></tr>\n", FullDiam);
  fprintf(F, "  <tr><td>90-percentile effective diameter</td> <td>%.2g</td></tr>\n", EffDiam);
  fprintf(F, "</table>\n");
  fprintf(F, "<br>\n");
  if (! OutFNm.Empty()) {
    fprintf(F, "\n<table id=\"datatab\" summary=\"Table of datasets\">\n");
    fprintf(F, "<tr>\n");
	  fprintf(F, "  <th>File</th>\n");
	  fprintf(F, "  <th>Description</th>\n");
    fprintf(F, "</tr>\n");
    fprintf(F, "<tr>\n");
	  fprintf(F, "  <td><a href=\"%s.txt.gz\">%s.txt.gz</a></td>\n", OutFNm.CStr(), OutFNm.CStr());
	  fprintf(F, "  <td>%s</td>\n", Desc.CStr());
    fprintf(F, "</tr>\n");
    fprintf(F, "</table>\n");
    fclose(F);
    TSnap::SaveEdgeList(G, OutFNm+".txt", Desc);
  }
}


// LiveJournal network from Matt Richardson, ISWC '03
void MakeEpinions() {
  PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\Epinions\\epinions.txt", 0, 1);
  PrintGraphStatTable(G, "soc-Epinions1", "Directed Epinions social network");
}

// LiveJournal network from Lars Backstrom, KDD '06
void MakeLiveJournal1() {
  PNGraph G = TSnap::LoadEdgeListStr<PNGraph>("W:\\Data\\_graphData\\LiveJournal-Lars\\friends.gz", 0, 1);
  PrintGraphStatTable(G, "soc-LiveJournal1", "Directed LiveJournal friednship social network");
}

// Gnutella network from M. Ripeanu, IEEE Internet Computing Journal 2002
void MakeGnutella() {
  { PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\Gnutella\\g.04.id.gz", 0, 1);
  PrintGraphStatTable(G, "p2p-Gnutella04", "Directed Gnutella P2P network from August 4 2002"); }
  { PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\Gnutella\\g.05.id.gz", 0, 1);
  PrintGraphStatTable(G, "p2p-Gnutella05", "Directed Gnutella P2P network from August 5 2002"); }
  { PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\Gnutella\\g.06.id.gz", 0, 1);
  PrintGraphStatTable(G, "p2p-Gnutella06", "Directed Gnutella P2P network from August 6 2002"); }
  { PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\Gnutella\\g.08.id.gz", 0, 1);
  PrintGraphStatTable(G, "p2p-Gnutella08", "Directed Gnutella P2P network from August 8 2002"); }
  { PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\Gnutella\\g.09.id.gz", 0, 1);
  PrintGraphStatTable(G, "p2p-Gnutella09", "Directed Gnutella P2P network from August 9 2002"); }
  { PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\Gnutella\\g.24.id.gz", 0, 1);
  PrintGraphStatTable(G, "p2p-Gnutella24", "Directed Gnutella P2P network from August 24 2002"); }
  { PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\Gnutella\\g.25.id.gz", 0, 1);
  PrintGraphStatTable(G, "p2p-Gnutella25", "Directed Gnutella P2P network from August 25 2002"); }
  { PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\Gnutella\\g.30.id.gz", 0, 1);
  PrintGraphStatTable(G, "p2p-Gnutella30", "Directed Gnutella P2P network from August 30 2002"); }
  { PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\Gnutella\\g.31.id.gz", 0, 1);
  PrintGraphStatTable(G, "p2p-Gnutella31", "Directed Gnutella P2P network from August 31 2002"); }
}

// Webgraphs
void MakeWebGraphs() {
  //{ PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\Google\\google_edges.gz", 0, 1);
  //PrintGraphStatTable(G, "web-Google", "Webgraph from the Google programming contest, 2002"); }
  //{ PNGraph G = TSnap::LoadConnList<PNGraph>("W:\\Data\\_graphData\\WWW-Stanford\\StanfordBerkeleyWeb.net");
  //PrintGraphStatTable(G, "web-BerkStan", "Berkely-Stanford web graph from 2002"); }
  { PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\WWW-Stanford\\StanfordWeb.net", 0, 1);
  PrintGraphStatTable(G, "web-Stanford", "Stanford web graph from 2002"); }
  //{ PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\WWW-Barabasi\\www_edges.gz", 0, 1);
  //PrintGraphStatTable(G, "web-NotreDame", "University of Notre Dame web graph from 1999 by Albert, Jeong and Barabasi"); }
}

// road networks of California, Texas and Pennsylvania
void MakeRoadNets() {
  { PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\Roards\\CA_edges.gz", 0, 1);
  PrintGraphStatTable(G, "roadNet-CA", "California road network"); }
  { PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\Roards\\PA_edges.gz", 0, 1);
  PrintGraphStatTable(G, "roadNet-PA", "Pennsylvania road network"); }
  { PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\Roards\\TX_edges.gz", 0, 1);
  PrintGraphStatTable(G, "roadNet-TX", "Texas road network"); }
}

// Arxiv collaboration networks
void MakeCollaborationNets() {
  { PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\Arxiv\\Arxiv-CoAuth\\astro-ph.coauth", 0, 1);
  PrintGraphStatTable(G, "CA-AstroPh", "Collaboration network of Arxiv Astro Physics category (there is an edge if authors coauthored at least one paper)"); }
  { PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\Arxiv\\Arxiv-CoAuth\\cond-mat.coauth", 0, 1);
  PrintGraphStatTable(G, "CA-CondMat", "Collaboration network of Arxiv Condensed Matter category (there is an edge if authors coauthored at least one paper)"); }
  { PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\Arxiv\\Arxiv-CoAuth\\gr-qc.coauth", 0, 1);
  PrintGraphStatTable(G, "CA-GrQc", "Collaboration network of Arxiv General Relativity category (there is an edge if authors coauthored at least one paper)"); }
  { PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\Arxiv\\Arxiv-CoAuth\\hep-ph.coauth", 0, 1);
  PrintGraphStatTable(G, "CA-HepPh", "Collaboration network of Arxiv High Energy Physics category (there is an edge if authors coauthored at least one paper)"); }
  { PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\Arxiv\\Arxiv-CoAuth\\hep-th.coauth", 0, 1);
  PrintGraphStatTable(G, "CA-HepTh", "Collaboration network of Arxiv High Energy Physics Theory category (there is an edge if authors coauthored at least one paper)"); }
}

// Arxiv citation networks
void MakeArxivCitationNets() {
  { PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\Arxiv\\hep-ph-citations", 0, 1);
  PrintGraphStatTable(G, "Cit-HepPh", "Paper citation network of Arxiv High Energy Physics category"); }
  { PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\Arxiv\\hep-th-citations", 0, 1);
  PrintGraphStatTable(G, "Cit-HepTh", "Paper citation network of Arxiv High Energy Physics Theory category"); }
}

// US Patent citation network
void MakePatentsCitationNet() {
  { PNGraph G = TSnap::LoadEdgeList<PNGraph>("W:\\Data\\_graphData\\Patents\\cite75_99.zip", 0, 1, ',');
  PrintGraphStatTable(G, "Cit-Patents", "US Patent citation network 1975-1999"); }
}

// Amazon product copurchasing networks
void MakeAmazonCoPurchNets() {
  { PNGraph G = TSnap::LoadEdgeListStr<PNGraph>("W:\\Data\\_graphData\\AmazonAsinNet\\2003_03_02_asin01.out", 0, 1);
  PrintGraphStatTable(G, "Amazon0302", "Amazon product co-purchaisng network from March 02 2003"); }
  { PNGraph G = TSnap::LoadEdgeListStr<PNGraph>("W:\\Data\\_graphData\\AmazonAsinNet\\2003_03_12_asin01.out", 0, 1);
  PrintGraphStatTable(G, "Amazon0312", "Amazon product co-purchaisng network from March 12 2003"); }
  { PNGraph G = TSnap::LoadEdgeListStr<PNGraph>("W:\\Data\\_graphData\\AmazonAsinNet\\2003_05_05_asin01.out", 0, 1);
  PrintGraphStatTable(G, "Amazon0505", "Amazon product co-purchaisng network from May 05 2003"); }
  { PNGraph G = TSnap::LoadEdgeListStr<PNGraph>("W:\\Data\\_graphData\\AmazonAsinNet\\2003_06_01_asin01.out", 0, 1);
  PrintGraphStatTable(G, "Amazon0601", "Amazon product co-purchaisng network from June 01 2003"); }
}

// Email networks
void MakeEmailNets() {
  { PNGraph G = TSnap::LoadEdgeListStr<PNGraph>("W:\\Data\\EmailGraph\\EMailTsactAnon.Txt", 1, 3);
  PrintGraphStatTable(G, "Email-EuAll", "Email network of a large European Research Institution (directed edge means at least one email was sent between October 2003 and March 2005)"); }
  { PNGraph G = TSnap::LoadEdgeListStr<PNGraph>("W:\\Data\\_graphData\\Enron\\Graph\\enron.directed.edges", 0, 1);
  PrintGraphStatTable(G, "Email-Enron", "Enron email network (edge indicated that email was exchanged, undirected edges)"); }
}

// Slashdot network
void MakeSlashdotNet(TStr InFNm, TStr OutFNm, TStr Desc) {
  TSsParser Ss(InFNm, ssfTabSep);
  PNGraph Graph = TNGraph::New();
  TStrHash<TInt> StrSet(Mega(1), true);
  while (Ss.Next()) {
    const int SrcNId = StrSet.AddKey(Ss[0]);
    if (! Graph->IsNode(SrcNId)) { Graph->AddNode(SrcNId); }
    for (int dst = 2; dst < Ss.Len(); dst++) {
      const int DstNId = StrSet.AddKey(Ss[dst]);
      if (! Graph->IsNode(DstNId)) { Graph->AddNode(DstNId); }
      Graph->AddEdge(SrcNId, DstNId);
    }
  }
  PrintGraphStatTable(Graph, OutFNm, Desc);
}

void MakeSlashdotSignNet(const TStr InFNm, TStr OutFNm, TStr Desc, THashSet<TChA> NIdSet) {
  //THashSet<TChA> NIdSet;
  TChA LnStr;
  TVec<char *> WrdV;
  int Sign;
  //PSignNet Net = TSignNet::New();
  TPt<TNodeEDatNet<TInt, TInt> >  Net = TNodeEDatNet<TInt, TInt>::New();
  int i = 0;
  for (TFIn FIn(InFNm); FIn.GetNextLn(LnStr); ) {
    if (LnStr.Empty() || LnStr[0]=='#') { continue; }
    LnStr.ToLc();
    TStrUtil::SplitOnCh(LnStr, WrdV, '\t', false);
    //NIdSet.AddKey(WrdV[0]);
    if (strcmp(WrdV[1], "friends")==0) { Sign = 1; }
    else if (strcmp(WrdV[1], "fans")==0) { continue; } // skip (fans are in-friends)
    else if (strcmp(WrdV[1], "foes")==0) { Sign = -1; } else { Fail; }
    const int SrcNId = NIdSet.AddKey(WrdV[0]);
    if (! Net->IsNode(SrcNId)) {
      Net->AddNode(SrcNId); }   
    for (int e = 2; e < WrdV.Len(); e++) {
      const int DstNId = NIdSet.AddKey(WrdV[e]);
      i ++ ;
      if ((SrcNId != DstNId) && ! Net->IsEdge(SrcNId, DstNId)) {
        if (! Net->IsNode(DstNId))
          Net->AddNode(DstNId);
        Net->AddEdge(SrcNId, DstNId, Sign);
      }
    }  
  }  
  TSnap::PrintInfo(Net, "Slashdot (" + TInt::GetStr(i) + ")");  

  // copied from gio.h - line 111
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "# Directed graph: %s\n", OutFNm.CStr());
  if (! Desc.Empty()) 
    fprintf(F, "# %s\n", (Desc).CStr());
    fprintf(F, "# Nodes: %d Edges: %d\n", Net->GetNodes(), Net->GetEdges());
    fprintf(F, "# UserId\tGroupId\tSign\n"); 
  for (TNodeEDatNet<TInt,TInt>::TEdgeI ei = Net->BegEI(); ei < Net->EndEI(); ei++) {
      fprintf(F, "%d\t%d\t%d\n", ei.GetSrcNId(), ei.GetDstNId(), ei()());
  }
  fclose(F);
  
  PrintGraphStatTable(Net, OutFNm, Desc);
}

void MakeSlasdotSignNets(){
  THashSet<TChA> NIdSet;
  MakeSlashdotSignNet("data/slashdot-08nov06.txt", "soc-sign-Slashdot081106", "Slashdot Zoo signed social network from November 6 2008", NIdSet);
  MakeSlashdotSignNet("data/slashdot-09feb16.txt", "soc-sign-Slashdot090216", "Slashdot Zoo signed social network from February 16 2009", NIdSet);
  MakeSlashdotSignNet("/u/ana/data/SlashdotZoo/slashdot-09feb21.txt", "soc-sign-Slashdot090221", "Slashdot Zoo signed social network from February 21 2009", NIdSet);
}

void MakeLJNets(TStr InFNm, TStr OutFNm, TStr Desc){
  TStrHash<TInt> StrSet(Mega(1), true);
  for (int i = 1; i < 13; i++){
    TStr tmp = "";  
      
    if (i < 10)
      tmp = InFNm + "ljgraph.0" + TInt::GetStr(i);  
    else
      tmp = InFNm + "ljgraph." + TInt::GetStr(i);  
  
    printf("%s\n",tmp());

    TSsParser Ss(tmp, ssfTabSep);
    PNGraph Graph = TNGraph::New();
      
    while (Ss.Next()) {
      const int SrcNId = StrSet.AddKey(Ss[0]);
      if (! Graph->IsNode(SrcNId)) { Graph->AddNode(SrcNId); }
      for (int dst = 2; dst < Ss.Len(); dst++) {
        TStr ls,rs;
          ((TStr)Ss[dst]).SplitOnCh(ls,' ',rs);
        if (ls == ">"){
          const int DstNId = StrSet.AddKey(rs);
          if (! Graph->IsNode(DstNId)) { Graph->AddNode(DstNId); }
          Graph->AddEdge(SrcNId, DstNId);
        }
      }
    }
    if (i < 10)
      OutFNm = "soc-lj-friends.0"+TInt::GetStr(i);
    else
      OutFNm = "soc-lj-friends."+TInt::GetStr(i);
  
    PrintGraphStatTable(Graph, OutFNm, Desc);
  }
}

void MakeLJGroupsNets(const TStr InFNm, TStr OutFNm, TStr Desc){
  TStrHash<TInt> StrSetU(Mega(1), true);
  TStrHash<TInt> StrSetG(Mega(1), true);
  
  for (int i = 1; i < 13; i++){
    TStr tmp = "";  
      
    if (i < 10)
      tmp = InFNm + "ljgraph.0" + TInt::GetStr(i);  
    else
      tmp = InFNm + "ljgraph." + TInt::GetStr(i);  
  
    printf("%s\n",tmp());

    TSsParser Ss(tmp, ssfTabSep);
    PNGraph Graph = TNGraph::New();
      
    while (Ss.Next()) {
      const int SrcNId = StrSetU.AddKey(Ss[0]);
      if (! Graph->IsNode(SrcNId)) 
         Graph->AddNode(SrcNId); 
      for (int dst = 2; dst < Ss.Len(); dst++) {
        TStr ls,rs;
          ((TStr)Ss[dst]).SplitOnCh(ls,' ',rs);
        if (ls == ">"){
          const int DstNId = StrSetU.AddKey(rs);
          if (! Graph->IsNode(DstNId)) { Graph->AddNode(DstNId); }
          Graph->AddEdge(SrcNId, DstNId);
        }
      }
    }
    if (i < 10)
      OutFNm = "soc-lj-friends.0"+TInt::GetStr(i);
    else
      OutFNm = "soc-lj-friends."+TInt::GetStr(i);
  
  //  PrintGraphStatTable(Graph, OutFNm, Desc);
  }
  
  for (int i = 5; i < 14; i++){
    TStr tmp = "";       
    
    tmp = InFNm + "ljcomm." + TInt::GetStr(i);  
    printf("%s\n",tmp());
    
    TSsParser Ss1(tmp, ssfTabSep);
  
    PNGraph Graph1 = TNGraph::New();
    PNGraph Graph2 = TNGraph::New();
  
    TSsParser Ss(tmp, ssfTabSep);    
    while (Ss.Next()) {
      const int SrcNId = StrSetU.AddKey(Ss[0]);
      if (! Graph1->IsNode(SrcNId)) { Graph1->AddNode(SrcNId); }
      if (! Graph2->IsNode(SrcNId)) { Graph2->AddNode(SrcNId); }
      
      for (int dst = 2; dst < Ss.Len(); dst++) {   
        TStr ls,rs;
        ((TStr)Ss[dst]).SplitOnCh(ls,' ',rs);            
        const int DstNId = StrSetG.AddKey(rs) + 10000000;
        if (! Graph1->IsNode(DstNId)) { Graph1->AddNode(DstNId); }
        if (! Graph2->IsNode(DstNId)) { Graph2->AddNode(DstNId); }
        if (ls == "<") // member
          Graph1->AddEdge(SrcNId, DstNId);  
        else if (ls == ">") // watching
          Graph2->AddEdge(SrcNId, DstNId);      
      }
    }
    OutFNm = "soc-lj-comm-";
    TStr s = "";
    if (i < 10)
      s = "0";
    
    PrintGraphStatTable(Graph1, OutFNm+"members-" + s + TInt::GetStr(i), Desc+" communities - members");
    PrintGraphStatTable(Graph2, OutFNm+"watchers-"+ s + TInt::GetStr(i), Desc+" communities - watchers");
  }
}

void MakeSignEpinions() {
  TSsParser Ss("/u/ana/data/EpinionRatings/user_rating.txt", ssfTabSep);
  //PSignNet Net = TSignNet::New();
  TPt<TNodeEDatNet<TInt, TInt> >  Net = TNodeEDatNet<TInt, TInt>::New();
  TStrHash<TInt> StrSet(Mega(1), true);
      
  while (Ss.Next()) {
    if ( ((TStr)Ss[0]).IsPrefix("#")  )
      continue;
    const int SrcNId = StrSet.AddKey(Ss[0]);
    const int DstNId = StrSet.AddKey(Ss[1]);
    if (! Net->IsNode(SrcNId)) { Net->AddNode(SrcNId); }
    if (! Net->IsNode(DstNId)) { Net->AddNode(DstNId); }
    const int Sign = ((TStr)Ss[2]).GetInt();
    Net->AddEdge(SrcNId, DstNId, Sign);
  }
    
  //    PrintGraphStatTable(Graph, OutFNm, Desc);
  TStr OutFNm = "soc-sign-epinions-user-ratings";
  TStr Desc = "Epinions signed social network";

  // copied from gio.h - line 111
  FILE *F = fopen(OutFNm.CStr(), "wt");
  fprintf(F, "# Directed graph: %s\n", OutFNm.CStr());
  if (! Desc.Empty()) 
    fprintf(F, "# %s\n", (Desc).CStr());
  fprintf(F, "# Nodes: %d Edges: %d\n", Net->GetNodes(), Net->GetEdges());
  fprintf(F, "# FromNodeId\tToNodeId\tSign\n"); 
  for (TNodeEDatNet<TInt,TInt>::TEdgeI ei = Net->BegEI(); ei < Net->EndEI(); ei++) {
      fprintf(F, "%d\t%d\t%d\n", ei.GetSrcNId(), ei.GetDstNId(), ei()());
  }
  fclose(F);
  
  PrintGraphStatTable(Net, OutFNm, Desc);
}


void MakeWikipediaNets() {
  { PNGraph G = TSnap::LoadEdgeListStr<PNGraph>("Wiki-Vote.txt", 0, 1);
  PrintGraphStatTable(G, "wiki-Vote", ""); }
  { PNGraph G = TSnap::LoadEdgeListStr<PNGraph>("Wiki-Talk.txt", 0, 1);
  PrintGraphStatTable(G, "wiki-Talk", ""); }
}

void MakeASNets() {
  { PUNGraph G = TSnap::LoadEdgeListStr<PUNGraph>("data/as20000102.txt", 0, 1);
  PrintGraphStatTable(G, "as01022000", "Autonomous Systems (from the BGP-Border Gateway Protocol logs) from Januar 02 2000"); }
}

void MakeASSkitterNets() {
  { PUNGraph G = TSnap::LoadEdgeListStr<PUNGraph>("as-skitter.txt", 0, 1);
  PrintGraphStatTable(G, "as-skitter", "Autonomous Systems (From traceroutes run daility in 2005 by skitter - http://www.caida.org/tools/measurement/skitter)"); 
  }
}

void MakeASCaidaNets() {
  { PNGraph G = TSnap::LoadEdgeListStr<PNGraph>("/u/ana/data/AS-Caida/dataweb/as-caida20071105.txt", 0, 1);
  PrintGraphStatTable(G , "as-caida20071105", "The CAIDA AS Relationships Dataset, from October 15 2007, http://www.caida.org/data/active/as-relationships/)"); } 
  { PNGraph G = TSnap::LoadEdgeListStr<PNGraph>("/u/ana/data/AS-Caida/dataweb/as-caida20070917.txt", 0, 1);
  PrintGraphStatTable(G , "as-caida20070917", "The CAIDA AS Relationships Dataset, from October 15 2007, http://www.caida.org/data/active/as-relationships/)"); } 
}


void MakeASOregonNets() {
  TStr files[18] = {"peer.oregon.010331","peer.oregon+.010331","peer.oregon.010407","peer.oregon+.010407","peer.oregon.010414","peer.oregon+.010414","peer.oregon.010421","peer.oregon+.010421","peer.oregon.010428","peer.oregon+.010428","peer.oregon.010505","peer.oregon+.010505","peer.oregon.010512","peer.oregon+.010512","peer.oregon.010519","peer.oregon+.010519","peer.oregon.010526","peer.oregon+.010526"};
  for (int i = 0; i < 18; i++){
    TStr tmp = ((TStr)"/u/ana/data/Oregon/") + files[i];
    PUNGraph G = TSnap::LoadEdgeList<PUNGraph>(tmp(), 0, 1, ':');
    if (files[i].GetCh(11) == '+'){
      files[i].DelSubStr(0,4);    
      files[i].DelSubStr(6,7);
      files[i].InsStr(6,"2_");    
      PrintGraphStatTable(G , files[i](), "AS peering information inferred from Oregon route-views BGP data, from ");  
    }
    else{
      files[i].DelSubStr(0,4);    
      files[i].DelSubStr(6,6);
      files[i].InsStr(6,"1_");
      PrintGraphStatTable(G , files[i](), "AS peering information inferred from Oregon route-views BGP data, from ");  
    }
  } 
  
 /* { PUNGraph G = TSnap::LoadEdgeListStr<PUNGraph>("data/peer.all020515.txt", 0, 1);
  PrintGraphStatTable(G , "peer.all020515", "AS peering information inferred from (1) Oregon route-views,(2) RIPE RIS BGP, (3) Looking glass data, and (4) Routing registry, all combined, May 15 2002"); } 
  { PUNGraph G = TSnap::LoadEdgeListStr<PUNGraph>("data/peer.ris020515.txt", 0, 1);
  PrintGraphStatTable(G , "peer.ris020515", "AS peering information inferred from RIPE RIS BGP data, May 15 2002"); } */
}

int main(int argc, char* argv[]) {
  //MakeEpinions();
  //MakeLiveJournal1();
  //MakeGnutella();
  //MakeRoadNets();
  //MakeWebGraphs();
  //MakeCollaborationNets();
  //MakeArxivCitationNets();
  //MakeAmazonCoPurchNets();
  //MakePatentsCitationNet();
  //MakeEmailNets();
  //MakeSlashdotNet("W:\\Data\\SlashdotZoo\\slashdot-08nov06.txt", "soc-Slashdot0811", "Slashdot Zoo social network from Noveber 6 2008");
  //MakeSlashdotNet("W:\\Data\\SlashdotZoo\\slashdot-09feb21.txt", "soc-Slashdot0902", "Slashdot Zoo social network from February 0 2009");
  //MakeWikipediaNets();
  //MakeASNets();
  //MakeASSkitterNets();
  //MakeASCaidaNets();
  //MakeASOregonNets();
  //MakeSlashdotSignNets();
//  MakeLJGroupsNets("/u/ana/data/LiveJournal/", "", "LiveJournal");
  //MakeLJNets("/u/ana/data/LiveJournal/", "", "Live Journal friends");
  //MakeSignEpinions();
  return 0;
}

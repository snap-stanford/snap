// pneanet.i
// Templates for SNAP TNEANet, PNEANet
//

/*
  Instanstiates templates from SNAP for inclusion in RINGO.
  Note in Vim, this replaces SNAP Template headers:
 
 :%s#^template.*<class PGraph> \S* \([^(]*\).*#%template(\1) TSnap::\1<PNEANet>;#gc
 :%s#^///.*\n:##g
*/

%extend TNEANet {
        TNEANetNodeI BegNI() {
          return TNEANetNodeI($self->BegNI());
        }
        TNEANetNodeI EndNI() {
          return TNEANetNodeI($self->EndNI());
        }
        TNEANetNodeI GetNI(const int &NId) {
          return TNEANetNodeI($self->GetNI(NId));
        }

  
        TNEANetEdgeI BegEI() {
          return TNEANetEdgeI($self->BegEI());
        }
        TNEANetEdgeI EndEI() {
          return TNEANetEdgeI($self->EndEI());
        }
  
        TNEANetAIntI BegNAIntI(const TStr& attr) {
          return TNEANetAIntI($self->BegNAIntI(attr));
        }
        TNEANetAIntI EndNAIntI(const TStr& attr) {
          return TNEANetAIntI($self->EndNAIntI(attr));
        }
  
        TNEANetAStrI BegNAStrI(const TStr& attr) {
          return TNEANetAStrI($self->BegNAStrI(attr));
        }
        TNEANetAStrI EndNAStrI(const TStr& attr) {
          return TNEANetAStrI($self->EndNAStrI(attr));
        }
  
        TNEANetAFltI BegNAFltI(const TStr& attr) {
          return TNEANetAFltI($self->BegNAFltI(attr));
        }
        TNEANetAFltI EndNAFltI(const TStr& attr) {
          return TNEANetAFltI($self->EndNAFltI(attr));
        }
  
        TNEANetAIntI BegEAIntI(const TStr& attr) {
          return TNEANetAIntI($self->BegEAIntI(attr));
        }
        TNEANetAIntI EndEAIntI(const TStr& attr) {
          return TNEANetAIntI($self->EndEAIntI(attr));
        }
        
        TNEANetAStrI BegEAStrI(const TStr& attr) {
          return TNEANetAStrI($self->BegEAStrI(attr));
        }
        TNEANetAStrI EndEAStrI(const TStr& attr) {
          return TNEANetAStrI($self->EndEAStrI(attr));
        }
        
        TNEANetAFltI BegEAFltI(const TStr& attr) {
          return TNEANetAFltI($self->BegEAFltI(attr));
        }
        TNEANetAFltI EndEAFltI(const TStr& attr) {
          return TNEANetAFltI($self->EndEAFltI(attr));
        }
  
};

// Convert a directed graph to a multi-edge attribute graph
//%template(ConvertGraphToPNEANet) ConvertGraph<PNEANet, PNGraph>;

// Use PNEANet as default function name.

%template(PrintGraphStatTable_PNEANet) PrintGraphStatTable<PNEANet>;

//%template(MxSccSz_PNEANet) TSnap::GetMxScc<PNEANet>;
//%template(MxWccSz_PNEANet) TSnap::GetMxWccSz<PNEANet>;
// End Basic Directed Graphs


// Basic PNEANets
%template(PNEANet) TPt< TNEANet >;


// gbase.h - PNEANet
%template(PrintInfo_PNEANet) TSnap::PrintInfo<PNEANet>;


// cncom.h - PNEANet
%template(GetNodeWcc_PNEANet) TSnap::GetNodeWcc<PNEANet>;
%template(IsConnected_PNEANet) TSnap::IsConnected<PNEANet>;
%template(IsWeaklyConn_PNEANet) TSnap::IsWeaklyConn<PNEANet>;
%template(GetWccSzCnt_PNEANet) TSnap::GetWccSzCnt<PNEANet>;
%template(GetWccs_PNEANet) TSnap::GetWccs<PNEANet>;
%template(GetSccSzCnt_PNEANet) TSnap::GetSccSzCnt<PNEANet>;
%template(GetSccs_PNEANet) TSnap::GetSccs<PNEANet>;
%template(GetMxWccSz_PNEANet) TSnap::GetMxWccSz<PNEANet>;
%template(GetMxSccSz_PNEANet) TSnap::GetMxSccSz<PNEANet>;

%template(GetMxWcc_PNEANet) TSnap::GetMxWcc<PNEANet>;
%template(GetMxScc_PNEANet) TSnap::GetMxScc<PNEANet>;
%template(GetMxBiCon_PNEANet) TSnap::GetMxBiCon<PNEANet>;

// centr.h - PNEANet
%template(GetNodeEcc_PNEANet) TSnap::GetNodeEcc<PNEANet>;
%template(GetPageRank_PNEANet) TSnap::GetPageRank<PNEANet>;
%template(GetHits_PNEANet) TSnap::GetHits<PNEANet>;


// alg.h - PNEANet
%template(CntInDegNodes_PNEANet) TSnap::CntInDegNodes<PNEANet>;
%template(CntOutDegNodes_PNEANet) TSnap::CntOutDegNodes<PNEANet>;
%template(CntDegNodes_PNEANet) TSnap::CntDegNodes<PNEANet>;
%template(CntNonZNodes_PNEANet) TSnap::CntNonZNodes<PNEANet>;
%template(CntEdgesToSet_PNEANet) TSnap::CntEdgesToSet<PNEANet>;

%template(GetMxDegNId_PNEANet) TSnap::GetMxDegNId<PNEANet>;
%template(GetMxInDegNId_PNEANet) TSnap::GetMxInDegNId<PNEANet>;
%template(GetMxOutDegNId_PNEANet) TSnap::GetMxOutDegNId<PNEANet>;

%template(GetInDegCnt_PNEANet) TSnap::GetInDegCnt<PNEANet>;
%template(GetOutDegCnt_PNEANet) TSnap::GetOutDegCnt<PNEANet>;
%template(GetDegCnt_PNEANet) TSnap::GetDegCnt<PNEANet>;
%template(GetDegSeqV_PNEANet) TSnap::GetDegSeqV<PNEANet>;

%template(GetNodeInDegV_PNEANet) TSnap::GetNodeInDegV<PNEANet>;
%template(GetNodeOutDegV_PNEANet) TSnap::GetNodeOutDegV<PNEANet>;

%template(CntUniqUndirEdges_PNEANet) TSnap::CntUniqUndirEdges<PNEANet>;
%template(CntUniqDirEdges_PNEANet) TSnap::CntUniqDirEdges<PNEANet>;
%template(CntUniqBiDirEdges_PNEANet) TSnap::CntUniqBiDirEdges<PNEANet>;
%template(CntSelfEdges_PNEANet) TSnap::CntSelfEdges<PNEANet>;

%template(GetUnDir_PNEANet) TSnap::GetUnDir<PNEANet>;
%template(MakeUnDir_PNEANet) TSnap::MakeUnDir<PNEANet>;
%template(AddSelfEdges_PNEANet) TSnap::AddSelfEdges<PNEANet>;
%template(DelSelfEdges_PNEANet) TSnap::DelSelfEdges<PNEANet>;
%template(DelNodes_PNEANet) TSnap::DelNodes<PNEANet>;
%template(DelZeroDegNodes_PNEANet) TSnap::DelZeroDegNodes<PNEANet>;
%template(DelDegKNodes_PNEANet) TSnap::DelDegKNodes<PNEANet>;
%template(IsTree_PNEANet) TSnap::IsTree<PNEANet>;
%template(GetTreeRootNId_PNEANet) TSnap::GetTreeRootNId<PNEANet>;
%template(GetTreeSig_PNEANet) TSnap::GetTreeSig<PNEANet>;


// bfsdfs.h - PNEANet
%template(GetBfsTree_PNEANet) TSnap::GetBfsTree<PNEANet>;
%template(GetSubTreeSz_PNEANet) TSnap::GetSubTreeSz<PNEANet>;
%template(GetNodesAtHop_PNEANet) TSnap::GetNodesAtHop<PNEANet>;
%template(GetNodesAtHops_PNEANet) TSnap::GetNodesAtHops<PNEANet>;
// Shortest paths
%template(GetShortPath_PNEANet) TSnap::GetShortPath<PNEANet>;
// Diameter
%template(GetBfsFullDiam_PNEANet) TSnap::GetBfsFullDiam<PNEANet>;
%template(GetBfsEffDiam_PNEANet) TSnap::GetBfsEffDiam<PNEANet>;


// drawgviz.h
%template(DrawGViz_PNEANet) TSnap::DrawGViz<PNEANet>;


// ggen.h
%template(GenGrid_PNEANet) TSnap::GenGrid<PNEANet>;
%template(GenStar_PNEANet) TSnap::GenStar<PNEANet>;
%template(GenCircle_PNEANet) TSnap::GenCircle<PNEANet>;
%template(GenFull_PNEANet) TSnap::GenFull<PNEANet>;
%template(GenTree_PNEANet) TSnap::GenTree<PNEANet>;
%template(GenBaraHierar_PNEANet) TSnap::GenBaraHierar<PNEANet>;
%template(GenRndGnm_PNEANet) TSnap::GenRndGnm<PNEANet>;


// gio.h
%template(LoadEdgeList_PNEANet) TSnap::LoadEdgeList<PNEANet>;
%template(LoadEdgeListStr_PNEANet) TSnap::LoadEdgeListStr<PNEANet>;
%template(LoadConnList_PNEANet) TSnap::LoadConnList<PNEANet>;
%template(LoadConnListStr_PNEANet) TSnap::LoadConnListStr<PNEANet>;
%template(LoadPajek_PNEANet) TSnap::LoadPajek<PNEANet>;
%template(SaveEdgeList_PNEANet) TSnap::SaveEdgeList<PNEANet>;
%template(SavePajek_PNEANet) TSnap::SavePajek<PNEANet>;
%template(SaveMatlabSparseMtx_PNEANet) TSnap::SaveMatlabSparseMtx<PNEANet>;
%template(SaveGViz_PNEANet) TSnap::SaveGViz<PNEANet>;


// kcore.h
%template(GetKCore_PNEANet) TSnap::GetKCore<PNEANet>;
%template(GetKCoreEdges_PNEANet) TSnap::GetKCoreEdges<PNEANet>;
%template(GetKCoreNodes_PNEANet) TSnap::GetKCoreNodes<PNEANet>;


// subgraph.h
%template(ConvertGraph_PNEANet_PNEANet) TSnap::ConvertGraph <PNEANet, PNEANet>;
%template(ConvertGraph_PNEANet_PNGraph) TSnap::ConvertGraph <PNEANet, PNGraph>;
%template(ConvertGraph_PNEANet_PUNGraph) TSnap::ConvertGraph <PNEANet, PUNGraph>;
%template(ConvertSubGraph_PNEANet_PNEANet) TSnap::ConvertSubGraph <PNEANet, PNEANet>;
%template(ConvertSubGraph_PNEANet_PNGraph) TSnap::ConvertSubGraph <PNEANet, PNGraph>;
%template(ConvertSubGraph_PNEANet_PUNGraph) TSnap::ConvertSubGraph <PNEANet, PUNGraph>;
%template(ConvertESubGraph_PNEANet_PNEANet) TSnap::ConvertESubGraph <PNGraph, PNEANet>;
%template(GetSubGraph_PNEANet) TSnap::GetSubGraph<PNEANet>;
%template(GetESubGraph_PNEANet) TSnap::GetESubGraph<PNEANet>;
%template(GetRndSubGraph_PNEANet) TSnap::GetRndSubGraph<PNEANet>;
%template(GetRndESubGraph_PNEANet) TSnap::GetRndESubGraph<PNEANet>;


// triad.h - PNEANet
%template(GetClustCf_PNEANet) TSnap::GetClustCf<PNEANet>;
%template(GetNodeClustCf_PNEANet) TSnap::GetNodeClustCf<PNEANet>;
%template(GetTriads_PNEANet) TSnap::GetTriads<PNEANet>;
%template(GetTriadEdges_PNEANet) TSnap::GetTriadEdges<PNEANet>;
//%template(GetNodeTriads_PNEANet) TSnap::GetNodeTriads<PNEANet>;
%template(GetTriadParticip_PNEANet) TSnap::GetTriadParticip<PNEANet>;

%template(GetCmnNbrs_PNEANet) TSnap::GetCmnNbrs<PNEANet>;
//%template(GetLen2Paths_PNEANet) TSnap::GetLen2Paths<PNEANet>;


// cmty.h - PNEANet
%template(GetModularity_PNEANet) TSnap::GetModularity<PNEANet>;
%template(GetEdgesInOut_PNEANet) TSnap::GetEdgesInOut<PNEANet>;


// anf.h - PNEANet
%template(GetAnf_PNEANet) TSnap::GetAnf<PNEANet>;
%template(GetAnfEffDiam_PNEANet) TSnap::GetAnfEffDiam<PNEANet>;
%template(TestAnf_PNEANet) TSnap::TestAnf<PNEANet>;


// statplot.h - PNEANet
%template(PlotKCoreEdges_PNEANet) TSnap::PlotKCoreEdges<PNEANet>;
%template(PlotKCoreNodes_PNEANet) TSnap::PlotKCoreNodes<PNEANet>;
%template(PlotShortPathDistr_PNEANet) TSnap::PlotShortPathDistr<PNEANet>;
%template(PlotHops_PNEANet) TSnap::PlotHops<PNEANet>;
%template(PlotClustCf_PNEANet) TSnap::PlotClustCf<PNEANet>;
%template(PlotSccDistr_PNEANet) TSnap::PlotSccDistr<PNEANet>;
%template(PlotWccDistr_PNEANet) TSnap::PlotWccDistr<PNEANet>;
%template(PlotOutDegDistr_PNEANet) TSnap::PlotOutDegDistr<PNEANet>;
%template(PlotInDegDistr_PNEANet) TSnap::PlotInDegDistr<PNEANet>;


// goodgraph.cpp - PNEANet
%template(PercentDegree_PNEANet) PercentDegree<PNEANet>;
%template(NodesGTEDegree_PNEANet) NodesGTEDegree<PNEANet>;
%template(MxDegree_PNEANet) MxDegree<PNEANet>;
%template(PercentMxWcc_PNEANet) PercentMxWcc<PNEANet>;
%template(PercentMxScc_PNEANet) PercentMxScc<PNEANet>;


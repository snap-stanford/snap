// pngraph.i
// Templates for SNAP TNGraph, PNGraph

%extend TNGraph {
        TNGraphNodeI BegNI() {
                return TNGraphNodeI($self->BegNI());
        }
        TNGraphNodeI EndNI() {
                return TNGraphNodeI($self->EndNI());
        }
        TNGraphNodeI GetNI(const int &NId) {
                return TNGraphNodeI($self->GetNI(NId));
        }
        TNGraphEdgeI BegEI() {
                return TNGraphEdgeI($self->BegEI());
        }
        TNGraphEdgeI EndEI() {
                return TNGraphEdgeI($self->EndEI());
        }
};

%pythoncode %{
# redefine TNGraphEdgeI.GetId to return a pair of nodes rather than -1
def GetId(self):
    return (self.GetSrcNId(), self.GetDstNId())

TNGraphEdgeI.GetId = GetId
%}


// Basic Undirected Graphs

%template(PrintGraphStatTable_PNGraph) PrintGraphStatTable<PNGraph>;

//%template(MxSccSz_PNGraph) TSnap::GetMxScc<PNGraph>;
//%template(MxWccSz_PNGraph) TSnap::GetMxWccSz<PNGraph>;
// End Basic Directed Graphs

// Basic PNGraphs
%template(PNGraph) TPt< TNGraph >;

// gbase.h - PNGraph
%template(PrintInfo_PNGraph) TSnap::PrintInfo<PNGraph>;

// cncom.h - PNGraph
%template(GetNodeWcc_PNGraph) TSnap::GetNodeWcc<PNGraph>;
%template(IsConnected_PNGraph) TSnap::IsConnected<PNGraph>;
%template(IsWeaklyConn_PNGraph) TSnap::IsWeaklyConn<PNGraph>;
%template(GetWccSzCnt_PNGraph) TSnap::GetWccSzCnt<PNGraph>;
%template(GetWccs_PNGraph) TSnap::GetWccs<PNGraph>;
%template(GetSccSzCnt_PNGraph) TSnap::GetSccSzCnt<PNGraph>;
%template(GetSccs_PNGraph) TSnap::GetSccs<PNGraph>;
%template(GetMxWccSz_PNGraph) TSnap::GetMxWccSz<PNGraph>;
%template(GetMxSccSz_PNGraph) TSnap::GetMxSccSz<PNGraph>;

%template(GetMxWcc_PNGraph) TSnap::GetMxWcc<PNGraph>;
%template(GetMxScc_PNGraph) TSnap::GetMxScc<PNGraph>;
%template(GetMxBiCon_PNGraph) TSnap::GetMxBiCon<PNGraph>;

// centr.h - PNGraph
%template(GetNodeEcc_PNGraph) TSnap::GetNodeEcc<PNGraph>;
%template(GetPageRank_PNGraph) TSnap::GetPageRank<PNGraph>;
%template(GetHits_PNGraph) TSnap::GetHits<PNGraph>;


// alg.h - PNGraph
%template(CntInDegNodes_PNGraph) TSnap::CntInDegNodes<PNGraph>;
%template(CntOutDegNodes_PNGraph) TSnap::CntOutDegNodes<PNGraph>;
%template(CntDegNodes_PNGraph) TSnap::CntDegNodes<PNGraph>;
%template(CntNonZNodes_PNGraph) TSnap::CntNonZNodes<PNGraph>;
%template(CntEdgesToSet_PNGraph) TSnap::CntEdgesToSet<PNGraph>;

%template(GetMxDegNId_PNGraph) TSnap::GetMxDegNId<PNGraph>;
%template(GetMxInDegNId_PNGraph) TSnap::GetMxInDegNId<PNGraph>;
%template(GetMxOutDegNId_PNGraph) TSnap::GetMxOutDegNId<PNGraph>;

%template(GetInDegCnt_PNGraph) TSnap::GetInDegCnt<PNGraph>;
%template(GetOutDegCnt_PNGraph) TSnap::GetOutDegCnt<PNGraph>;
%template(GetDegCnt_PNGraph) TSnap::GetDegCnt<PNGraph>;
%template(GetDegSeqV_PNGraph) TSnap::GetDegSeqV<PNGraph>;

%template(GetNodeInDegV_PNGraph) TSnap::GetNodeInDegV<PNGraph>;
%template(GetNodeOutDegV_PNGraph) TSnap::GetNodeOutDegV<PNGraph>;

%template(CntUniqUndirEdges_PNGraph) TSnap::CntUniqUndirEdges<PNGraph>;
%template(CntUniqDirEdges_PNGraph) TSnap::CntUniqDirEdges<PNGraph>;
%template(CntUniqBiDirEdges_PNGraph) TSnap::CntUniqBiDirEdges<PNGraph>;
%template(CntSelfEdges_PNGraph) TSnap::CntSelfEdges<PNGraph>;

%template(GetUnDir_PNGraph) TSnap::GetUnDir<PNGraph>;
%template(MakeUnDir_PNGraph) TSnap::MakeUnDir<PNGraph>;
%template(AddSelfEdges_PNGraph) TSnap::AddSelfEdges<PNGraph>;
%template(DelSelfEdges_PNGraph) TSnap::DelSelfEdges<PNGraph>;
%template(DelNodes_PNGraph) TSnap::DelNodes<PNGraph>;
%template(DelZeroDegNodes_PNGraph) TSnap::DelZeroDegNodes<PNGraph>;
%template(DelDegKNodes_PNGraph) TSnap::DelDegKNodes<PNGraph>;
%template(IsTree_PNGraph) TSnap::IsTree<PNGraph>;
%template(GetTreeRootNId_PNGraph) TSnap::GetTreeRootNId<PNGraph>;
%template(GetTreeSig_PNGraph) TSnap::GetTreeSig<PNGraph>;


// bfsdfs.h - PNGraph
%template(GetBfsTree_PNGraph) TSnap::GetBfsTree<PNGraph>;
%template(GetSubTreeSz_PNGraph) TSnap::GetSubTreeSz<PNGraph>;
%template(GetNodesAtHop_PNGraph) TSnap::GetNodesAtHop<PNGraph>;
%template(GetNodesAtHops_PNGraph) TSnap::GetNodesAtHops<PNGraph>;
// Shortest paths
%template(GetShortPath_PNGraph) TSnap::GetShortPath<PNGraph>;
// Diameter
%template(GetBfsFullDiam_PNGraph) TSnap::GetBfsFullDiam<PNGraph>;
%template(GetBfsEffDiam_PNGraph) TSnap::GetBfsEffDiam<PNGraph>;


// drawgviz.h
%template(DrawGViz_PNGraph) TSnap::DrawGViz<PNGraph>;


// ggen.h
%template(GenGrid_PNGraph) TSnap::GenGrid<PNGraph>;
%template(GenStar_PNGraph) TSnap::GenStar<PNGraph>;
%template(GenCircle_PNGraph) TSnap::GenCircle<PNGraph>;
%template(GenFull_PNGraph) TSnap::GenFull<PNGraph>;
%template(GenTree_PNGraph) TSnap::GenTree<PNGraph>;
%template(GenBaraHierar_PNGraph) TSnap::GenBaraHierar<PNGraph>;
%template(GenRndGnm_PNGraph) TSnap::GenRndGnm<PNGraph>;


// gio.h
%template(LoadEdgeList_PNGraph) TSnap::LoadEdgeList<PNGraph>;
%template(LoadEdgeListStr_PNGraph) TSnap::LoadEdgeListStr<PNGraph>;
%template(LoadConnList_PNGraph) TSnap::LoadConnList<PNGraph>;
%template(LoadConnListStr_PNGraph) TSnap::LoadConnListStr<PNGraph>;
%template(LoadPajek_PNGraph) TSnap::LoadPajek<PNGraph>;
%template(SaveEdgeList_PNGraph) TSnap::SaveEdgeList<PNGraph>;
%template(SavePajek_PNGraph) TSnap::SavePajek<PNGraph>;
%template(SaveMatlabSparseMtx_PNGraph) TSnap::SaveMatlabSparseMtx<PNGraph>;
%template(SaveGViz_PNGraph) TSnap::SaveGViz<PNGraph>;


// kcore.h
%template(GetKCore_PNGraph) TSnap::GetKCore<PNGraph>;
%template(GetKCoreEdges_PNGraph) TSnap::GetKCoreEdges<PNGraph>;
%template(GetKCoreNodes_PNGraph) TSnap::GetKCoreNodes<PNGraph>;


// subgraph.h
%template(ConvertGraph_PNGraph_PUNGraph) TSnap::ConvertGraph <PNGraph, PUNGraph>;
%template(ConvertGraph_PNGraph_PNGraph) TSnap::ConvertGraph <PNGraph, PNGraph>;
%template(ConvertGraph_PNGraph_PNEANet) TSnap::ConvertGraph <PNGraph, PNEANet>;
%template(ConvertSubGraph_PNGraph_PUNGraph) TSnap::ConvertSubGraph <PNGraph, PUNGraph>;
%template(ConvertSubGraph_PNGraph_PNGraph) TSnap::ConvertSubGraph <PNGraph, PNGraph>;
%template(ConvertSubGraph_PNGraph_PNEANet) TSnap::ConvertSubGraph <PNGraph, PNEANet>;
%template(ConvertESubGraph_PNGraph_PNEANet) TSnap::ConvertESubGraph <PNGraph, PNEANet>;
%template(GetSubGraph_PNGraph) TSnap::GetSubGraph<PNGraph>;
%template(GetRndSubGraph_PNGraph) TSnap::GetRndSubGraph<PNGraph>;
%template(GetRndESubGraph_PNGraph) TSnap::GetRndESubGraph<PNGraph>;


// triad.h - PNGraph
%template(GetClustCf_PNGraph) TSnap::GetClustCf<PNGraph>;
%template(GetNodeClustCf_PNGraph) TSnap::GetNodeClustCf<PNGraph>;
%template(GetTriads_PNGraph) TSnap::GetTriads<PNGraph>;
%template(GetTriadEdges_PNGraph) TSnap::GetTriadEdges<PNGraph>;
//%template(GetNodeTriads_PNGraph) TSnap::GetNodeTriads<PNGraph>;
%template(GetTriadParticip_PNGraph) TSnap::GetTriadParticip<PNGraph>;

%template(GetCmnNbrs_PNGraph) TSnap::GetCmnNbrs<PNGraph>;
//%template(GetLen2Paths_PNGraph) TSnap::GetLen2Paths<PNGraph>;


// cmty.h - PNGraph
%template(GetModularity_PNGraph) TSnap::GetModularity<PNGraph>;
%template(GetEdgesInOut_PNGraph) TSnap::GetEdgesInOut<PNGraph>;


// anf.h - PNGraph
%template(GetAnf_PNGraph) TSnap::GetAnf<PNGraph>;
%template(GetAnfEffDiam_PNGraph) TSnap::GetAnfEffDiam<PNGraph>;
%template(TestAnf_PNGraph) TSnap::TestAnf<PNGraph>;

// statplot.h - PNGraph
%template(PlotKCoreEdges_PNGraph) TSnap::PlotKCoreEdges<PNGraph>;
%template(PlotKCoreNodes_PNGraph) TSnap::PlotKCoreNodes<PNGraph>;
%template(PlotShortPathDistr_PNGraph) TSnap::PlotShortPathDistr<PNGraph>;
%template(PlotHops_PNGraph) TSnap::PlotHops<PNGraph>;
%template(PlotClustCf_PNGraph) TSnap::PlotClustCf<PNGraph>;
%template(PlotSccDistr_PNGraph) TSnap::PlotSccDistr<PNGraph>;
%template(PlotWccDistr_PNGraph) TSnap::PlotWccDistr<PNGraph>;
%template(PlotOutDegDistr_PNGraph) TSnap::PlotOutDegDistr<PNGraph>;
%template(PlotInDegDistr_PNGraph) TSnap::PlotInDegDistr<PNGraph>;


// goodgraph.cpp - PNGraph
%template(PercentDegree_PNGraph) PercentDegree<PNGraph>;
%template(NodesGTEDegree_PNGraph) NodesGTEDegree<PNGraph>;
%template(MxDegree_PNGraph) MxDegree<PNGraph>;
%template(PercentMxWcc_PNGraph) PercentMxWcc<PNGraph>;
%template(PercentMxScc_PNGraph) PercentMxScc<PNGraph>;


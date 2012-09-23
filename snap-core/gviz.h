/////////////////////////////////////////////////
/// GraphViz Layout Engines
typedef enum { gvlDot, gvlNeato, gvlTwopi, gvlCirco } TGVizLayout;

/////////////////////////////////////////////////
// GraphViz
namespace TSnap {
/// Draws a given Graph using a selected GraphViz Layout engine. Useful for drawing small (<100 node) graphs.
/// @param PltFNm Output filename (extension .ps, .png, .gif) determines the output format.
/// @param NIdColorH Maps node ids to node colors (see GraphViz documentation for more details).
template<class PGraph> void DrawGViz(const PGraph& Graph, const TGVizLayout& Layout, const TStr& PltFNm, const TStr& Desc=TStr(), const bool& NodeLabels=false, const TIntStrH& NIdColorH=TIntStrH());
/// Draws a given Graph using a selected GraphViz Layout engine. Useful for drawing small (<100 node) graphs.
/// @param PltFNm Output filename (extension .ps, .png, .gif) determines the output format.
/// @param NIdColorH Maps node ids to node colors (see GraphViz documentation for more details).
template<class PGraph> void DrawGViz(const PGraph& Graph, const TGVizLayout& Layout, const TStr& PltFNm, const TStr& Desc, const TIntStrH& NIdColorH);

namespace TSnapDetail {
/// Runs GraphViz layout engine over a graph saved in the file GraphInFNm with output saved to OutFNm.
void GVizDoLayout(const TStr& GraphInFNm, TStr OutFNm, const TGVizLayout& Layout);
/// Generates the GraphViz command string based on the selected Layout engine.
TStr GVizGetLayoutStr(const TGVizLayout& Layout);
} // namespace TSnapDetail

/////////////////////////////////////////////////
// GraphViz implementation

template<class PGraph>
void DrawGViz(const PGraph& Graph, const TGVizLayout& Layout, const TStr& PltFNm, const TStr& Desc, const bool& NodeLabels, const TIntStrH& NIdColorH) {
  const TStr Ext = PltFNm.GetFExt();
  const TStr GraphFNm = PltFNm.GetSubStr(0, PltFNm.Len()-Ext.Len()) + "dot";
  SaveGViz(Graph, GraphFNm, Desc, NodeLabels, NIdColorH);
  TSnap::TSnapDetail::GVizDoLayout(GraphFNm, PltFNm, Layout);
}

template<class PGraph>
void DrawGViz(const PGraph& Graph, const TGVizLayout& Layout, const TStr& PltFNm, const TStr& Desc, const TIntStrH& NodeLabelH) {
  const TStr Ext = PltFNm.GetFExt();
  const TStr GraphFNm = PltFNm.GetSubStr(0, PltFNm.Len()-Ext.Len()) + "dot";
  SaveGViz(Graph, GraphFNm, Desc, NodeLabelH);
  TSnap::TSnapDetail::GVizDoLayout(GraphFNm, PltFNm, Layout);
}

} // namespace TSNap

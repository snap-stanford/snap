/////////////////////////////////////////////////
// GraphViz Layout Engines
typedef enum { gvlDot, gvlNeato, gvlTwopi } TGVizLayout;

/////////////////////////////////////////////////
// GraphViz
namespace TGraphViz {

template<class PGraph> void SaveTxt(const PGraph& Graph, const TStr& OutFNm, const TStr& Desc=TStr(), const bool& NodeLabels=false, const TIntStrH& NIdColorH=TIntStrH());
template<class PGraph> void SaveTxt(const PGraph& Graph, const TStr& OutFNm, const TStr& Desc, const TIntStrH& NodeLabelH);
template<class PGraph> void Plot(const PGraph& Graph, const TGVizLayout& Layout, const TStr& PltFNm, const TStr& Desc=TStr(), const bool& NodeLabels=false, const TIntStrH& NIdColorH=TIntStrH());
template<class PGraph> void Plot(const PGraph& Graph, const TGVizLayout& Layout, const TStr& PltFNm, const TStr& Desc, const TIntStrH& NIdColorH);
void DoLayout(const TStr& GraphInFNm, TStr OutFNm, const TGVizLayout& Layout);
TStr GetLayoutStr(const TGVizLayout& Layout);

/////////////////////////////////////////////////
// GraphViz implamentations
template<class PGraph>
void SaveTxt(const PGraph& Graph, const TStr& OutFNm, const TStr& Desc, const bool& NodeLabels, const TIntStrH& NIdColorH) {
  const bool IsDir = HasGraphFlag(typename PGraph::TObj, gfDirected);
  FILE *F = fopen(OutFNm.CStr(), "wt");
  if (! Desc.Empty()) fprintf(F, "/*****\n%s\n*****/\n\n", Desc.CStr());
  if (IsDir) { fprintf(F, "digraph G {\n"); } else { fprintf(F, "graph G {\n"); }
  fprintf(F, "  graph [splines=false overlap=false]\n"); //size=\"12,10\" ratio=fill
  // node  [width=0.3, height=0.3, label=\"\", style=filled, color=black]
  // node  [shape=box, width=0.3, height=0.3, label=\"\", style=filled, fillcolor=red]
  fprintf(F, "  node  [shape=ellipse, width=0.3, height=0.3%s]\n", NodeLabels?"":", label=\"\"");
  // node colors
  //for (int i = 0; i < NIdColorH.Len(); i++) {
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (NIdColorH.IsKey(NI.GetId())) {
      fprintf(F, "  %d [style=filled, fillcolor=\"%s\"];\n", NI.GetId(), NIdColorH.GetDat(NI.GetId()).CStr()); }
    else {
      fprintf(F, "  %d ;\n", NI.GetId());
    }
  }
  // edges
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (NI.GetOutDeg()==0 && NI.GetInDeg()==0 && !NIdColorH.IsKey(NI.GetId())) { 
      fprintf(F, "%d;\n", NI.GetId()); }
    else {
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        if (! IsDir && NI.GetId() > NI.GetOutNId(e)) { continue; }
        fprintf(F, "  %d %s %d;\n", NI.GetId(), IsDir?"->":"--", NI.GetOutNId(e)); 
      }
    }
  }
  if (! Desc.Empty()) {
    fprintf(F, "  label = \"\\n%s\\n\";", Desc.CStr());
    fprintf(F, "  fontsize=24;\n");
  }
  fprintf(F, "}\n");
  fclose(F);
}

template<class PGraph>
void SaveTxt(const PGraph& Graph, const TStr& OutFNm, const TStr& Desc, const TIntStrH& NodeLabelH) {
  const bool IsDir = Graph->HasFlag(gfDirected);
  FILE *F = fopen(OutFNm.CStr(), "wt");
  if (! Desc.Empty()) fprintf(F, "/*****\n%s\n*****/\n\n", Desc.CStr());
  if (IsDir) { fprintf(F, "digraph G {\n"); } else { fprintf(F, "graph G {\n"); }
  fprintf(F, "  graph [splines=true overlap=false]\n"); //size=\"12,10\" ratio=fill
  fprintf(F, "  node  [shape=ellipse, width=0.3, height=0.3]\n");
  // node colors
  //for (int i = 0; i < NodeLabelH.Len(); i++) {
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    fprintf(F, "  %d [label=\"%s\"];\n", NI.GetId(), NodeLabelH.GetDat(NI.GetId()).CStr()); 
}
  // edges
  for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
    if (NI.GetOutDeg()==0 && NI.GetInDeg()==0 && ! NodeLabelH.IsKey(NI.GetId())) {
      fprintf(F, "%d;\n", NI.GetId()); }
    else {
      for (int e = 0; e < NI.GetOutDeg(); e++) {
        if (! IsDir && NI.GetId() > NI.GetOutNId(e)) { continue; }
        fprintf(F, "  %d %s %d;\n", NI.GetId(), IsDir?"->":"--", NI.GetOutNId(e));
      }
    }
  }
  if (! Desc.Empty()) {
    fprintf(F, "  label = \"\\n%s\\n\";", Desc.CStr());
    fprintf(F, "  fontsize=24;\n");
  }
  fprintf(F, "}\n");
  fclose(F);
}

template<class PGraph>
void Plot(const PGraph& Graph, const TGVizLayout& Layout, const TStr& PltFNm, const TStr& Desc, const bool& NodeLabels, const TIntStrH& NIdColorH) {
  const TStr Ext = PltFNm.GetFExt();
  const TStr GraphFNm = PltFNm.GetSubStr(0, PltFNm.Len()-Ext.Len()) + "dot";
  SaveTxt(Graph, GraphFNm, Desc, NodeLabels, NIdColorH);
  DoLayout(GraphFNm, PltFNm, Layout);
}

template<class PGraph>
void Plot(const PGraph& Graph, const TGVizLayout& Layout, const TStr& PltFNm, const TStr& Desc, const TIntStrH& NodeLabelH) {
  const TStr Ext = PltFNm.GetFExt();
  const TStr GraphFNm = PltFNm.GetSubStr(0, PltFNm.Len()-Ext.Len()) + "dot";
  SaveTxt(Graph, GraphFNm, Desc, NodeLabelH);
  DoLayout(GraphFNm, PltFNm, Layout);
}

} // TGraphViz

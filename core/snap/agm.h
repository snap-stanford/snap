#ifndef snaplib_agm_h
#define snaplib_agm_h
#include "./Snap.h"

class TAGM{
public:
	static void RndConnectInsideCommunity(PUNGraph& Graph, const TIntV& CmtyV, const double& Prob, TRnd& Rnd);
	static PUNGraph GenAGM(TVec<TIntV >& CmtyVV, const double& DensityCoef, const double& ScaleCoef, TRnd& Rnd=TInt::Rnd);
	static void GetNodeMembership(THash<TInt,TIntV >& NIDComVH, const THash<TInt,TIntV >& CmtyVH);
	static void GetNodeMembership(THash<TInt,TIntV >& NIDComVH, const TVec<TIntV >& CmtyVV);
	template<class PGraph>
	static void GVizComGraph(const PGraph& Graph,const TVec<TIntV >& CmtyVV, const TStr& OutFNm, const TStr& Desc = TStr()){
		TStrV Colors = TStrV::GetV("red","blue","green","pink","cyan");
		TStrV Shapes = TStrV::GetV("ellipse","triangle","square","pentagon","hexagon");
		THash<TInt,TIntV> NIDComVH;
		GetNodeMembership(NIDComVH,CmtyVV);

	  const TStr Ext = OutFNm.GetFExt();
	  const TStr GraphFNm = OutFNm.GetSubStr(0, OutFNm.Len()-Ext.Len()) + "dot";
		const bool IsDir = HasGraphFlag(typename PGraph::TObj, gfDirected);
		FILE *F = fopen(GraphFNm.CStr(), "wt");
		if (! Desc.Empty()) fprintf(F, "/*****\n%s\n*****/\n\n", Desc.CStr());
		if (IsDir) { fprintf(F, "digraph G {\n"); } else { fprintf(F, "graph G {\n"); }
		fprintf(F, "  graph [splines=false overlap=false]\n"); //size=\"12,10\" ratio=fill
		// node  [width=0.3, height=0.3, label=\"\", style=filled, color=black]
		// node  [shape=box, width=0.3, height=0.3, label=\"\", style=filled, fillcolor=red]
		fprintf(F, "  node  [width=0.3, height=0.3]\n");
		// node colors
		//for (int i = 0; i < NIdColorH.Len(); i++) {
		for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
			int NID = NI.GetId();
			TIntV& CIDV = NIDComVH.GetDat(NID);
			IAssert(CIDV.Len()>0);
			TStr ShapeNm = Shapes[(CIDV.Len()-1)%Shapes.Len()];
			TStr ColorNm = Colors[CIDV[0]%Colors.Len()];
			TStr NodeComLabel = TStr::Fmt("%d(",NID);
			for(int i=0;i<CIDV.Len();i++) {
				TStr TmpStr = TStr::Fmt("%d",int(CIDV[i]));
				NodeComLabel += TmpStr;
				if(i<CIDV.Len()-1){NodeComLabel+=",";}
			}
			NodeComLabel += ")";
			fprintf(F, "  %d [style=filled, shape=\"%s\" fillcolor=\"%s\" label=\"%s\"];\n", NI.GetId(), ShapeNm.CStr(),ColorNm.CStr(), NodeComLabel.CStr()); 
		}
	
		// edges
		for (typename PGraph::TObj::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++) {
			if (NI.GetOutDeg()==0 && NI.GetInDeg()==0  ) { 
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
		TGraphViz::DoLayout(GraphFNm, OutFNm, gvlNeato);
	}
};
#endif
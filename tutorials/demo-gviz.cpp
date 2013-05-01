#include "Snap.h"

#define DIRNAME "graphviz"

// Demo drawing of graph with nodes labeled with text
void DrawGVizDemo() {

  PUNGraph UNGraph1;
  UNGraph1 = TSnap::GenRndGnm<PUNGraph>(30, 40);
  
  PNGraph NGraph1;
  NGraph1 = TSnap::GenRndGnm<PNGraph>(30, 40);
  
  TStrV LNames; //  gvlDot, gvlNeato, gvlTwopi, gvlCirco
  LNames.Add("Dot");
  LNames.Add("Neato");
  LNames.Add("Twopi");
  LNames.Add("Circo");
  
  TStrV Exts;
  Exts.Add("ps");
  Exts.Add("gif");
  
  Exts.Add("png");
  
  for (int i = 0; i < LNames.Len(); i++) {
    for (int e = 0; e < Exts.Len(); e++) {
      for (int d = 0; d < 2; d++) {
        // Baseline file is already created (use as benchmark)
        TStr FNameDemo = TStr::Fmt("%s/demo_%s_%s.%s", DIRNAME,
                                   d ? "ngraph" : "ungraph" ,
                                   LNames[i].CStr(), Exts[e].CStr());
                
        // Remove test graph if it already exists
        remove(FNameDemo.CStr());
        
        // Draw new graph
        if (d) {
          TSnap::DrawGViz(NGraph1, TGVizLayout(i), FNameDemo, LNames[i], true);
        }
        else {
          TSnap::DrawGViz(UNGraph1, TGVizLayout(i), FNameDemo, LNames[i], true);
        }
        printf("Drawing graph '%s'\n", FNameDemo.CStr());
      }
    }
  }
  
}

// Demo drawing of graph with nodes marked with colors
void DrawGVizColorDemo() {
    
  PUNGraph UNGraph1;
  UNGraph1 = TSnap::GenRndGnm<PUNGraph>(30, 40);
  
  PNGraph NGraph1;
  NGraph1 = TSnap::GenRndGnm<PNGraph>(30, 40);
  
  TStrV LNames; //  gvlDot, gvlNeato, gvlTwopi, gvlCirco
  LNames.Add("Dot");
  LNames.Add("Neato");
  LNames.Add("Twopi");
  LNames.Add("Circo");
  
  TStrV Exts;
  Exts.Add("ps");
  Exts.Add("gif");
  Exts.Add("png");
  
  TStrH Colors;
  Colors.AddDat("white");
  Colors.AddDat("black");
  Colors.AddDat("red");
  Colors.AddDat("green");
  Colors.AddDat("blue");
  Colors.AddDat("yellow");
  Colors.AddDat("magenta");
  Colors.AddDat("cyan");
  Colors.AddDat("magenta");

  TIntStrH NIdColorH;
  
  for (TUNGraph::TNodeI NI = UNGraph1->BegNI(); NI < UNGraph1->EndNI(); NI++) {
    int RVal = TInt::Rnd.GetUniDevInt(0, Colors.Len()-1);
    NIdColorH.AddDat(NI.GetId(), Colors.GetKey(RVal));
  }
  
  for (int i = 0; i < LNames.Len(); i++) {
    for (int e = 0; e < Exts.Len(); e++) {
      for (int d = 0; d < 2; d++) {

        TStr FName = TStr::Fmt("%s/demo_%s_%s_col.%s", DIRNAME, d ? "ngraph" : "ungraph" , LNames[i].CStr(), Exts[e].CStr());
        
        // Remove demo graph if it already exists
        remove(FName.CStr());
        
        // Draw new graph with specified layout and extension
        if (d) {
          TSnap::DrawGViz(NGraph1, TGVizLayout(i), FName, LNames[i], true, NIdColorH);
        }
        else {
          TSnap::DrawGViz(UNGraph1, TGVizLayout(i), FName, LNames[i], true, NIdColorH);
        }
        printf("Drawing graph '%s'\n", FName.CStr());
      }
    }
  }
  
}

int main(int argc, char* argv[]) {
  mkdir(DIRNAME, S_IRWXU | S_IRWXG | S_IRWXO);

  DrawGVizDemo();
  DrawGVizColorDemo();
}

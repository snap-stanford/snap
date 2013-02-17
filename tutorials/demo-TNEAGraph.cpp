#include "Snap.h"

int main(int argc, char* argv[]) {
  //DefaultConstructor();
  //ManipulateNodesEdges();
  //GetSmallGraph();
  TNEAGraph *p;
  TStr attr1 = "double";
  TStr attr2 = "triple";
  TStr attr3 = "double-float";
  p = new TNEAGraph();
  p->AddNode();
  p->AddNode();
  p->AddNode();
  for (TNEAGraph::TNodeI NI = p->BegNI(); NI < p->EndNI(); NI++) {
    printf("Node id: %d\n", NI.GetId());
    p->AddIntAttrDat(NI.GetId(), NI.GetId()*2, attr1);
    p->AddIntAttrDat(NI.GetId(), NI.GetId()*3, attr2);
    p->AddFltAttrDat(NI.GetId(), (float) (NI.GetId()*2), attr3);
    //    p->AddStrAttrDat(NI.GetId(), "Test value", attr1);
  }
  p->AddNode();
  for (TNEAGraph::TNodeI NI = p->BegNI(); NI < p->EndNI(); NI++) {
    printf("Node id: %d, value: %d\n", NI.GetId(), p->GetIntAttrDat(NI.GetId(), attr1));
    printf("Node id: %d, value: %d\n", NI.GetId(), p->GetIntAttrDat(NI.GetId(), attr2));
    printf("Node id: %d, value: %f\n", NI.GetId(), p->GetFltAttrDat(NI.GetId(), attr3));
    //    printf("Node id: %d, value: %s\n", NI.GetId(), p->GetStrAttrDat(NI.GetId(), attr1));
  }
  printf("Yay!\n");
}


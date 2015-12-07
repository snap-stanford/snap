#include "Snap.h"

// Test attribute functionality
void ManipulateSparseAttributes() {
  TAttr Attrs;

  // Add mappings for three attributes
  TStr IntAttr("TestInt");
  TInt IntId;
  Attrs.AddSAttr(IntAttr, atInt, IntId);
  printf("Added attribute %s with id %d\n", IntAttr.CStr(), IntId.Val);

  TStr FltAttr("TestFlt");
  TInt FltId;
  Attrs.AddSAttr(FltAttr, atFlt, FltId);
  printf("Added attribute %s with id %d\n", FltAttr.CStr(), FltId.Val);

  TStr StrAttr("TestStr");
  TInt StrId;
  Attrs.AddSAttr(StrAttr, atStr, StrId);
  printf("Added attribute %s with id %d\n", StrAttr.CStr(), StrId.Val);

  // Add values for attributes to id NId.
  TInt NId(0);
  TInt IntVal(5);
  Attrs.AddSAttrDat(NId, IntId, IntVal);
  TFlt FltVal(5.0);
  Attrs.AddSAttrDat(NId, FltId, FltVal);
  TStr StrVal("5");
  Attrs.AddSAttrDat(NId, StrId, StrVal);


  // Get values for the attributes for id NId.
  TInt IntVal2;
  Attrs.GetSAttrDat(NId, IntId, IntVal2);
  printf("Node %d has attribute, with id %d, with value %d.\n", NId.Val, IntId.Val, IntVal2.Val);
  TFlt FltVal2;
  Attrs.GetSAttrDat(NId, FltAttr, FltVal2);
  printf("Node %d has attribute, with id %d, with value %f.\n", NId.Val, FltId.Val, FltVal2.Val);
  TStr StrVal2;
  Attrs.GetSAttrDat(NId, StrId, StrVal2);
  printf("Node %d has attribute, with id %d, with value %s.\n", NId.Val, StrId.Val, StrVal2.CStr());


  // Get list of attributes for id NId.
  TAttrPrV AttrV;
  Attrs.GetSAttrV(NId, atInt, AttrV);
  printf("ID %d has %d int attributes.\n", NId.Val, AttrV.Len());
  Attrs.GetSAttrV(NId, atFlt, AttrV);
  printf("ID %d has %d flt attributes.\n", NId.Val, AttrV.Len());
  Attrs.GetSAttrV(NId, atStr, AttrV);
  printf("ID %d has %d str attributes.\n", NId.Val, AttrV.Len());
  //Attrs.GetSAttrV(NId, atAny, AttrV);
  //printf("ID %d has %d attributes.\n", NId.Val, AttrV.Len());

  // Delete all attributes for id NId (use either name or id).
  Attrs.DelSAttrDat(NId, IntAttr);
  Attrs.DelSAttrDat(NId, FltId);
  Attrs.DelSAttrDat(NId, StrAttr);

  //Get all ids with given attribute
  for (int i = 1; i <= 10; i++) {
    TInt Id(i);
    Attrs.AddSAttrDat(Id, IntId, IntVal);
    if (i%2 == 0) {
      Attrs.AddSAttrDat(Id, FltId, FltVal);
    }
  }
  Attrs.AddSAttrDat(NId, StrId, StrVal);

  TIntV IdV;
  Attrs.GetIdVSAttr(IntAttr, IdV);
  printf("%d ids have attribute with name %s\n", IdV.Len(), IntAttr.CStr());

  Attrs.GetIdVSAttr(FltAttr, IdV);
  printf("%d ids have attribute with name %s\n", IdV.Len(), FltAttr.CStr());

  Attrs.GetIdVSAttr(StrAttr, IdV);
  printf("%d ids have attribute with name %s\n", IdV.Len(), StrAttr.CStr());
}

// Test pair sparse attribute functionality
void ManipulateSparseAttributesPair() {
  TAttrPair Attrs;
  int SrcNId = 0;
  int DstNId = 1;
  TIntPr EId(SrcNId, DstNId);

  // Add mappings for three attributes
  TStr IntAttr("TestInt");
  TInt IntId;
  Attrs.AddSAttr(IntAttr, atInt, IntId);
  printf("Added attribute %s with id %d\n", IntAttr.CStr(), IntId.Val);

  TStr FltAttr("TestFlt");
  TInt FltId;
  Attrs.AddSAttr(FltAttr, atFlt, FltId);
  printf("Added attribute %s with id %d\n", FltAttr.CStr(), FltId.Val);

  TStr StrAttr("TestStr");
  TInt StrId;
  Attrs.AddSAttr(StrAttr, atStr, StrId);
  printf("Added attribute %s with id %d\n", StrAttr.CStr(), StrId.Val);

  // Add values for attributes to id NId.
  TInt IntVal(5);
  Attrs.AddSAttrDat(EId, IntId, IntVal);
  TFlt FltVal(5.0);
  Attrs.AddSAttrDat(EId, FltId, FltVal);
  TStr StrVal("5");
  Attrs.AddSAttrDat(EId, StrId, StrVal);


  // Get values for the attributes for id NId.
  TInt IntVal2;
  Attrs.GetSAttrDat(EId, IntId, IntVal2);
  printf("ID (%d, %d) has attribute, with id %d, with value %d.\n", SrcNId, DstNId, IntId.Val, IntVal2.Val);
  TFlt FltVal2;
  Attrs.GetSAttrDat(EId, FltAttr, FltVal2);
  printf("ID (%d, %d) has attribute, with id %d, with value %f.\n", SrcNId, DstNId, FltId.Val, FltVal2.Val);
  TStr StrVal2;
  Attrs.GetSAttrDat(EId, StrId, StrVal2);
  printf("ID (%d, %d) has attribute, with id %d, with value %s.\n", SrcNId, DstNId, StrId.Val, StrVal2.CStr());


  // Get list of attributes for id NId.
  TAttrPrV AttrV;
  Attrs.GetSAttrV(EId, atInt, AttrV);
  printf("ID (%d, %d) has %d int attributes.\n", SrcNId, DstNId, AttrV.Len());
  Attrs.GetSAttrV(EId, atFlt, AttrV);
  printf("ID (%d, %d) has %d flt attributes.\n", SrcNId, DstNId, AttrV.Len());
  Attrs.GetSAttrV(EId, atStr, AttrV);
  printf("ID (%d, %d) has %d str attributes.\n", SrcNId, DstNId, AttrV.Len());
  //Attrs.GetSAttrV(EId, atAny, AttrV);
  //printf("ID (%d, %d) has %d attributes.\n", SrcNId, DstNId, AttrV.Len());

  // Delete all attributes for id NId (use either name or id).
  Attrs.DelSAttrDat(EId, IntAttr);
  Attrs.DelSAttrDat(EId, FltId);
  Attrs.DelSAttrDat(EId, StrAttr);

  //Get all ids with given attribute
  for (int i = 2; i < 12; i++) {
    TIntPr EId2(i-1, i);
    Attrs.AddSAttrDat(EId2, IntId, IntVal);
    if (i%2 == 0) {
      Attrs.AddSAttrDat(EId2, FltId, FltVal);
    }
  }
  Attrs.AddSAttrDat(EId, StrId, StrVal);

  TIntPrV IdV;
  Attrs.GetIdVSAttr(IntAttr, IdV);
  printf("%d ids have attribute with name %s\n", IdV.Len(), IntAttr.CStr());

  Attrs.GetIdVSAttr(FltAttr, IdV);
  printf("%d ids have attribute with name %s\n", IdV.Len(), FltAttr.CStr());

  Attrs.GetIdVSAttr(StrAttr, IdV);
  printf("%d ids have attribute with name %s\n", IdV.Len(), StrAttr.CStr());
}

int main(int argc, char* argv[]) {
  ManipulateSparseAttributes();
  ManipulateSparseAttributesPair();
}

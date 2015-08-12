#include <gtest/gtest.h>

#include "Snap.h"

TEST(TSparseAttrSingle, AddSAttr) {
  TSparseAttrSingle Attrs;
  TInt AttrId;
  int status = Attrs.AddSAttr("TestInt", atInt, AttrId);
  EXPECT_EQ(0, status);
  EXPECT_EQ(0, AttrId.Val);
  status = Attrs.AddSAttr("TestFlt", atFlt, AttrId);
  EXPECT_EQ(0, status);
  EXPECT_EQ(1, AttrId.Val);
  status = Attrs.AddSAttr("TestStr", atStr, AttrId);
  EXPECT_EQ(0, status);
  EXPECT_EQ(2, AttrId.Val);
  status = Attrs.AddSAttr("TestAny", atAny, AttrId);
  EXPECT_EQ(-1, status);
}

TEST(TSparseAttrSingle, GetSAttrId) {
  TSparseAttrSingle Attrs;
  TInt AttrId;
  Attrs.AddSAttr("TestInt", atInt, AttrId);
  Attrs.AddSAttr("TestFlt", atFlt, AttrId);
  Attrs.AddSAttr("TestStr", atStr, AttrId);
  TAttrType AttrType;
  int status = Attrs.GetSAttrId(TStr("TestInt"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atInt, AttrType);
  EXPECT_EQ(0, AttrId.Val);
  status = Attrs.GetSAttrId(TStr("TestFlt"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atFlt, AttrType);
  EXPECT_EQ(1, AttrId.Val);
  status = Attrs.GetSAttrId(TStr("TestStr"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atStr, AttrType);
  EXPECT_EQ(2, AttrId.Val);
  status = Attrs.GetSAttrId(TStr("TestError"), AttrId, AttrType);
  EXPECT_EQ(-1, status);
}

TEST(TSparseAttrSingle, GetSAttrName) {
  TSparseAttrSingle Attrs;
  TInt AttrId;
  Attrs.AddSAttr("TestInt", atInt, AttrId);
  Attrs.AddSAttr("TestFlt", atFlt, AttrId);
  Attrs.AddSAttr("TestStr", atStr, AttrId);
  TAttrType AttrType;
  TStr Name;
  int status = Attrs.GetSAttrName(0, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atInt, AttrType);
  EXPECT_STREQ("TestInt", Name.CStr());
  status = Attrs.GetSAttrName(1, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atFlt, AttrType);
  EXPECT_STREQ("TestFlt", Name.CStr());
  status = Attrs.GetSAttrName(2, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atStr, AttrType);
  EXPECT_STREQ("TestStr", Name.CStr());
  status = Attrs.GetSAttrName(3, Name, AttrType);
  EXPECT_EQ(-1, status);
}

TEST(TSparseAttrSingle, AddSAttrDat_int) {
  TSparseAttrSingle Attrs;
  TInt Val(5);
  TInt Id(0);
  int status = Attrs.AddSAttrDat(Id, 1, Val);
  EXPECT_EQ(-1, status);
  TInt AttrId;
  TStr AttrName("TestInt");
  Attrs.AddSAttr(AttrName, atInt, AttrId);
  TFlt ErrorVal(1);
  status = Attrs.AddSAttrDat(Id, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Attrs.AddSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Attrs.AddSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestInt2");
  status = Attrs.AddSAttrDat(Id, NewName, Val);
  EXPECT_EQ(0, status);
}

TEST(TSparseAttrSingle, AddSAttrDat_flt) {
  TSparseAttrSingle Attrs;
  TFlt Val(5.0);
  TInt Id(0);
  int status = Attrs.AddSAttrDat(Id, 1, Val);
  EXPECT_EQ(-1, status);
  TInt AttrId;
  TStr AttrName("TestFlt");
  Attrs.AddSAttr(AttrName, atFlt, AttrId);
  TInt ErrorVal(1);
  status = Attrs.AddSAttrDat(Id, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Attrs.AddSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Attrs.AddSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestFlt2");
  status = Attrs.AddSAttrDat(Id, NewName, Val);
  EXPECT_EQ(0, status);
}

TEST(TSparseAttrSingle, AddSAttrDat_str) {
  TSparseAttrSingle Attrs;
  TStr Val("5");
  TInt Id(0);
  int status = Attrs.AddSAttrDat(Id, 1, Val);
  EXPECT_EQ(-1, status);
  TInt AttrId;
  TStr AttrName("TestFlt");
  Attrs.AddSAttr(AttrName, atStr, AttrId);
  TInt ErrorVal(1);
  status = Attrs.AddSAttrDat(Id, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Attrs.AddSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Attrs.AddSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestStr2");
  status = Attrs.AddSAttrDat(Id, NewName, Val);
  EXPECT_EQ(0, status);
}

TEST(TSparseAttrSingle, GetSAttrDat_int) {
  TSparseAttrSingle Attrs;
  TInt Val;
  TInt AttrId(0);
  TStr AttrName("TestInt");
  TInt NId(0);
  int status = Attrs.GetSAttrDat(NId, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Attrs.GetSAttrDat(NId, AttrId, Val);
  EXPECT_EQ(-1, status);
  Attrs.AddSAttr(AttrName, atInt, AttrId);
  TInt TestVal(5);
  Attrs.AddSAttrDat(NId, AttrId, TestVal);
  status = Attrs.GetSAttrDat(NId, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  status = Attrs.GetSAttrDat(NId, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
}

TEST(TSparseAttrSingle, GetSAttrDat_flt) {
  TSparseAttrSingle Attrs;
  TFlt Val;
  TInt AttrId(0);
  TStr AttrName("TestInt");
  TInt NId(0);
  int status = Attrs.GetSAttrDat(NId, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Attrs.GetSAttrDat(NId, AttrId, Val);
  EXPECT_EQ(-1, status);
  Attrs.AddSAttr(AttrName, atFlt, AttrId);
  TFlt TestVal(5.0);
  Attrs.AddSAttrDat(NId, AttrId, TestVal);
  status = Attrs.GetSAttrDat(NId, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  status = Attrs.GetSAttrDat(NId, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
}

TEST(TSparseAttrSingle, GetSAttrDat_str) {
  TSparseAttrSingle Attrs;
  TStr Val;
  TInt AttrId(0);
  TStr AttrName("TestInt");
  TInt NId(0);
  int status = Attrs.GetSAttrDat(NId, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Attrs.GetSAttrDat(NId, AttrId, Val);
  EXPECT_EQ(-1, status);
  Attrs.AddSAttr(AttrName, atStr, AttrId);
  TStr TestVal("5");
  Attrs.AddSAttrDat(NId, AttrId, TestVal);
  status = Attrs.GetSAttrDat(NId, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_STREQ(TestVal.CStr(), Val.CStr());
  status = Attrs.GetSAttrDat(NId, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_STREQ(TestVal.CStr(), Val.CStr());
}

TEST(TSparseAttrSingle, DelSAttrDat) {
  TSparseAttrSingle Attrs;
  TStr IntAttr("TestInt");
  TInt IntId;
  Attrs.AddSAttr(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt FltId;
  Attrs.AddSAttr(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt StrId;
  Attrs.AddSAttr(StrAttr, atStr, StrId);
  TInt Id(0);
  int status = Attrs.DelSAttrDat(Id, IntAttr);
  EXPECT_EQ(-1, status);
  status = Attrs.DelSAttrDat(Id, IntId);
  EXPECT_EQ(-1, status);

  TInt IntVal(5);
  Attrs.AddSAttrDat(Id, IntId, IntVal);
  status = Attrs.DelSAttrDat(Id, IntAttr);
  EXPECT_EQ(0, status);
  Attrs.AddSAttrDat(Id, IntId, IntVal);
  status = Attrs.DelSAttrDat(Id, IntId);
  EXPECT_EQ(0, status);
  status = Attrs.DelSAttrDat(Id, IntId);
  EXPECT_EQ(-1, status);

  TFlt FltVal(5.0);
  Attrs.AddSAttrDat(Id, FltId, FltVal);
  status = Attrs.DelSAttrDat(Id, FltAttr);
  EXPECT_EQ(0, status);
  Attrs.AddSAttrDat(Id, FltId, FltVal);
  status = Attrs.DelSAttrDat(Id, FltId);
  EXPECT_EQ(0, status);
  status = Attrs.DelSAttrDat(Id, FltId);
  EXPECT_EQ(-1, status);

  TStr StrVal("5");
  Attrs.AddSAttrDat(Id, StrId, StrVal);
  status = Attrs.DelSAttrDat(Id, StrAttr);
  EXPECT_EQ(0, status);
  Attrs.AddSAttrDat(Id, StrId, StrVal);
  status = Attrs.DelSAttrDat(Id, StrId);
  EXPECT_EQ(0, status);
  status = Attrs.DelSAttrDat(Id, StrId);
  EXPECT_EQ(-1, status);
}

TEST(TSparseAttrSingle, GetSAttrV) {
  TSparseAttrSingle Attrs;
  TStr IntAttr("TestInt");
  TInt IntId;
  Attrs.AddSAttr(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt FltId;
  Attrs.AddSAttr(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt StrId;
  Attrs.AddSAttr(StrAttr, atStr, StrId);

  TInt Id(0);
  TInt IntVal(5);
  Attrs.AddSAttrDat(Id, IntId, IntVal);
  TFlt FltVal(5.0);
  Attrs.AddSAttrDat(Id, FltId, FltVal);
  TStr StrVal("5");
  Attrs.AddSAttrDat(Id, StrId, StrVal);

  TAttrPrV AttrV;
  Attrs.GetSAttrV(Id, atInt, AttrV);
  EXPECT_EQ(1, AttrV.Len());
  Attrs.GetSAttrV(Id, atFlt, AttrV);
  EXPECT_EQ(1, AttrV.Len());
  Attrs.GetSAttrV(Id, atStr, AttrV);
  EXPECT_EQ(1, AttrV.Len());
  Attrs.GetSAttrV(Id, atAny, AttrV);
  EXPECT_EQ(3, AttrV.Len());
  Attrs.GetSAttrV(Id, atUndef, AttrV);
  EXPECT_EQ(0, AttrV.Len());
}
TEST(TSparseAttrSingle, GetIdVSAttr) {
  TSparseAttrSingle Attrs;
  TStr IntAttr("TestInt");
  TInt IntId;
  Attrs.AddSAttr(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt FltId;
  Attrs.AddSAttr(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt StrId;
  Attrs.AddSAttr(StrAttr, atStr, StrId);

  TInt IntVal(0);
  TFlt FltVal(0);
  TStr StrVal("test");
  for (int i = 0; i < 10; i++) {
    TInt Id(i);
    Attrs.AddSAttrDat(Id, IntId, IntVal);
    if (i%2 == 0) {
      Attrs.AddSAttrDat(Id, FltId, FltVal);
    }
  }
  Attrs.AddSAttrDat(0, StrId, StrVal);

  TIntV IdV;
  Attrs.GetIdVSAttr(IntAttr, IdV);
  EXPECT_EQ(10, IdV.Len());
  Attrs.GetIdVSAttr(IntId, IdV);
  EXPECT_EQ(10, IdV.Len());

  Attrs.GetIdVSAttr(FltAttr, IdV);
  EXPECT_EQ(5, IdV.Len());
  Attrs.GetIdVSAttr(FltId, IdV);
  EXPECT_EQ(5, IdV.Len());

  Attrs.GetIdVSAttr(StrAttr, IdV);
  EXPECT_EQ(1, IdV.Len());
  Attrs.GetIdVSAttr(StrId, IdV);
  EXPECT_EQ(1, IdV.Len());
}

TEST(TSparseAttrPair, AddSAttr) {
  TSparseAttrPair Attrs;
  TInt AttrId;
  int status = Attrs.AddSAttr("TestInt", atInt, AttrId);
  EXPECT_EQ(0, status);
  EXPECt_EQ(0, AttrId.Val);
  status = Attrs.AddSAttr("TestFlt", atFlt, AttrId);
  EXPECT_EQ(0, status);
  EXPECt_EQ(1, AttrId.Val);
  status = Attrs.AddSAttr("TestStr", atStr, AttrId);
  EXPECT_EQ(0, status);
  EXPECt_EQ(2, AttrId.Val);
  status = Attrs.AddSAttr("TestAny", atAny, AttrId);
  EXPECT_EQ(-1, status);
}

TEST(TSparseAttrPair, GetSAttrId) {
  TSparseAttrPair Attrs;
  TInt AttrId;
  Attrs.AddSAttr("TestInt", atInt, AttrId);
  Attrs.AddSAttr("TestFlt", atFlt, AttrId);
  Attrs.AddSAttr("TestStr", atStr, AttrId);
  TAttrType AttrType;
  int status = Attrs.GetSAttrId(TStr("TestInt"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atInt, AttrType);
  EXPECt_EQ(0, AttrId.Val);
  status = Attrs.GetSAttrId(TStr("TestFlt"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atFlt, AttrType);
  EXPECt_EQ(1, AttrId.Val);
  status = Attrs.GetSAttrId(TStr("TestStr"), AttrId, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atStr, AttrType);
  EXPECt_EQ(2, AttrId.Val);
  status = Attrs.GetSAttrId(TStr("TestError"), AttrId, AttrType);
  EXPECT_EQ(-1, status);
}

TEST(TSparseAttrPair, GetSAttrName) {
  TSparseAttrPair Attrs;
  TInt AttrId;
  Attrs.AddSAttr("TestInt", atInt, AttrId);
  Attrs.AddSAttr("TestFlt", atFlt, AttrId);
  Attrs.AddSAttr("TestStr", atStr, AttrId);
  TAttrType AttrType;
  TStr Name;
  int status = Attrs.GetSAttrName(0, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atInt, AttrType);
  EXPECT_STREQ("TestInt", Name.CStr());
  status = Attrs.GetSAttrName(1, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atFlt, AttrType);
  EXPECT_STREQ("TestFlt", Name.CStr());
  status = Attrs.GetSAttrName(2, Name, AttrType);
  EXPECT_EQ(0, status);
  EXPECT_EQ(atStr, AttrType);
  EXPECT_STREQ("TestStr", Name.CStr());
  status = Attrs.GetSAttrName(3, Name, AttrType);
  EXPECT_EQ(-1, status);
}

TEST(TSparseAttrPair, AddSAttrDat_int) {
  TSparseAttrPair Attrs;
  TInt Val(5);
  TIntPr Id(0, 1);
  int status = Attrs.AddSAttrDat(Id, 1, Val);
  EXPECT_EQ(-1, status);
  TInt AttrId;
  TStr AttrName("TestInt");
  Attrs.AddSAttr(AttrName, atInt, AttrId);
  TFlt ErrorVal(1);
  status = Attrs.AddSAttrDat(Id, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Attrs.AddSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Attrs.AddSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestInt2");
  status = Attrs.AddSAttrDat(Id, NewName, Val);
  EXPECT_EQ(0, status);
}

TEST(TSparseAttrPair, AddSAttrDat_flt) {
  TSparseAttrPair Attrs;
  TFlt Val(5.0);
  TIntPr Id(0, 1);
  int status = Attrs.AddSAttrDat(Id, 1, Val);
  EXPECT_EQ(-1, status);
  TInt AttrId;
  TStr AttrName("TestFlt");
  Attrs.AddSAttr(AttrName, atFlt, AttrId);
  TInt ErrorVal(1);
  status = Attrs.AddSAttrDat(Id, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Attrs.AddSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Attrs.AddSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestFlt2");
  status = Attrs.AddSAttrDat(Id, NewName, Val);
  EXPECT_EQ(0, status);
}

TEST(TSparseAttrPair, AddSAttrDat_str) {
  TSparseAttrPair Attrs;
  TStr Val("5");
  TIntPr Id(0, 1);
  int status = Attrs.AddSAttrDat(Id, 1, Val);
  EXPECT_EQ(-1, status);
  TInt AttrId;
  TStr AttrName("TestFlt");
  Attrs.AddSAttr(AttrName, atStr, AttrId);
  TInt ErrorVal(1);
  status = Attrs.AddSAttrDat(Id, AttrId, ErrorVal);
  EXPECT_EQ(-2, status);
  status = Attrs.AddSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  status = Attrs.AddSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  TStr NewName("TestStr2");
  status = Attrs.AddSAttrDat(Id, NewName, Val);
  EXPECT_EQ(0, status);
}

TEST(TSparseAttrPair, GetSAttrDat_int) {
  TSparseAttrPair Attrs;
  TInt Val;
  TInt AttrId(0);
  TStr AttrName("TestInt");
  TIntPr Id(0, 1);
  int status = Attrs.GetSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Attrs.GetSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(-1, status);
  Attrs.AddSAttr(AttrName, atInt, AttrId);
  TInt TestVal(5);
  Attrs.AddSAttrDat(Id, AttrId, TestVal);
  status = Attrs.GetSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  status = Attrs.GetSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
}

TEST(TSparseAttrPair, GetSAttrDat_flt) {
  TSparseAttrPair Attrs;
  TFlt Val;
  TInt AttrId(0);
  TStr AttrName("TestInt");
  TIntPr Id(0, 1);
  int status = Attrs.GetSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Attrs.GetSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(-1, status);
  Attrs.AddSAttr(AttrName, atFlt, AttrId);
  TFlt TestVal(5.0);
  Attrs.AddSAttrDat(Id, AttrId, TestVal);
  status = Attrs.GetSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
  status = Attrs.GetSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_EQ(TestVal.Val, Val.Val);
}

TEST(TSparseAttrPair, GetSAttrDat_str) {
  TSparseAttrPair Attrs;
  TStr Val;
  TInt AttrId(0);
  TStr AttrName("TestInt");
  TIntPr Id(0, 1);
  int status = Attrs.GetSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(-1, status);
  status = Attrs.GetSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(-1, status);
  Attrs.AddSAttr(AttrName, atStr, AttrId);
  TStr TestVal("5");
  Attrs.AddSAttrDat(Id, AttrId, TestVal);
  status = Attrs.GetSAttrDat(Id, AttrId, Val);
  EXPECT_EQ(0, status);
  EXPECT_STREQ(TestVal.CStr(), Val.CStr());
  status = Attrs.GetSAttrDat(Id, AttrName, Val);
  EXPECT_EQ(0, status);
  EXPECT_STREQ(TestVal.CStr(), Val.CStr());
}

TEST(TSparseAttrPair, DelSAttrDat) {
  TSparseAttrPair Attrs;
  TStr IntAttr("TestInt");
  TInt IntId;
  Attrs.AddSAttr(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt FltId;
  Attrs.AddSAttr(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt StrId;
  Attrs.AddSAttr(StrAttr, atStr, StrId);
  TIntPr Id(0, 1);
  int status = Attrs.DelSAttrDat(Id, IntAttr);
  EXPECT_EQ(-1, status);
  status = Attrs.DelSAttrDat(Id, IntId);
  EXPECT_EQ(-1, status);

  TInt IntVal(5);
  Attrs.AddSAttrDat(Id, IntId, IntVal);
  status = Attrs.DelSAttrDat(Id, IntAttr);
  EXPECT_EQ(0, status);
  Attrs.AddSAttrDat(Id, IntId, IntVal);
  status = Attrs.DelSAttrDat(Id, IntId);
  EXPECT_EQ(0, status);
  status = Attrs.DelSAttrDat(Id, IntId);
  EXPECT_EQ(-1, status);

  TFlt FltVal(5.0);
  Attrs.AddSAttrDat(Id, FltId, FltVal);
  status = Attrs.DelSAttrDat(Id, FltAttr);
  EXPECT_EQ(0, status);
  Attrs.AddSAttrDat(Id, FltId, FltVal);
  status = Attrs.DelSAttrDat(Id, FltId);
  EXPECT_EQ(0, status);
  status = Attrs.DelSAttrDat(Id, FltId);
  EXPECT_EQ(-1, status);

  TStr StrVal("5");
  Attrs.AddSAttrDat(Id, StrId, StrVal);
  status = Attrs.DelSAttrDat(Id, StrAttr);
  EXPECT_EQ(0, status);
  Attrs.AddSAttrDat(Id, StrId, StrVal);
  status = Attrs.DelSAttrDat(Id, StrId);
  EXPECT_EQ(0, status);
  status = Attrs.DelSAttrDat(Id, StrId);
  EXPECT_EQ(-1, status);
}

TEST(TSparseAttrPair, GetSAttrV) {
  TSparseAttrPair Attrs;
  TStr IntAttr("TestInt");
  TInt IntId;
  Attrs.AddSAttr(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt FltId;
  Attrs.AddSAttr(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt StrId;
  Attrs.AddSAttr(StrAttr, atStr, StrId);

  TIntPr Id(0, 1);
  TInt IntVal(5);
  Attrs.AddSAttrDat(Id, IntId, IntVal);
  TFlt FltVal(5.0);
  Attrs.AddSAttrDat(Id, FltId, FltVal);
  TStr StrVal("5");
  Attrs.AddSAttrDat(Id, StrId, StrVal);

  TAttrPrV AttrV;
  Attrs.GetSAttrV(Id, atInt, AttrV);
  EXPECT_EQ(1, AttrV.Len());
  Attrs.GetSAttrV(Id, atFlt, AttrV);
  EXPECT_EQ(1, AttrV.Len());
  Attrs.GetSAttrV(Id, atStr, AttrV);
  EXPECT_EQ(1, AttrV.Len());
  Attrs.GetSAttrV(Id, atAny, AttrV);
  EXPECT_EQ(3, AttrV.Len());
  Attrs.GetSAttrV(Id, atUndef, AttrV);
  EXPECT_EQ(0, AttrV.Len());
}
TEST(TSparseAttrPair, GetIdVSAttr) {
  TSparseAttrPair Attrs;
  TStr IntAttr("TestInt");
  TInt IntId;
  Attrs.AddSAttr(IntAttr, atInt, IntId);
  TStr FltAttr("TestFlt");
  TInt FltId;
  Attrs.AddSAttr(FltAttr, atFlt, FltId);
  TStr StrAttr("TestStr");
  TInt StrId;
  Attrs.AddSAttr(StrAttr, atStr, StrId);

  TInt IntVal(0);
  TFlt FltVal(0);
  TStr StrVal("test");
  for (int i = 0; i < 10; i++) {
    TIntPr Id(i, i+1);
    Attrs.AddSAttrDat(Id, IntId, IntVal);
    if (i%2 == 0) {
      Attrs.AddSAttrDat(Id, FltId, FltVal);
    }
  }
  TIntPr Id(0, 1);
  Attrs.AddSAttrDat(Id, StrId, StrVal);

  TIntPrV IdV;
  Attrs.GetIdVSAttr(IntAttr, IdV);
  EXPECT_EQ(10, IdV.Len());
  Attrs.GetIdVSAttr(IntId, IdV);
  EXPECT_EQ(10, IdV.Len());

  Attrs.GetIdVSAttr(FltAttr, IdV);
  EXPECT_EQ(5, IdV.Len());
  Attrs.GetIdVSAttr(FltId, IdV);
  EXPECT_EQ(5, IdV.Len());

  Attrs.GetIdVSAttr(StrAttr, IdV);
  EXPECT_EQ(1, IdV.Len());
  Attrs.GetIdVSAttr(StrId, IdV);
  EXPECT_EQ(1, IdV.Len());
}
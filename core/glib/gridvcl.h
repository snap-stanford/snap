#ifndef gridvcl_h
#define gridvcl_h

/////////////////////////////////////////////////
// Includes
#include "base.h"
#include <vcl.h>
#include <Grids.hpp>

/////////////////////////////////////////////////
// Vcl-String-Grid
typedef enum {
 gvtUndef, gvtBool, gvtInt, gvtFlt, gvtStr, gvtStrV, gvtStrMask, gvtExt}
 TVclStrGridVarType;

ClassTP(TVclStrGrid, PVclStrGrid)//{
private:
  TStrVV GridValVV;
  int FixedCols, FixedRows;
  TStringGrid* StrGrid;
  TCheckBox* GridValCheckBox;
  TComboBox* GridValComboBox;
  int SelColN, SelRowN;
  TStr PrevSelValStr;
  TEdit* ActValEd;
  TCheckBox* ActValCheckBox;
  TComboBox* ActValComboBox;
  TEdit* ActExtValEd;
  TButton* ActExtEdBt;
  TButton* GridExtEdBt;
  TIntIntH ColNToWidthH;
  TIntPrIntH ColRowPrToVarTypeH;
  TIntPrStrH ColRowPrToDfValStrH;
  TIntPrStrVH ColRowPrToValStrVH;
  typedef void (*TExtGetStr)(
   const bool& GetDfValP, const TStr& InStr, TStr& OutStr, bool& OkP);
  typedef TFunc<TExtGetStr> TExtGetStrF;
  THash<TStr, TExtGetStrF> TypeToGetStrFH;
  UndefCopyAssign(TVclStrGrid);
public:
  // gui events
  void __fastcall OnGridKeyDown(
   TObject *Sender, WORD &Key, TShiftState Shift);
  void __fastcall OnGridMouseDown(
   TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall OnGridMouseUp(
   TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
  void __fastcall OnGridSelectCell(
   TObject *Sender, int ACol, int ARow, bool &CanSelect);
  void __fastcall OnGridSetEditText(
   TObject *Sender, int ACol, int ARow, const AnsiString Value);
  void __fastcall OnGridGetEditMask(
   TObject *Sender, int ACol, int ARow, AnsiString &Value);
  void __fastcall OnGridTopLeftChanged(TObject *Sender);
  void __fastcall OnActValChange(TObject *Sender);
  void __fastcall OnActExtEdBtClick(TObject *Sender);
  // vcl-components
  void EnableCtrl(TControl* Control, const bool& VisibleP=true);
  void DisableCtrl(TControl* Control, const bool& VisibleP=false);
  void InitGrid();
  void InitActInput(const PVclStrGrid& PrevStrGrid);
public:
  TVclStrGrid(
   TStringGrid* _StrGrid, TEdit* _ActValEd,
   const int& _FixedCols, const int& _FixedRows,
   const PVclStrGrid& PrevStrGrid=NULL);
  static PVclStrGrid New(
   TStringGrid* StrGrid, TEdit* ActValEd,
   const int& FixedCols, const int& FixedRows,
   const PVclStrGrid& PrevStrGrid=NULL);
  ~TVclStrGrid();

  // extension string edit functions
  void RegExtGetStrF(const TStr& ExtNm, const TExtGetStrF& ExtGetStrF);
  bool IsExtGetStrF(const TStr& ExtNm, TExtGetStrF& ExtGetStrF) const;

  // gui operations
  void SyncToGui();
  void _OnGridSetEditText(const int& ColN, const int& RowN, const TStr& ValStr);
  void FitColWidth(const int& MnWidth=0);
  void FitRowHeight(const int& MnHeight);
  void FitColRowSize(const int& MnColWidth=0, const int& MnRowWidth=0);
  void SetFocus(){StrGrid->SetFocus();}
  int GetFontSize() const {return StrGrid->Font->Size;}
  int SetFontSize(const int& FontSize);
  int IncFontSize(){return SetFontSize(GetFontSize()+1);}
  int DecFontSize(){return SetFontSize(GetFontSize()-1);}
  void SetDfGridLook();

  // cell selection
  bool SelCell(const int ColN, const int RowN, const bool& UpdatePrevSelValStrP=true);
  void UnSelCell();
  int GetSelColN() const {return SelColN;}
  int GetSelRowN() const {return SelRowN;}
  void GetSelColNRowN(int& SelColN, int& SelRowN) const {
    if (IsSelCell(SelColN, SelRowN)){} else {SelColN=-1; SelRowN=-1;}}
  bool IsSelCell(int& _SelColN, int& _SelRowN) const;
  bool IsSelCell() const {
    int SelColN; int SelRowN; return IsSelCell(SelColN, SelRowN);}

  // general operations
  int GetCols() const {return GridValVV.GetXDim();}
  int GetRows() const {return GridValVV.GetYDim();}
  int GetFixedCols() const {return FixedCols;}
  int GetFixedRows() const {return FixedRows;}
  bool Empty() const {return (GetColRecs()==0)||(GetRowRecs()==0);}
  TStr& At(const int& ColN, const int& RowN){
    return GridValVV.At(ColN, RowN);}
  void PutVal(const int& ColN, const int& RowN, const TStr& ValStr){
    At(ColN, RowN)=ValStr; SyncToGui();}
  TStr GetVal(const int& ColN, const int& RowN);
  void DelCol(const int& ColN);
  void DelRow(const int& RowN);
  void GetCellVarType(
   const int& ColN, const int& RowN,
   TVclStrGridVarType& VarType, TStrV& ValStrV) const;
  void GetCellVarType(const int& ColN, const int& RowN,
   TVclStrGridVarType& VarType) const {
    TStrV ValStrV; GetCellVarType(ColN, RowN, VarType, ValStrV);}
  bool IsGridCell(const int& ColN, const int& RowN) const;

  // variables
  int AddColVar(
   const TStr& VarNm, const int& ColWidth=-1,
   const TVclStrGridVarType& VarType=gvtStr,
   const TStr& DfValStr="", const TStrV& ValStrV=TStrV());
  int AddColVar(
   const TStr& VarNm, const int& ColWidth,
   const TVclStrGridVarType& VarType,
   const TStr& DfValStr, const TStr& ValStrVStr, const char& SplitCh=',');
  int AddRowVar(
   const TStr& VarNm,
   const TVclStrGridVarType& VarType=gvtStr,
   const TStr& DfValStr="", const TStrV& ValStrV=TStrV());
  int GetColVars() const {return GetCols()-FixedCols;}
  int GetRowVars() const {return GetRows()-FixedRows;}
  TStr& ColVarNm(const int& VarN){return At(FixedCols+VarN, 0);}
  TStr& RowVarNm(const int& VarN){return At(0, FixedRows+VarN);}
  void PutColVarNm(const int& VarN, const TStr& VarNm){
    ColVarNm(VarN)=VarNm; SyncToGui();}
  void PutRowVarNm(const int& VarN, const TStr& VarNm){
    RowVarNm(VarN)=VarNm; SyncToGui();}
  TStr GetDfColVarVal(const int& ColVarN);

  // records
  int AddColRec();
  int AddRowRec();
  int AddRowRec(const TStr& VarNm, const TStrV& ValStrV);
  int AddRowRec(const TStrV& ValStrV){return AddRowRec("", ValStrV);}
  int GetColRecs() const {return GetCols()-FixedCols;}
  int GetRowRecs() const {return GetRows()-FixedRows;}
  int GetColNFromRecN(const int& RecN) const {return FixedCols+RecN;}
  int GetRowNFromRecN(const int& RecN) const {return FixedRows+RecN;}
  TStr& ColVarVal(const int& ColVarN, const int& RowRecN){
    return At(FixedCols+ColVarN, FixedRows+RowRecN);}
  TStr& RowVarVal(const int& RowVarN, const int& ColRecN){
    return At(FixedCols+ColRecN, FixedRows+RowVarN);}
  void PutColVarVal(const int& ColVarN, const int& RowRecN, const TStr& ValStr){
    ColVarVal(ColVarN, RowRecN)=ValStr; SyncToGui();}
  void PutRowVarVal(const int& RowVarN, const int& ColRecN, const TStr& ValStr){
    RowVarVal(RowVarN, ColRecN)=ValStr; SyncToGui();}
  TStr GetUniqueColVarVal(const int& ColVarN, const TStr& PfxValStr);
  void DelSelRowRec();
  void CopySelRowRec();                
};

#endif

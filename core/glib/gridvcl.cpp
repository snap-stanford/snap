/////////////////////////////////////////////////
// Includes
#include "gridvcl.h"

/////////////////////////////////////////////////
// Vcl-String-Grid
void __fastcall TVclStrGrid::OnGridKeyDown(
 TObject *Sender, WORD &Key, TShiftState Shift){
  if (Key==27){
    int ColN; int RowN;
    if (IsSelCell(ColN, RowN)){
      PutVal(ColN, RowN, PrevSelValStr);
    }
  }
  if (Key==' '){
    int ColN; int RowN;
    if (IsSelCell(ColN, RowN)){
      TVclStrGridVarType VarType; GetCellVarType(ColN, RowN, VarType);
      // set new field value
      if (VarType==gvtBool){ // checkbox
        ActValCheckBox->Checked=!ActValCheckBox->Checked;
      } else
      if (VarType==gvtStrV){ // combobox
        if (ActValComboBox->Items->Count>0){
          ActValComboBox->ItemIndex=
           (ActValComboBox->ItemIndex+1)%ActValComboBox->Items->Count;
          OnActValChange(ActValComboBox);
        }
      }
    }
  }
}

void __fastcall TVclStrGrid::OnGridMouseDown(
 TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y){
  int ColN; int RowN; StrGrid->MouseToCell(X, Y, ColN, RowN);
  SelCell(ColN, RowN);
}

void __fastcall TVclStrGrid::OnGridMouseUp(
 TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y){
  SyncToGui();
}

void __fastcall TVclStrGrid::OnGridSelectCell(
 TObject *Sender, int ACol, int ARow, bool &CanSelect){
  CanSelect=SelCell(ACol, ARow);
  SyncToGui();
}

void __fastcall TVclStrGrid::OnGridSetEditText(
 TObject *Sender, int ACol, int ARow, const AnsiString Value){
  _OnGridSetEditText(ACol, ARow, Value.c_str());
}

void __fastcall TVclStrGrid::OnGridGetEditMask(
 TObject *Sender, int ColN, int RowN, AnsiString& MaskStr){
  TVclStrGridVarType VarType; GetCellVarType(ColN, RowN, VarType);
  if (VarType==gvtStrMask){
    MaskStr="#0.9;1; ";
  } else {
    MaskStr="";
  }
}

void __fastcall TVclStrGrid::OnGridTopLeftChanged(TObject *Sender){
  SyncToGui();
}

void __fastcall TVclStrGrid::OnActValChange(TObject *Sender){
  // get value
  TStr ValStr;
  if (Sender==ActValEd){
    ValStr=ActValEd->Text.c_str();
    int ColN; int RowN;
    if (IsSelCell(ColN, RowN)){
      TVclStrGridVarType VarType; GetCellVarType(ColN, RowN, VarType);
      //if (VarType==gvtFlt){if (!ValStr.IsFlt()){ValStr="0";}}
      //if (VarType==gvtInt){if (!ValStr.IsInt()){ValStr="0";}}
    }
  } else
  if (Sender==ActValCheckBox){
    ValStr=TBool::GetYesNoStr(ActValCheckBox->Checked);
  } else
  if (Sender==GridValCheckBox){
    ValStr=TBool::GetYesNoStr(GridValCheckBox->Checked);
  } else
  if (Sender==ActValComboBox){
    ValStr=ActValComboBox->Text.c_str();
  } else
  if (Sender==GridValComboBox){
    ValStr=GridValComboBox->Text.c_str();
  } else
  if (Sender==ActExtValEd){
    ValStr=ActExtValEd->Text.c_str();
  } else {
    Fail;
  }
  // put value to grid
  int ColN; int RowN;
  if (IsSelCell(ColN, RowN)){
    PutVal(ColN, RowN, ValStr);
  }
}

void __fastcall TVclStrGrid::OnActExtEdBtClick(TObject *Sender){
  TStr ExtNm=ActExtEdBt->Caption.c_str();
  TExtGetStrF ExtGetStrF;
  if (IsExtGetStrF(ExtNm, ExtGetStrF)){
    int ColN; int RowN;
    if (IsSelCell(ColN, RowN)){
      TStr PrevValStr=GetVal(ColN, RowN);
      TStr NewValStr; bool OkP;
      (*ExtGetStrF())(false, PrevValStr, NewValStr, OkP);
      if (OkP){
        ActExtValEd->Text=NewValStr.CStr();
      }
    }
  }
}

void TVclStrGrid::EnableCtrl(TControl* Control, const bool& VisibleP){
  if (Control!=NULL){
    if (!Control->Enabled){Control->Enabled=true;}
    if (Control->Visible!=VisibleP){Control->Visible=VisibleP;}
  }
}

void TVclStrGrid::DisableCtrl(TControl* Control, const bool& VisibleP){
  if (Control!=NULL){
    if (Control->Enabled){Control->Enabled=false;}
    if (Control->Visible!=VisibleP){Control->Visible=VisibleP;}
  }
}

void TVclStrGrid::InitGrid(){
  IAssert(StrGrid!=NULL);
  // set grid properties
  StrGrid->ScrollBars=ssBoth;
  // set grid options
  StrGrid->Options.Clear();
  StrGrid->Options<<goFixedVertLine<<goFixedHorzLine;
  StrGrid->Options<<goVertLine<<goHorzLine;
  StrGrid->Options<<goDrawFocusSelected<<goColSizing;
  StrGrid->Options<<goTabs;
  // set event handlers
  StrGrid->OnKeyDown=&OnGridKeyDown;
  StrGrid->OnMouseDown=&OnGridMouseDown;
  StrGrid->OnMouseUp=&OnGridMouseUp;
  StrGrid->OnSelectCell=&OnGridSelectCell;
  StrGrid->OnSetEditText=&OnGridSetEditText;
  StrGrid->OnGetEditMask=&OnGridGetEditMask;
  StrGrid->OnTopLeftChanged=&OnGridTopLeftChanged;
}

void TVclStrGrid::InitActInput(const PVclStrGrid& PrevStrGrid){
  if (PrevStrGrid.Empty()){
    if (ActValEd!=NULL){
      // prepare editbox
      ActValEd->OnChange=&OnActValChange;
      ActValEd->OnKeyDown=&OnGridKeyDown;
      // prepare active-value checkbox
      ActValCheckBox=new TCheckBox(StrGrid);
      ActValCheckBox->Parent=ActValEd->Parent;
      ActValCheckBox->OnClick=&OnActValChange;
      ActValCheckBox->OnKeyDown=&OnGridKeyDown;
      ActValCheckBox->Visible=false;
      ActValCheckBox->TabOrder=-1;
      ActValCheckBox->Anchors=ActValEd->Anchors;
      ActValCheckBox->Left=ActValEd->Left; ActValCheckBox->Top=ActValEd->Top;
      ActValCheckBox->Width=ActValEd->Width; ActValCheckBox->Height=ActValEd->Height;
      // prepare grid-value checkbox
      GridValCheckBox=new TCheckBox(StrGrid);
      GridValCheckBox->Parent=StrGrid->Parent;
      GridValCheckBox->OnClick=&OnActValChange;
      GridValCheckBox->OnKeyDown=&OnGridKeyDown;
      GridValCheckBox->Visible=false;
      GridValCheckBox->TabOrder=-1;
      GridValCheckBox->Color=clActiveCaption;
      // prepare active-value combobox
      ActValComboBox=new TComboBox(StrGrid);
      ActValComboBox->Parent=ActValEd->Parent;
      ActValComboBox->OnChange=&OnActValChange;
      ActValComboBox->OnKeyDown=&OnGridKeyDown;
      ActValComboBox->Style=Stdctrls::csDropDownList;
      ActValComboBox->Visible=false;
      ActValComboBox->TabOrder=-1;
      ActValComboBox->Anchors=ActValEd->Anchors;
      ActValComboBox->Left=ActValEd->Left; ActValComboBox->Top=ActValEd->Top;
      ActValComboBox->Width=ActValEd->Width; ActValComboBox->Height=ActValEd->Height;
      // prepare active-value combobox
      GridValComboBox=new TComboBox(StrGrid);
      GridValComboBox->Parent=StrGrid->Parent;
      GridValComboBox->OnChange=&OnActValChange;
      GridValComboBox->OnKeyDown=&OnGridKeyDown;
      GridValComboBox->Style=Stdctrls::csDropDownList;
      GridValComboBox->Visible=false;
      GridValComboBox->TabOrder=-1;
    } else {
      ActValCheckBox=NULL;
      GridValCheckBox=NULL;
      ActValComboBox=NULL;
      GridValComboBox=NULL;
    }
    // prepare active-extension-editor button
    ActExtEdBt=new TButton(StrGrid);
    ActExtEdBt->Parent=ActValEd->Parent;
    ActExtEdBt->OnClick=&OnActExtEdBtClick;
    ActExtEdBt->Visible=false;
    ActExtEdBt->TabOrder=-1;
    ActExtEdBt->Anchors=ActValEd->Anchors;
    ActExtEdBt->Anchors>>akLeft;
    ActExtEdBt->Width=80;
    ActExtEdBt->Height=ActValEd->Height;
    ActExtEdBt->Left=ActValEd->Left+ActValEd->Width-ActExtEdBt->Width;
    ActExtEdBt->Top=ActValEd->Top;
    // prepare grid-extension-editor button
    GridExtEdBt=new TButton(StrGrid);
    GridExtEdBt->Parent=StrGrid->Parent;
    GridExtEdBt->OnClick=&OnActExtEdBtClick;
    GridExtEdBt->Visible=false;
    GridExtEdBt->TabOrder=-1;
    //GridExtEdBt->Color=clActiveCaption;
    // prepare active-extension-value editor
    ActExtValEd=new TEdit(StrGrid);
    ActExtValEd->Parent=ActValEd->Parent;
    ActExtValEd->OnChange=&OnActValChange;
    ActExtValEd->OnKeyDown=&OnGridKeyDown;
    ActExtValEd->Visible=false;
    ActExtValEd->TabOrder=-1;
    ActExtValEd->Anchors=ActValEd->Anchors;
    ActExtValEd->Left=ActValEd->Left; ActExtValEd->Top=ActValEd->Top;
    ActExtValEd->Width=ActValEd->Width-ActExtEdBt->Width-10;
    ActExtValEd->Height=ActValEd->Height;
  } else {
    IAssert(PrevStrGrid->ActValEd==ActValEd);
    //ActValCheckBox=PrevStrGrid->ActValCheckBox;
    //GridValCheckBox=PrevStrGrid->GridValCheckBox;
    //ActValComboBox=PrevStrGrid->ActValComboBox;
    //GridValComboBox=PrevStrGrid->GridValComboBox;
    //ActExtEdBt=PrevStrGrid->ActExtEdBt;
    //GridExtEdBt=PrevStrGrid->GridExtEdBt;
    //ActExtValEd=PrevStrGrid->ActExtValEd;
  }
}

TVclStrGrid::TVclStrGrid(
 TStringGrid* _StrGrid, TEdit* _ActValEd,
 const int& _FixedCols, const int& _FixedRows,
 const PVclStrGrid& PrevStrGrid):
  GridValVV(), FixedCols(_FixedCols), FixedRows(_FixedRows),
  StrGrid(_StrGrid), GridValCheckBox(NULL), GridValComboBox(NULL),
  SelColN(-1), SelRowN(-1), PrevSelValStr(),
  ActValEd(_ActValEd), ActValCheckBox(NULL), ActValComboBox(NULL),
  ActExtValEd(NULL), GridExtEdBt(NULL), ActExtEdBt(NULL),
  ColRowPrToVarTypeH(), ColRowPrToDfValStrH(), ColRowPrToValStrVH(){
  // data-grid
  GridValVV.Gen(FixedCols, FixedRows);
  // prepare active-input
  InitActInput(PrevStrGrid);
  // prepare gui-grid
  InitGrid();
  UnSelCell();
  // sync
  SyncToGui();
}

PVclStrGrid TVclStrGrid::New(
 TStringGrid* StrGrid, TEdit* ActValEd,
 const int& FixedCols, const int& FixedRows,
 const PVclStrGrid& PrevStrGrid){
  if (PrevStrGrid.Empty()){
    return new TVclStrGrid(StrGrid, ActValEd, FixedCols, FixedRows, PrevStrGrid);
  } else {
    PVclStrGrid StrGrid=PrevStrGrid;
    StrGrid->FixedCols=FixedCols;
    StrGrid->FixedRows=FixedRows;
    StrGrid->SelColN=-1;
    StrGrid->SelRowN=-1;
    StrGrid->PrevSelValStr="";
    StrGrid->ColRowPrToVarTypeH.Clr();
    StrGrid->ColRowPrToDfValStrH.Clr();
    StrGrid->ColRowPrToValStrVH.Clr();
    // data-grid
    StrGrid->GridValVV.Gen(FixedCols, FixedRows);
    // prepare gui-grid
    StrGrid->InitGrid();
    StrGrid->UnSelCell();
    // sync
    StrGrid->SyncToGui();
    // return string-grid
    return StrGrid;
  }
}

TVclStrGrid::~TVclStrGrid(){
  if (GridValCheckBox!=NULL){GridValCheckBox->Visible=false;}
  if (GridValComboBox!=NULL){GridValComboBox->Visible=false;}
  if (ActValEd!=NULL){ActValEd->Visible=false;}
  if (ActValCheckBox!=NULL){ActValCheckBox->Visible=false;}
  if (ActValComboBox!=NULL){ActValComboBox->Visible=false;}
  if (ActExtValEd!=NULL){ActExtValEd->Visible=false;}
  if (ActExtEdBt!=NULL){ActExtEdBt->Visible=false;}
  if (GridExtEdBt!=NULL){GridExtEdBt->Visible=false;}
}

void TVclStrGrid::RegExtGetStrF(
 const TStr& ExtNm, const TExtGetStrF& ExtGetStrF){
  TypeToGetStrFH.AddDat(ExtNm, ExtGetStrF);
}

bool TVclStrGrid::IsExtGetStrF(const TStr& ExtNm, TExtGetStrF& ExtGetStrF) const {
  return TypeToGetStrFH.IsKeyGetDat(ExtNm, ExtGetStrF);
}

void TVclStrGrid::SyncToGui(){
  // calculate visible number of columns & rows (min 1, FixedCols<Cols)
  int GridCols=TInt::GetMx(FixedCols+1, GetCols());
  int GridRows=TInt::GetMx(FixedRows+1, GetRows());
  // update new fixed and full number of columns & rows
  if (StrGrid->FixedCols!=FixedCols){StrGrid->FixedCols=FixedCols;}
  if (StrGrid->FixedRows!=FixedRows){StrGrid->FixedRows=FixedRows;}
  if (StrGrid->ColCount!=GridCols){StrGrid->ColCount=GridCols;}
  if (StrGrid->RowCount!=GridRows){StrGrid->RowCount=GridRows;}
  // update new cells contents
  for (int ColN=0; ColN<GridCols; ColN++){
    for (int RowN=0; RowN<GridRows; RowN++){
      // if the coordinates are within the data range
      if ((ColN<GetCols())&&(RowN<GetRows())){
        // if the contents is different than existing
        TStr ValStr=GetVal(ColN, RowN);
        if (StrGrid->Cells[ColN][RowN]!=ValStr.CStr()){
          StrGrid->Cells[ColN][RowN]=ValStr.CStr();
        }
      } else {
        // clear the contents of the non-existing cell
        StrGrid->Cells[ColN][RowN]="";
      }
    }
  }
  SelCell(SelColN, SelRowN, false);
}

void TVclStrGrid::_OnGridSetEditText(
 const int& ColN, const int& RowN, const TStr& ValStr){
  if ((0<=ColN)&&(ColN<GetCols())&&(0<=RowN)&&(RowN<GetRows())){
    // update grid-value
    At(ColN, RowN)=ValStr.CStr();
    // update active-value-edit
    SelCell(ColN, RowN, false);
  }
}

void TVclStrGrid::FitColWidth(const int& MnWidth){
  // iterate over columns
  for (int ColN=0; ColN<GetCols(); ColN++){
    // calculate maximal string width
    int MxWidth=0;
    for (int RowN=0; RowN<GetRows(); RowN++){
      TStr ValStr=GetVal(ColN, RowN);
      int Width=StrGrid->Canvas->TextWidth(ValStr.CStr());
      MxWidth=TInt::GetMx(MxWidth, Width);
    }
    // set column width
    MxWidth=TInt::GetMx(5+MxWidth, MnWidth);
    StrGrid->ColWidths[ColN]=MxWidth;
  }
  // sync
  SyncToGui();
}

void TVclStrGrid::FitRowHeight(const int& MnHeight){
  // iterate over rows
  int AllMxHeight=0;
  for (int RowN=0; RowN<GetRows(); RowN++){
    // calculate maximal string height
    int MxHeight=0;
    for (int ColN=0; ColN<GetCols(); ColN++){
      TStr ValStr=GetVal(ColN, RowN);
      int Height=StrGrid->Canvas->TextHeight(ValStr.CStr());
      MxHeight=TInt::GetMx(MxHeight, Height);
    }
    // set row-height
    MxHeight=TInt::GetMx(1+MxHeight, MnHeight);
    AllMxHeight=TInt::GetMx(AllMxHeight, MxHeight);
    //StrGrid->RwoHeights[RowN]=MxHeight;
  }
  StrGrid->DefaultRowHeight=AllMxHeight;
  // sync
  SyncToGui();
}

void TVclStrGrid::FitColRowSize(const int& MnColWidth, const int& MnRowHeight){
  FitColWidth(MnColWidth);
  FitRowHeight(MnRowHeight);
}

int TVclStrGrid::SetFontSize(const int& FontSize){
  if (FontSize>0){
    StrGrid->Font->Size=FontSize;
    GridValCheckBox->Font->Size=FontSize;
    GridValComboBox->Font->Size=FontSize;
    GridExtEdBt->Font->Size=FontSize;
    FitColRowSize();
  }
  return StrGrid->Font->Size;
}

void TVclStrGrid::SetDfGridLook(){
  // set font size
  SetFontSize(8);
  // set column widths
  for (int ColN=0; ColN<StrGrid->ColCount; ColN++){
    TInt ColWidth;
    if (!ColNToWidthH.IsKeyGetDat(ColN, ColWidth)){ColWidth=64;}
    StrGrid->ColWidths[ColN]=ColWidth;
  }
  // set row heights
  for (int RowN=0; RowN<StrGrid->RowCount; RowN++){
    StrGrid->RowHeights[RowN]=22;}
  // sync
  SyncToGui();
}

bool TVclStrGrid::SelCell(
 const int ColN, const int RowN, const bool& UpdatePrevSelValStrP){
  if ((FixedCols<=ColN)&&(ColN<GetCols())&&(FixedRows<=RowN)&&(RowN<GetRows())){
    // update active-value-edit
    if (ActValEd!=NULL){
      // unselect cell
      SelColN=-1; SelRowN=-1;
      // get type info
      TVclStrGridVarType VarType; TStrV ValStrV;
      GetCellVarType(ColN, RowN, VarType, ValStrV);
      // set new field
      if (VarType==gvtBool){ // checkbox
        // enable/disable controls
        DisableCtrl(ActValEd);
        EnableCtrl(ActValCheckBox); EnableCtrl(GridValCheckBox);
        DisableCtrl(ActValComboBox); DisableCtrl(GridValComboBox);
        DisableCtrl(ActExtValEd); DisableCtrl(ActExtEdBt); DisableCtrl(GridExtEdBt);
        // placing grid component
        TRect Rect=StrGrid->CellRect(ColN, RowN);
        GridValCheckBox->Left=3+Rect.Left/*+10*/;
        GridValCheckBox->Top=3+Rect.Top;
        GridValCheckBox->Width=Rect.Width()/*-10*/;
        GridValCheckBox->Height=Rect.Height();
        // assign value
        ActValCheckBox->Checked=(At(ColN, RowN)==TBool::GetYesNoStr(true));
        GridValCheckBox->Checked=ActValCheckBox->Checked;
      } else
      if (VarType==gvtStrV){ // combobox
        // enable/disable controls
        DisableCtrl(ActValEd);
        DisableCtrl(ActValCheckBox); DisableCtrl(GridValCheckBox);
        EnableCtrl(ActValComboBox); EnableCtrl(GridValComboBox);
        DisableCtrl(ActExtValEd); DisableCtrl(ActExtEdBt); DisableCtrl(GridExtEdBt);
        // placing
        TRect Rect=StrGrid->CellRect(ColN, RowN);
        GridValComboBox->Left=3+Rect.Left/*+10*/;
        GridValComboBox->Top=3+Rect.Top;
        GridValComboBox->Width=Rect.Width()/*-10*/;
        GridValComboBox->Height=Rect.Height();
        // prepare choice-values
        ActValComboBox->Items->Clear(); GridValComboBox->Items->Clear();
        for (int ValStrN=0; ValStrN<ValStrV.Len(); ValStrN++){
          ActValComboBox->Items->Add(ValStrV[ValStrN].CStr());
          GridValComboBox->Items->Add(ValStrV[ValStrN].CStr());
        }
        // assign value
        ActValComboBox->ItemIndex=
         ActValComboBox->Items->IndexOf(At(ColN, RowN).CStr());
        if ((ValStrV.Len()>0)&&(ActValComboBox->ItemIndex==-1)){
          ActValComboBox->ItemIndex=0;}
        GridValComboBox->ItemIndex=ActValComboBox->ItemIndex;
      } else
      if (VarType==gvtExt){ // extension
        // get extension name
        TStr ExtNm;
        if (ColRowPrToDfValStrH.IsKeyGetDat(TIntPr(ColN, 1), ExtNm)){}
        else if (ColRowPrToDfValStrH.IsKeyGetDat(TIntPr(1, RowN), ExtNm)){}
        else {ExtNm="";}
        // activate extension
        TExtGetStrF ExtGetStrF;
        if (IsExtGetStrF(ExtNm, ExtGetStrF)){
          // enable/disable controls
          DisableCtrl(ActValEd);
          DisableCtrl(ActValCheckBox); DisableCtrl(GridValCheckBox);
          DisableCtrl(ActValComboBox); DisableCtrl(GridValComboBox);
          EnableCtrl(ActExtValEd); EnableCtrl(ActExtEdBt); EnableCtrl(GridExtEdBt);
          // placing grid button
          TRect Rect=StrGrid->CellRect(ColN, RowN);
          GridExtEdBt->Left=3+Rect.Left;
          GridExtEdBt->Top=3+Rect.Top;
          GridExtEdBt->Width=Rect.Width();
          GridExtEdBt->Height=Rect.Height();
          int TxtWidth=10+StrGrid->Canvas->TextWidth(ExtNm.CStr());
          if (GridExtEdBt->Width>2*TxtWidth){
            GridExtEdBt->Left=Rect.Right-TxtWidth+1;
            GridExtEdBt->Width=TxtWidth;
          }
          // assign value & button captions
          ActExtValEd->Text=At(ColN, RowN).CStr();
          ActExtEdBt->Caption=ExtNm.CStr();
          GridExtEdBt->Caption=ExtNm.CStr();
        }
      } else { // untyped cell
        // enable/disable controls
        EnableCtrl(ActValEd);
        DisableCtrl(ActValCheckBox); DisableCtrl(GridValCheckBox);
        DisableCtrl(ActValComboBox); DisableCtrl(GridValComboBox);
        DisableCtrl(ActExtValEd); DisableCtrl(ActExtEdBt); DisableCtrl(GridExtEdBt);
        // assign value
        ActValEd->Text=At(ColN, RowN).CStr();
      }
    }
    // set new selected-cell
    SelColN=ColN; SelRowN=RowN;
    if (UpdatePrevSelValStrP){PrevSelValStr=GetVal(ColN, RowN);}
    // turn on editing
    if (IsGridCell(ColN, RowN)){
      StrGrid->Options<<goEditing;
    } else {
      StrGrid->Options>>goEditing;
    }
    // select rectangle
    TGridRect SelRect;
    SelRect.Left=ColN; SelRect.Top=RowN;
    SelRect.Right=ColN; SelRect.Bottom=RowN;
    StrGrid->Selection=SelRect;
    return true;
  } else {
    UnSelCell();
    return false;
  }
}

void TVclStrGrid::UnSelCell(){
  // undefine coordinates
  SelColN=-1; SelRowN=-1;
  // disable controls
  DisableCtrl(ActValEd, true);
  DisableCtrl(ActValCheckBox); DisableCtrl(GridValCheckBox);
  DisableCtrl(ActValComboBox); DisableCtrl(GridValComboBox);
  DisableCtrl(ActExtValEd); DisableCtrl(ActExtEdBt); DisableCtrl(GridExtEdBt);
  if (Empty()){ActValEd->Text="No Data";}
  else {ActValEd->Text="";}
  // turn off editing
  StrGrid->Options>>goEditing;
  // select rectangle
  TGridRect SelRect;
  SelRect.Left=-1; SelRect.Top=-1;
  SelRect.Right=-1; SelRect.Bottom=-1;
  StrGrid->Selection=SelRect;
}

bool TVclStrGrid::IsSelCell(int& _SelColN, int& _SelRowN) const {
  if ((FixedCols<=SelColN)&&(SelColN<GetCols())&&
   (FixedRows<=SelRowN)&&(SelRowN<GetRows())){
    _SelColN=SelColN; _SelRowN=SelRowN; return true;
  } else {
    return false;
  }
}

TStr TVclStrGrid::GetVal(const int& ColN, const int& RowN){
  // get value
  TStr ValStr=GridValVV.At(ColN, RowN);
  // if inside the value-grid
  if ((FixedCols<=ColN)&&(FixedRows<=RowN)){
    // get cell type
    TVclStrGridVarType VarType; TStrV ValStrV;
    GetCellVarType(ColN, RowN, VarType, ValStrV);
    // get value according to the type
    if (VarType==gvtBool){ // checkbox
      ValStr=TBool::GetYesNoStr(ValStr==TBool::GetYesNoStr(true));
    } else
    if (VarType==gvtStrV){ // combobox
      if (!ValStrV.IsIn(ValStr)){
        if (ValStrV.Len()>0){ValStr=ValStrV[0];} else {ValStr="";}
      }
    } else
    if (VarType==gvtFlt){ // float
      if (!ValStr.IsFlt()){ValStr="0";}
    } else
    if (VarType==gvtInt){ // integer
      if (!ValStr.IsInt()){ValStr="0";}
    } else
    if (VarType==gvtExt){ // extension
      // get extension name
      TStr ExtNm;
      if (ColRowPrToDfValStrH.IsKeyGetDat(TIntPr(ColN, 1), ExtNm)){}
      else if (ColRowPrToDfValStrH.IsKeyGetDat(TIntPr(1, RowN), ExtNm)){}
      else {ExtNm="";}
      // activate extension
      TExtGetStrF ExtGetStrF;
      if (IsExtGetStrF(ExtNm, ExtGetStrF)){
        TStr NewValStr; bool OkP;
        (*ExtGetStrF())(true, ValStr, NewValStr, OkP);
        if (OkP){ValStr=NewValStr;}
      }
    }
    // reassign correct value
    GridValVV.PutXY(ColN, RowN, ValStr);
  }
  // return value
  return ValStr;
}

void TVclStrGrid::DelCol(const int& ColN){
  GridValVV.DelX(ColN);
  SyncToGui();
}

void TVclStrGrid::DelRow(const int& DelRowN){
  // delete type-name
  ColRowPrToVarTypeH.DelIfKey(TIntPr(1, DelRowN));
  // delete default-values
  ColRowPrToDfValStrH.DelIfKey(TIntPr(1, DelRowN));
  // delete combobox-values
  ColRowPrToValStrVH.DelIfKey(TIntPr(1, DelRowN));
  // move rows below deleted-row one row up
  for (int RowN=DelRowN+1; RowN<GetRows(); RowN++){
    // move type-name
    TInt VarType;
    if (ColRowPrToVarTypeH.IsKeyGetDat(TIntPr(1, RowN), VarType)){
      ColRowPrToVarTypeH.AddDat(TIntPr(1, RowN-1), VarType);
      ColRowPrToVarTypeH.DelKey(TIntPr(1, RowN));
    }
    // move default-value
    TStr DfValStr;
    if (ColRowPrToDfValStrH.IsKeyGetDat(TIntPr(1, RowN), DfValStr)){
      ColRowPrToDfValStrH.AddDat(TIntPr(1, RowN-1), DfValStr);
      ColRowPrToDfValStrH.DelKey(TIntPr(1, RowN));
    }
    // move combobox-values
    TStrV ValStrV;
    if (ColRowPrToValStrVH.IsKeyGetDat(TIntPr(1, RowN), ValStrV)){
      ColRowPrToValStrVH.AddDat(TIntPr(1, RowN-1), ValStrV);
      ColRowPrToValStrVH.DelKey(TIntPr(1, RowN));
    }
  }
  // delete row
  GridValVV.DelY(DelRowN);
  // synchronize gui
  SyncToGui();
}

void TVclStrGrid::GetCellVarType(
 const int& ColN, const int& RowN,
 TVclStrGridVarType& VarType, TStrV& ValStrV) const {
  TInt _VarType;
  if ((FixedRows==1)&&(ColRowPrToVarTypeH.IsKeyGetDat(TIntPr(ColN, 1), _VarType))){
    VarType=TVclStrGridVarType(int(_VarType));
    // column variables
    if (VarType==gvtStrV){
      if (ColRowPrToValStrVH.IsKeyGetDat(TIntPr(ColN, 1), ValStrV)){}
      else {ValStrV.Clr();}
    }
  } else
  if ((FixedCols==1)&&(ColRowPrToVarTypeH.IsKeyGetDat(TIntPr(1, RowN), _VarType))){
    VarType=TVclStrGridVarType(int(_VarType));
    // row variables
    if (VarType==gvtStrV){
      if (ColRowPrToValStrVH.IsKeyGetDat(TIntPr(1, RowN), ValStrV)){}
      else {ValStrV.Clr();}
    }
  } else {
    // no type
    VarType=gvtUndef; ValStrV.Clr();
  }
}

bool TVclStrGrid::IsGridCell(const int& ColN, const int& RowN) const {
  TVclStrGridVarType VarType; GetCellVarType(ColN, RowN, VarType);
  return (VarType!=gvtBool)&&(VarType!=gvtStrV);
}

int TVclStrGrid::AddColVar(
 const TStr& VarNm, const int& ColWidth,
 const TVclStrGridVarType& VarType,
 const TStr& DfValStr, const TStrV& ValStrV){
  IAssert(FixedRows==1);
  // add variable
  GridValVV.AddXDim();
  int ColN=GetCols()-1;
  At(ColN, 0)=VarNm;
  // save column width if defined
  if (ColWidth!=-1){
    ColNToWidthH.AddDat(ColN, ColWidth);
    StrGrid->ColCount=GetCols();
    StrGrid->ColWidths[ColN]=ColWidth;
  }
  // save type
  ColRowPrToVarTypeH.AddDat(TIntPr(ColN, 1), VarType);
  // save default value
  if (!DfValStr.Empty()){
    ColRowPrToDfValStrH.AddDat(TIntPr(ColN, 1), DfValStr);}
  // save combo-box values
  if (!ValStrV.Empty()){
    ColRowPrToValStrVH.AddDat(TIntPr(ColN, 1), ValStrV);}
  // synchronize to gui
  SyncToGui();
  // return vaiable number
  int VarN=GetCols()-1-FixedCols;
  return VarN;
}

int TVclStrGrid::AddColVar(
 const TStr& VarNm, const int& ColWidth,
 const TVclStrGridVarType& VarType,
 const TStr& DfValStr, const TStr& ValStrVStr, const char& SplitCh){
  TStrV ValStrV; ValStrVStr.SplitOnAllCh(SplitCh, ValStrV);
  return AddColVar(VarNm, ColWidth, VarType, DfValStr, ValStrV);
}

int TVclStrGrid::AddRowVar(
 const TStr& VarNm,
 const TVclStrGridVarType& VarType,
 const TStr& DfValStr, const TStrV& ValStrV){
  IAssert(FixedCols==1);
  // add variable
  GridValVV.AddYDim();
  At(0, GetRows()-1)=VarNm;
  int RowN=GetRows()-1;
  // save type
  ColRowPrToVarTypeH.AddDat(TIntPr(1, RowN), VarType);
  // save default value
  if (!DfValStr.Empty()){
    ColRowPrToDfValStrH.AddDat(TIntPr(1, RowN), DfValStr);}
  // save combo-box values
  if (!ValStrV.Empty()){
    ColRowPrToValStrVH.AddDat(TIntPr(1, RowN), ValStrV);}
  // synchronize to gui
  SyncToGui();
  // return vaiable number
  int VarN=GetRows()-1-FixedRows;
  return VarN;
}

TStr TVclStrGrid::GetDfColVarVal(const int& ColVarN){
  int ColN=FixedCols+ColVarN;
  TStr DfValStr;
  if (ColRowPrToDfValStrH.IsKeyGetDat(TIntPr(ColN, 1), DfValStr)){
    // append unique id if '#' at the end
    if ((!DfValStr.Empty())&&(DfValStr.LastCh()=='#')){
      TStr SubDfValStr=DfValStr.GetSubStr(0, DfValStr.Len()-2);
      DfValStr=GetUniqueColVarVal(ColVarN, SubDfValStr);
    }
    // reset default if extension field
    TVclStrGridVarType VarType; GetCellVarType(ColN, -1, VarType);
    if (VarType==gvtExt){DfValStr="";}
  }
  return DfValStr;
}

int TVclStrGrid::AddColRec(){
  GridValVV.AddXDim();
  SyncToGui();
  int ColRecN=GetCols()-1-FixedCols;
  return ColRecN;
}

int TVclStrGrid::AddRowRec(){
  // add row
  GridValVV.AddYDim();
  int RowN=GetRows()-1;
  int RowRecN=GetRows()-1-FixedRows;
  // put default values
  for (int ColVarN=0; ColVarN<GetColVars(); ColVarN++){
    PutColVarVal(ColVarN, RowRecN, GetDfColVarVal(ColVarN));}
  // synchronise gui & set focus to grid
  SyncToGui(); SetFocus();
  // select first cell of the new record
  SelCell(FixedCols, RowN);
  // return current record number
  return RowRecN;
}

int TVclStrGrid::AddRowRec(const TStr& VarNm, const TStrV& ValStrV){
  int RecN=AddRowRec();
  if (!VarNm.Empty()){RowVarNm(RecN)=VarNm;}
  for (int VarN=0; VarN<ValStrV.Len(); VarN++){
    ColVarVal(VarN, RecN)=ValStrV[VarN];}
  SyncToGui();
  return RecN;
}

TStr TVclStrGrid::GetUniqueColVarVal(const int& ColVarN, const TStr& PfxValStr){
  // collect values
  int RowRecs=GetRowRecs();
  TStrH VarValH(RowRecs);
  for (int RowRecN=0; RowRecN<RowRecs; RowRecN++){
    VarValH.AddDat(ColVarVal(ColVarN, RowRecN));}
  // search for unique value
  int ValStrN=1;
  forever{
    TStr ValStr=PfxValStr+TInt::GetStr(ValStrN);
    if (!VarValH.IsKey(ValStr)){return ValStr;}
    ValStrN++;
  }
}

void TVclStrGrid::DelSelRowRec(){
  int DelColN; int DelRowN;
  if (IsSelCell(DelColN, DelRowN)){
    // delete row
    DelRow(DelRowN);
    // select the same cell if possible
    SelCell(DelColN, DelRowN);
    // otherwise select the cell above
    if (!IsSelCell()){
      SelCell(DelColN, DelRowN-1);}
    // synchronize gui
    SyncToGui();
  }
  SetFocus();
}

void TVclStrGrid::CopySelRowRec(){
  int SrcColN; int SrcRowN;
  if (IsSelCell(SrcColN, SrcRowN)){
    // create destination row
    int DstRecN=AddRowRec();
    int DstRowN=GetRowNFromRecN(DstRecN);
    // copy values
    for (int ColN=0; ColN<GetCols(); ColN++){
      At(ColN, DstRowN)=GetVal(ColN, SrcRowN);}
    // select copied row
    SelCell(GetFixedCols(), GetRows()-1);
    // synchronize gui
    SyncToGui();
  }
  SetFocus();
}

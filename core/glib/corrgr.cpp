/////////////////////////////////////////////////
// Includes
#include "corrgr.h"

/////////////////////////////////////////////////
// Correlation-Groups
void TCorrGroups::GetCorrSimVV(TFltVV& SimVV) const {
  int Vars=GetVars();
  SimVV.Gen(Vars, Vars);
  for (int VarN1=0; VarN1<Vars; VarN1++){
    TStr MsgStr=TStr("Var:")+TInt::GetStr(VarN1+1)+"/"+TInt::GetStr(Vars)+"\r";
    TNotify::OnStatus(Notify, MsgStr);
    SimVV.At(VarN1, VarN1)=1;
    for (int VarN2=VarN1+1; VarN2<Vars; VarN2++){
      PCorr Corr=TCorr::New(VarValVV[VarN1], VarValVV[VarN2]);
      SimVV.At(VarN1, VarN2)=fabs(Corr->GetCorrCf());
      SimVV.At(VarN2, VarN1)=fabs(Corr->GetCorrCf());
    }
  }
  TNotify::OnStatus(Notify, "\n");
}

void TCorrGroups::GetEuclSimVV(TFltVV& SimVV) const {
  int Vars=GetVars();
  SimVV.Gen(Vars, Vars);
  for (int VarN1=0; VarN1<Vars; VarN1++){
    TStr MsgStr=TStr("Var:")+TInt::GetStr(VarN1+1)+"/"+TInt::GetStr(Vars)+"\r";
    TNotify::OnStatus(Notify, MsgStr);
    SimVV.At(VarN1, VarN1)=1;
    for (int VarN2=VarN1+1; VarN2<Vars; VarN2++){
      // calculate distance
      double Sim=0;
      for (int ValN=0; ValN<VarValVV[VarN1].Len(); ValN++){
        double ValSim=VarValVV[VarN1][ValN]-VarValVV[VarN2][ValN];
        Sim+=TMath::Sqr(ValSim);
      }
      Sim=-sqrt(Sim);
      // save distance
      SimVV.At(VarN1, VarN2)=Sim;
      SimVV.At(VarN2, VarN1)=Sim;
    }
  }
  TNotify::OnStatus(Notify, "\n");
}

void TCorrGroups::GetSimVVFromData(TFltVV& SimVV) const {
  // create similarity matrix
  IAssert(VarValVV[0].Len()==VarValVV.Len());
  SimVV.Gen(VarValVV.Len(), VarValVV.Len());
  for (int Y=0; Y<VarValVV.Len(); Y++){
    for (int X=0; X<VarValVV.Len(); X++){
      SimVV.At(Y, X)=VarValVV[Y][X];
    }
  }
}

void TCorrGroups::SetGroups(const double& _CorrCfTsh){
  TNotify::OnNotify(Notify, ntInfo,
   "Calculating Correlation Groups for Threshold "+TFlt::GetStr(CorrCfTsh));
  // prepare
  CorrCfTsh=_CorrCfTsh;
  int Vars=GetVars();

  // calculate correlation table
  TFltVV CorrCfVV; GetCorrSimVV(CorrCfVV);

  // create graph (only variables)
  PGraph Graph=TGGraph::New();
  for (int VarN=0; VarN<Vars; VarN++){
    TStr VarNm=TInt::GetStr(VarN);
    PVrtx Vrtx=PVrtx(new TGVrtx(VarNm));
    Graph->AddVrtx(Vrtx);
  }

  // calculate good correlations (add edges to graph)
  int GoodCorrs=0;
  for (int VarN1=0; VarN1<Vars; VarN1++){
    for (int VarN2=VarN1; VarN2<Vars; VarN2++){
      if ((VarN1!=VarN2)&&(fabs(CorrCfVV.At(VarN1, VarN2))>CorrCfTsh)){
        GoodCorrs++;
        TStr MsgStr=TStr("Good Correlations:")+TInt::GetStr(GoodCorrs)+"\r";
        TNotify::OnStatus(Notify, MsgStr);
        //printf("(%d,%d:%0.2f) ", VarN1, VarN2, CorrCfVV.At(VarN1, VarN2)());
        TStr VrtxNm1=TInt::GetStr(VarN1);
        TStr VrtxNm2=TInt::GetStr(VarN2);
        TStr ENm=TFlt::GetStr(CorrCfVV.At(VarN1, VarN2), "%0.2f");
        PVrtx Vrtx1=Graph->GetVrtx(VrtxNm1);
        PVrtx Vrtx2=Graph->GetVrtx(VrtxNm2);
        PEdge Edge=PEdge(new TGEdge(Vrtx1, Vrtx2, ENm, false));
        Graph->AddEdge(Edge);
      }
    }
  }
  TNotify::OnStatus(Notify, "\n");

  // construct variable groups
  VarNVV.Clr();
  TBoolV VarUsedV(Vars);
  {for (int VarN=0; VarN<Vars; VarN++){
    if (!VarUsedV[VarN]){
      VarUsedV[VarN]=true;
      VarNVV.Add();
      TStr VrtxNm=TInt::GetStr(VarN);
      PVrtx Vrtx=Graph->GetVrtx(VrtxNm);
      TVrtxV ConnVrtxV;
      Graph->GetConnVrtxV(Vrtx, ConnVrtxV);
      for (int ConnVrtxN=0; ConnVrtxN<ConnVrtxV.Len(); ConnVrtxN++){
        PVrtx ConnVrtx=ConnVrtxV[ConnVrtxN];
        TStr ConnVrtxVNm=ConnVrtx->GetVNm();
        int ConnVarN=ConnVrtxVNm.GetInt();
        VarNVV.Last().Add(ConnVarN);
        VarUsedV[ConnVrtx->GetVId()]=true;
      }
    }
  }}
  TNotify::OnNotify(Notify, ntInfo, "... Done.");
}

void TCorrGroups::GetGroupCentroid(const TIntV& VarNV, TMomV& VarValMomV) const {
  int Recs=GetRecs();
  TMom::NewV(VarValMomV, Recs);
  for (int VarNN=0; VarNN<VarNV.Len(); VarNN++){
    int VarN=VarNV[VarNN];
    for (int RecN=0; RecN<Recs; RecN++){
      double Val=VarValVV[VarN][RecN];
      VarValMomV[RecN]->Add(Val);
    }
  }
  TMom::DefV(VarValMomV);
}

PGraph TCorrGroups::GetHClustGraph(const TFltVV& SimVV) const {
  // create graph
  PGraph HClustGraph=TGGraph::New();
  // create leaf vertices
  TVrtxV VrtxV(GetVars()); TVec<TIntV> VarNVV(GetVars());
  for (int VarN=0; VarN<GetVars(); VarN++){
    PVrtx Vrtx=PVrtx(new TGVrtx(VarN, GetVarNm(VarN)));
    HClustGraph->AddVrtx(Vrtx);
    VrtxV[VarN]=Vrtx; VarNVV[VarN].Add(VarN);
  }
  // join most similar nodes
  for (int HClustJoinN=0; HClustJoinN<GetVars()-1; HClustJoinN++){
    printf("Join %d\r", HClustJoinN);
    bool FirstMx=true; double MxMeanSim=0; int MxVrtxN1=-1; int MxVrtxN2=-1;
    for (int VrtxN1=0; VrtxN1<VrtxV.Len(); VrtxN1++){
      PVrtx Vrtx1=VrtxV[VrtxN1];
      if (!Vrtx1.Empty()){
        for (int VrtxN2=VrtxN1+1; VrtxN2<VrtxV.Len(); VrtxN2++){
          PVrtx Vrtx2=VrtxV[VrtxN2];
          if (!Vrtx2.Empty()){
            TIntV& VarNV1=VarNVV[VrtxN1];
            TIntV& VarNV2=VarNVV[VrtxN2];
            double SumSim=0;
            for (int VarNN1=0; VarNN1<VarNV1.Len(); VarNN1++){
              for (int VarNN2=0; VarNN2<VarNV2.Len(); VarNN2++){
                SumSim+=SimVV.At(VarNV1[VarNN1], VarNV2[VarNN2]);
              }
            }
            double MeanSim=SumSim/(VarNV1.Len()*VarNV2.Len());
            if ((FirstMx)||(MeanSim>MxMeanSim)){
              FirstMx=false;
              MxMeanSim=MeanSim; MxVrtxN1=VrtxN1; MxVrtxN2=VrtxN2;
            }
          }
        }
      }
    }
    PVrtx JoinVrtx=PVrtx(new TGVrtx()); HClustGraph->AddVrtx(JoinVrtx);
    PEdge JoinEdge1=PEdge(new TGEdge(JoinVrtx, VrtxV[MxVrtxN1]));
    PEdge JoinEdge2=PEdge(new TGEdge(JoinVrtx, VrtxV[MxVrtxN2]));
    HClustGraph->AddEdge(JoinEdge1); HClustGraph->AddEdge(JoinEdge2);
    JoinVrtx->PutWgt(MxMeanSim);
//    JoinEdge1->PutWgt(MxMeanSim); JoinEdge2->PutWgt(MxMeanSim);
    VrtxV[MxVrtxN1]=JoinVrtx; VrtxV[MxVrtxN2]=NULL;
    VarNVV[MxVrtxN1].AddV(VarNVV[MxVrtxN2]); VarNVV[MxVrtxN2].Clr();
  }
  return HClustGraph;
}

PGraph TCorrGroups::GetHClustGraph() const {
  TFltVV SimVV; GetCorrSimVV(SimVV);
  return GetHClustGraph(SimVV);
}

void TCorrGroups::SetHClustGroups(const TFltVV& SimVV, const int& Groups){
  int Vars=GetVars();
  IAssert((0<Groups)&&(Groups<=Vars));
  // create leaf groups
  VarNVV.Gen(Vars);
  for (int VarN=0; VarN<Vars; VarN++){VarNVV[VarN].Add(VarN);}
  // join most similar groups
  for (int HClustJoinN=0; HClustJoinN<Vars-Groups; HClustJoinN++){
    printf("Join %d\r", HClustJoinN);
    bool FirstMx=true; double MxMeanSim=0; int MxVarNVN1=-1; int MxVarNVN2=-1;
    for (int VarNVN1=0; VarNVN1<VarNVV.Len(); VarNVN1++){
      for (int VarNVN2=VarNVN1+1; VarNVN2<VarNVV.Len(); VarNVN2++){
        TIntV& VarNV1=VarNVV[VarNVN1];
        TIntV& VarNV2=VarNVV[VarNVN2];
        double SumSim=0;
        for (int VarNN1=0; VarNN1<VarNV1.Len(); VarNN1++){
          for (int VarNN2=0; VarNN2<VarNV2.Len(); VarNN2++){
            SumSim+=SimVV.At(VarNV1[VarNN1], VarNV2[VarNN2]);
          }
        }
        double MeanSim=SumSim/(VarNV1.Len()*VarNV2.Len());
        if ((FirstMx)||(MeanSim>MxMeanSim)){
          FirstMx=false;
          MxMeanSim=MeanSim; MxVarNVN1=VarNVN1; MxVarNVN2=VarNVN2;
        }
      }
    }
    VarNVV[MxVarNVN1].AddV(VarNVV[MxVarNVN2]);
    VarNVV.Del(MxVarNVN2);
  }
}

void TCorrGroups::SetKMeansGroups(const int& Groups, const int& Trials){
  TRnd Rnd(1); int Vars=GetVars(); int Recs=GetRecs();
  IAssert((0<Groups)&&(Groups<=Vars));
  // create best split state
  double BestQual=0; TVec<TIntV> BestVarNVV;
  // go for trials
  for (int TrialN=0; TrialN<Trials; TrialN++){
    printf("Trial: %d\n", TrialN);
    // create & init centroids
    TVec<TFltV> CentrVarNV(Groups);
    for (int CentrN=0; CentrN<Groups; CentrN++){
      int CentrVarN=Rnd.GetUniDevInt(Vars);
      CentrVarNV[CentrN]=VarValVV[CentrVarN];
    }
    // create split quality
    double PrevQual=0; double Qual=0;
    do {
      PrevQual=Qual;
      // split according to centroids
      VarNVV.Gen(Groups);
      for (int VarN=0; VarN<Vars; VarN++){
        TFltV SimV(Groups);
        for (int CentrN=0; CentrN<Groups; CentrN++){
          PCorr Corr=TCorr::New(CentrVarNV[CentrN], VarValVV[VarN]);
          SimV[CentrN]=fabs(Corr->GetCorrCf());
        }
        int BestGroupN=SimV.GetMxValN();
        VarNVV[BestGroupN].Add(VarN);
      }
      // calculate new centroids
      for (int CentrN=0; CentrN<Groups; CentrN++){
        TMomV VarValMomV; GetGroupCentroid(VarNVV[CentrN], VarValMomV);
        for (int RecN=0; RecN<Recs; RecN++){
          if (VarValMomV[RecN]->IsUsable()){
            CentrVarNV[CentrN][RecN]=VarValMomV[RecN]->GetMedian();
          } else {
            CentrVarNV[CentrN][RecN]=0;
          }
        }
      }
      // calculate split quality
      Qual=0;
      for (int GroupN=0; GroupN<Groups; GroupN++){
        int GroupVars=VarNVV[GroupN].Len();
        for (int VarNN=0; VarNN<GroupVars; VarNN++){
          int VarN=VarNVV[GroupN][VarNN];
          PCorr Corr=TCorr::New(CentrVarNV[GroupN], VarValVV[VarN]);
          double Sim=fabs(Corr->GetCorrCf());
          Qual+=TMath::Sqr(Sim);
        }
      }
      // check for the best split
      if ((PrevQual==0)||(Qual<BestQual)){
        BestQual=Qual; BestVarNVV=VarNVV;
      }
      if (PrevQual!=0){
        printf("   %g (%g)\n", Qual, PrevQual/Qual);}
    } while ((PrevQual==0)||(PrevQual/Qual<0.9999));
  }
  // assign best split to groups
  VarNVV=BestVarNVV;
}

PCorrGroups TCorrGroups::LoadTxtVarCVal(const TStr& FNm){
  // define dataset variables
  TStrV VarNmV; // variable names
  TStrV RecNmV; // record names
  TVec<TFltV> VarValVV; // variable-values
  // open file
  TILx Lx(TFIn::New(FNm), TFSet(iloRetEoln, iloSigNum));
  Lx.GetSym(syInt, syEof); int RecN=0;
  while (Lx.Sym!=syEof){
    // get record name
    RecN++; TStr RecNm=TInt::GetStr(RecN);
    RecNmV.Add(RecNm);
    // prepare for variable parsing
    int PrevVarN=-1; TStrV CurVarNmV; TFltV VarValV(VarNmV.Len(), 0);
    while (Lx.Sym!=syEoln){
      // assert current variable is increment from previous
      IAssert((PrevVarN==-1)||(PrevVarN+1==Lx.Int));
      PrevVarN=Lx.Int;
      // prepare variable name
      TStr VarNm=TInt::GetStr(Lx.Int);
      CurVarNmV.Add(VarNm);
      // parse colon
      Lx.GetSym(syColon);
      // get & save variable value
      double VarVal=Lx.GetFlt();
      VarValV.Add(VarVal);
      // get next variable or end-of-record
      Lx.GetSym(syInt, syEoln);
    }
    // check variable names
    if (VarNmV.Empty()){VarNmV=CurVarNmV;}
    else {IAssert(VarNmV==CurVarNmV);}
    // save variable-values
    VarValVV.Add(VarValV);
    // get new record or end-of-file
    Lx.GetSym(syInt, syEof);
  }
  // create & return corr-groups object
  PCorrGroups CorrGroups=New(VarNmV, RecNmV, VarValVV, TNotify::StdNotify);
  return CorrGroups;
}

void TCorrGroups::SaveTxt(
 const PSOut& SOut, const bool& CentrP, const bool& SqrtValP) const {
  SOut->PutStr("==============================="); SOut->PutLn();
  for (int GroupN=0; GroupN<VarNVV.Len(); GroupN++){
    // output group header
    SOut->PutStr("Group"); SOut->PutInt(GroupN);
    SOut->PutInt(VarNVV[GroupN].Len(), "(%d)");
    SOut->PutStr(":"); SOut->PutLn();
    // output variables in group
    for (int VarNN=0; VarNN<VarNVV[GroupN].Len(); VarNN++){
      int VarN=VarNVV[GroupN][VarNN];
      SOut->PutStr("   ");
      SOut->PutStr(GetVarNm(VarN));
      SOut->PutLn();
    }
    VarNVV[GroupN].Sort();
    // output centroid
    if (CentrP){
      TIntV VarNV; GetGroup(GroupN, VarNV);
      TMomV VarValMomV; GetGroupCentroid(VarNV, VarValMomV);
      SOut->PutStr("Centroid:\n");
      for (int RecN=0; RecN<GetRecs(); RecN++){
        SOut->PutStr("  ");
        SOut->PutStr(GetRecNm(RecN));
        SOut->PutStr(":");
        if (!VarValMomV[RecN]->IsUsable()){
          SOut->PutStr("X");
        } else {
          if (SqrtValP){
            SOut->PutFlt(sqrt(VarValMomV[RecN]->GetMean()), "%0.0f");
            //SOut->PutFlt(sqrt(VarValMomV[RecN]->GetSDev()), "(%0.0f)");
          } else {
            SOut->PutFlt(VarValMomV[RecN]->GetMean());
            //SOut->PutFlt(VarValMomV[RecN]->GetSDev(), "(%g)");
          }
        }
        if ((RecN+1)%8==0){SOut->PutLn();}
      }
      SOut->PutLn();
    }
    // skip line
    SOut->PutLn();
  }
}


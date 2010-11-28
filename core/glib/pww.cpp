//---------------------------------------------------------------------------
#include <vcl\condefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>

#include "base.h"
#include "hc.h"
#include "ts.h"
#include "tbhc.h"
#include "cgi.h"

USEUNIT("\Users\Marko\pww\base.cpp");
USEUNIT("\Users\Marko\pww\hc.cpp");
USEUNIT("\Users\Marko\pww\tbval.cpp");
USEUNIT("\Users\Marko\pww\tbhc.cpp");
USEUNIT("\Users\Marko\pww\tb.cpp");
USEUNIT("\Users\Marko\pww\dm.cpp");
USEUNIT("\Users\Marko\pww\dmhd.cpp");
USEUNIT("\Users\Marko\pww\exset.cpp");
USEUNIT("\Users\Marko\pww\mdtr.cpp");
USEUNIT("\Users\Marko\pww\md.cpp");
USEUNIT("\Users\Marko\pww\valret.cpp");
USEUNIT("\Users\Marko\pww\valds.cpp");
USEUNIT("\Users\Marko\pww\mtr.cpp");
USEUNIT("\Users\Marko\pww\mg.cpp");
USEUNIT("\Users\Marko\pww\ts.cpp");
USEUNIT("\Users\Marko\pww\cgi.cpp");
USEUNIT("\Users\Marko\pww\url.cpp");
USEUNIT("\users\Marko\pww\pest.cpp");
USEUNIT("\users\Marko\pww\aest.cpp");
USEUNIT("\users\marko\pww\html.cpp");
//---------------------------------------------------------------------------
int main(){
//  PTb Tb=TTb::LoadTxt("d:/users/marko/t/t.txt", new TGTb());
//  Tb->SaveAssis("d:/users/marko/t/Vret");

  TStrV FPathV; FPathV.Add("../html"); FPathV.Add("../data"); FPathV.Add("..");
  TStrV FExtV; FExtV.Add("html"); FExtV.Add("zip"); FExtV.Add("cpp");
  TFFile FFile(FPathV, FExtV); TStr FNm;
  while (FFile.Next(FNm)){printf("%s ", FNm.CStr());} printf("\n");

  PPp TsMgPp=TTsMg::GetPp(TTypeNm<TTsMg>(), TTsMg::DNm);
//  TsMgPp->Wr();
  TsMgPp->PutVal(TTypeNm<TTsMgCV>());
  TsMgPp->GetSelPp()->GetPp(TTypeNm<TMg>())->PutVal(TTypeNm<TMgNNbr>());
  TsMgPp->GetSelPp()->GetPp("Folds")->PutVal(10);
//  TsMgPp->Wr();
  PTsMg TsMg=TTsMg::New(TsMgPp);
  TsMg->Wr();

  printf("Tralala!\n"); getchar();
  printf("Tralala!\n");
}

int mmain(){
  int StartTime=time(NULL);
  {PHcBase HcBase=new THcBase();
  TStrV FPathV; FPathV.Add("../html"); //FPathV.Add("html"); FPathV.Add("data");
  TStrV FExtV; FExtV.Add("html"); FExtV.Add("htm"); FExtV.Add("txt");
  TFFile FFile(FPathV, FExtV); int Files=0; TStr FNm;
  while (FFile.Next(FNm)){
    printf("%3d:  %6d  %s\n", Files, HcBase->GetWords(), FNm.CStr());
    HcBase->AddDoc(FNm);
    if (++Files==0){break;}
  }
  printf("%d\n", (int)time(NULL)-StartTime);
//  HcBase->Wr();
  HcBase->GetHLDocV(0);

  PTb Tb=new THBTb(HcBase);
  PDm Dm=new TGDm(Tb, "Tralala", 1);
  PMg Mg=new TMgBayes(Dm, 0);

  {printf("Saving...\n");
  TFOut FOut("t.t");
  HcBase->Save(FOut);}}

  {printf("Loading...\n");
  TFIn FIn("t.t");
  PHcBase NewHcBase(FIn);
  printf("%d\n", (int)time(NULL)-StartTime);
  NewHcBase->DelDocs();
  }

//  getchar();
  return 0;
}


#include "stdafx.h"
//#include "agm.h"
#include "Snap.h"

void PopulateCommunitiesFromFile(const TStr& InFNm, TVec<TIntV>& CmtyVV) {
  printf("Loading communities from %s...\n", InFNm.GetCStr());
  TSsParser Ss(InFNm, ssfWhiteSep);
   
  while (Ss.Next()) {
    if(Ss.GetFlds() > 0) {
      TIntV CmtyV;
      for(int i = 0; i < Ss.GetFlds(); i++) {
        if (Ss.IsInt(i)) { 
          CmtyV.Add(Ss.GetInt(i)); 
        }
      }
      // For ease of comparison, we sort a community by increasing member ID
      CmtyV.Sort();
      CmtyVV.Add(CmtyV);
    }
  }

  // We also sort the collection of communities to enable comparison
  CmtyVV.Sort();
}

// We assume each community and the set (vector) of communities are sorted
bool EqualCommunities(const TVec<TIntV>& CmtyVV1, const TVec<TIntV>& CmtyVV2) {

  // Check if the number of communities are the same  
  if (CmtyVV1.Len() != CmtyVV2.Len()) {
    return false;
  }
  
  // For each pair of communities, we check if they are equal (i.e. contain
  // the same set of member IDs)
  for (TInt i = 0; i < CmtyVV1.Len(); i++) {
    if (!(CmtyVV1[i] == CmtyVV2[i])) {
      return false;
    }
  }

  return true;
}

void PrintCommunityMembership(const TIntV& CmtyV, int NumMembersToPrint = 5) {
  // If there are no members in the community, we have nothing to do
  if (CmtyV.Len() == 0) {
    return;
  }

  printf("(Members' ID: %d", CmtyV[0]());
  for (TInt i = 1; i < TInt::GetMn(CmtyV.Len(), NumMembersToPrint); i++) {
    printf(", %d", CmtyV[i]());
  }
  if (CmtyV.Len() > NumMembersToPrint) {
    printf(", ...");
  }
  printf(")");
}

void PrintInconsistencies(const TVec<TIntV>& CmtyVV1, 
  const TVec<TIntV>& CmtyVV2, int MaxInconsistencies = 10) {

  if (CmtyVV1.Len() != CmtyVV2.Len()) {
    printf("The number of communities do not match.");
    return;
  }

  TInt NumInconsistencies = 0;

  for (TInt i = 0; i < CmtyVV1.Len(); i++) {
    if (!(CmtyVV1[i] == CmtyVV2[i])) {
      printf("The membership of community %d in the first set of "
             "communities ", i());
      PrintCommunityMembership(CmtyVV1[i]);
      printf(" does not match that in the second set of communities ");
      PrintCommunityMembership(CmtyVV2[i]);
      printf(".\n");
      
      NumInconsistencies++;
      if (NumInconsistencies > MaxInconsistencies) {
        printf("...and so on (only the first %d inconsistencies are reported).",
               MaxInconsistencies);
        return;
      }
    }

  } 
}

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(
    TStr::Fmt("cmtycompare - Compare if two sets of communities"
              "from text dump are identical.\n"
              "Executable built at: %s, %s.\n"
              "Start time: %s", 
              __TIME__, __DATE__, TExeTm::GetCurTm()));

  const TStr InFNm1 = 
    Env.GetIfArgPrefixStr("-i1:", "DEMO", "Community affiliation data");
  const TStr InFNm2 = 
    Env.GetIfArgPrefixStr("-i2:", "DEMO", "Community affiliation data");

  TVec<TIntV> CmtyVV1, CmtyVV2;
  PopulateCommunitiesFromFile(InFNm1, CmtyVV1);
  PopulateCommunitiesFromFile(InFNm2, CmtyVV2);

  if (EqualCommunities(CmtyVV1, CmtyVV2)) {
    printf("\nThe two sets of communities are equal.");
  } else {
    printf("\nThe two sets of communities are not equal with the following "
           "inconsistencies:\n");
    PrintInconsistencies(CmtyVV1, CmtyVV2);
  }

  printf("\nEnd time: %s\n", TExeTm::GetCurTm());
  return 0;
}

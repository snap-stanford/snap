#include "stdafx.h"
//#include "agm.h"
#include "Snap.h"
#include <cassert>

TVec<TIntV> CreateCommunitiesFromFile(const TStr& InFNm) {
  printf("Loading communities from %s...\n", InFNm.GetCStr());
  
  TVec<TIntV> CmtyVV;
  TSsParser Ss(InFNm, ssfWhiteSep);
   
  while (Ss.Next()) {
    if(Ss.GetFlds() > 0) {
      TIntV CmtyV;
      for(int i = 0; i < Ss.GetFlds(); i++) {
        if (Ss.IsInt(i)) { 
          CmtyV.Add(Ss.GetInt(i)); 
        }
      }
      CmtyVV.Add(CmtyV);
    }
  }

  return CmtyVV;
}

// Copy constructor runs in O(c*m)
// Sorting every communities in the set runs in O(c*m log m)
// Sorting the community set runs in O(c log c)
// where c is the number of communities,
//       m is the *max* number of members in a community across the set.
// N.B.: The function has a runtime complexity of O(c*m log m) if m^m >= c.
TVec<TIntV> CreateCmtyCopy(const TVec<TIntV>& CmtyVV, 
  bool SortedCommunities = false, bool SortedCommunityList = false) {

  TVec<TIntV> CmtyVVCopy(CmtyVV);

  assert(CmtyVVCopy.Len() == CmtyVV.Len());

  if (SortedCommunities) {
    for (TInt i = 0; i < CmtyVVCopy.Len(); i++) {
      CmtyVVCopy[i].Sort();
    }
  }

  if (SortedCommunityList) {
    CmtyVVCopy.Sort();
  }

  return CmtyVVCopy;
}

bool EqualCmtyFast(const TVec<TIntV>& CmtyVV1, const TVec<TIntV>& CmtyVV2) {

  // Quick check: if the number of communities are the same
  if (CmtyVV1.Len() != CmtyVV2.Len()) {
    return false;
  }

  // More elaborate flag values to enhance code readability
  const bool WITH_SORTED_COMMUNITIES = true;
  const bool WITH_SORTED_COMMUNITY_LIST = true;

  // Create copies of the community sets - it is a bad idea to mess with the
  // original sets of community because we want to show the index of the
  // community (line in the dump file) involved in the mismatch 
  TVec<TIntV> CmtyList1 = CreateCmtyCopy(CmtyVV1, WITH_SORTED_COMMUNITIES,
                                         WITH_SORTED_COMMUNITY_LIST);
  TVec<TIntV> CmtyList2 = CreateCmtyCopy(CmtyVV2, WITH_SORTED_COMMUNITIES,
                                         WITH_SORTED_COMMUNITY_LIST);
  
  // Compare if each (sorted) community in the first sorted set matches with
  // that in the second sorted set
  for (TInt i = 0; i < CmtyList1.Len(); i++) {
    if (!(CmtyList1[i] == CmtyList2[i])) {
      return false;
    }
  }

  return true;
}

void PrintMaxInconsistenciesReached(int MaxInconsistentCmty) {
  printf("...and so on (only the first %d inconsistencies are reported.", 
         MaxInconsistentCmty);
}

bool EqualCmtyDetailed(const TVec<TIntV>& CmtyVV1,
                       const TVec<TIntV>& CmtyVV2,
                       TInt MaxInconsistentCmty = 10) {

  // Quick check: if the number of communities are the same
  if (CmtyVV1.Len() != CmtyVV2.Len()) {
    return false;
  }

  const bool WITH_SORTED_COMMUNITIES = true;
  const bool KEEP_COMMUNITY_LIST_ORDER = false;

  TVec<TIntV> CmtyList1 = CreateCmtyCopy(CmtyVV2, WITH_SORTED_COMMUNITIES,
                                         KEEP_COMMUNITY_LIST_ORDER);
  TVec<TIntV> CmtyList2 = CreateCmtyCopy(CmtyVV2, WITH_SORTED_COMMUNITIES,
                                         KEEP_COMMUNITY_LIST_ORDER);

  TInt NumInconsistentCmty = 0;
  TIntV UnmatchedCmtyIndex;
  for (TInt i = 0; i < CmtyList2.Len(); i++) {
    UnmatchedCmtyIndex.Add(i);
  }

  for (TInt i = 0; i < CmtyList1.Len(); i++) {
    TIntV CurrCmty = CmtyList1[i];
    bool Matched = false;
    TInt ClosestMatchMemberCount = 0;
    TInt ClosestMatchCmtyIndex = 0;

    for (TInt j = 0; j < UnmatchedCmtyIndex.Len(); j++) {
      if (CurrCmty.UnionLen(CmtyList2[j]) == CurrCmty.Len()) {
        UnmatchedCmtyIndex.Del(j);
        break;
      }
    }

    if (Matched) {
      continue;
    }

    if (NumInconsistentCmty >= MaxInconsistentCmty) {
      PrintMaxInconsistenciesReached(MaxInconsistentCmty);
      return false;
    }
  }

  return false;
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

  TVec<TIntV> CmtyVV1 = CreateCommunitiesFromFile(InFNm1);
  TVec<TIntV> CmtyVV2 = CreateCommunitiesFromFile(InFNm2);

  if (EqualCmtyFast(CmtyVV1, CmtyVV2)) {
    printf("Woohoo!");
  }

  //if (EqualCommunities(CmtyVV1, CmtyVV2)) {
   // printf("\nThe two sets of communities are equal.");
  //} else {
  //  printf("\nThe two sets of communities are not equal with the following "
  //         "inconsistencies:\n");
  //  PrintInconsistencies(CmtyVV1, CmtyVV2);
  //}

  //printf("\nEnd time: %s\n", TExeTm::GetCurTm());
  return 0;
}

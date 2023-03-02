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

void PrintCmtyMembership(
  const TIntV& CmtyV, int MaxNumMembersToPrint = 5) {

  // If there are no members in the community, we have nothing to do
  if (CmtyV.Len() == 0) {
    return;
  }

  printf("(Members' ID: %d", CmtyV[0]());
  for (TInt i = 1; i < TInt::GetMn(CmtyV.Len(), MaxNumMembersToPrint); i++) {
    printf(", %d", CmtyV[i]());
  }
  if (CmtyV.Len() > MaxNumMembersToPrint) {
    printf(", ...");
  }
  printf(")");
}

void PrintInconsistentCmty(
  TInt Cmty1Index, TInt ClosestMatchCmty2Index,
  const TVec<TIntV>& CmtyVV1, const TVec<TIntV>& CmtyVV2,
  double ClosestMatchSimilarity) {

  printf("No exact match for community (line) %d ", Cmty1Index() + 1);
  PrintCmtyMembership(CmtyVV1[Cmty1Index]);
  printf(" in the first input community set. "
         "Closest match in the second input community set is ");
  printf("community (line) %d ", ClosestMatchCmty2Index() + 1);
  PrintCmtyMembership(CmtyVV2[ClosestMatchCmty2Index]);
  printf(", with %.2f%% similarity.", ClosestMatchSimilarity);
}

void PrintMaxInconsistentCmtyReachedMsg(TInt MaxInconsistentCmtyPrinted) {
  printf("...and so on (only the first %d inconsistencies are reported).", 
         MaxInconsistentCmtyPrinted());
}

void PrintInconsistentCmtySetDiagnostic(const TVec<TIntV>& CmtyVV1,
  const TVec<TIntV>& CmtyVV2, TInt MaxInconsistentCmtyPrinted = 10) {

  // Quick check: if the number of communities are the same
  if (CmtyVV1.Len() != CmtyVV2.Len()) {
    printf("The number of communities in the input files are not the same.");
    return;
  }

  const bool WITH_SORTED_COMMUNITIES = true;
  const bool KEEP_COMMUNITY_LIST_ORDER = false;

  TVec<TIntV> CmtyList1 = CreateCmtyCopy(CmtyVV1, WITH_SORTED_COMMUNITIES,
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
    double ClosestMatchSimilarity = 0.0;
    TInt ClosestMatchCmtyIndex = 0;

    for (TInt j = 0; j < UnmatchedCmtyIndex.Len(); j++) {
      TInt NumMemberIntersection = 
        CurrCmty.IntrsLen(CmtyList2[UnmatchedCmtyIndex[j]]);

      TInt NumMemberUnion = 
        CurrCmty.UnionLen(CmtyList2[UnmatchedCmtyIndex[j]]);
      
      if (NumMemberIntersection == NumMemberUnion) {
        Matched = true;
        UnmatchedCmtyIndex.Del(j);
        break;
      } else if (100.0 * NumMemberIntersection() / NumMemberUnion() > 
                 ClosestMatchSimilarity) {
        ClosestMatchSimilarity = 
          100.0 * NumMemberIntersection() / NumMemberUnion();
        ClosestMatchCmtyIndex = UnmatchedCmtyIndex[j];
      }
    }

    if (Matched) {
      continue;
    }

    NumInconsistentCmty++;
    PrintInconsistentCmty(i, ClosestMatchCmtyIndex, CmtyVV1, CmtyVV2,
                          ClosestMatchSimilarity);
    
    if (NumInconsistentCmty >= MaxInconsistentCmtyPrinted) {
      PrintMaxInconsistentCmtyReachedMsg(MaxInconsistentCmtyPrinted);
      break;
    }
  }
  
}


int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(
    TStr::Fmt("cmtycompare - Compare if two sets of communities "
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
    printf("\nThe community sets contain in the input files are equal.\n");
  } else {
    printf("\nIt seems the community sets in the input files are not "
           "exactly the same:\n");
    PrintInconsistentCmtySetDiagnostic(CmtyVV1, CmtyVV2);
  }

  printf("\nEnd time: %s\n", TExeTm::GetCurTm());
  return 0;
}

#include "Snap.h"

#define DIR "/lfs/local/0/ringo/testfiles/"

class RingoTable : public TVec<TVec<TInt>*> {

public:
  TVec<TStr> Colnames;

  RingoTable() {}

  RingoTable(TStr fname) {
    printf("%s",fname.CStr());
    TSsParser Ss(fname, ssfTabSep);
    Ss.Next();
    for (int i=0; i<Ss.GetFlds(); i++) {
      Colnames.Add(Ss.GetFld(i));
    }
    for (; Ss.Next(); ) {
      TVec<TInt> *row = new TVec<TInt>;
      for (int i=0; i<Ss.GetFlds(); i++) {
        row->Add(Ss.GetInt(i));
      }
      Add(row);
    }
  }

  void Rename(TStr Col, TStr Name) {
    TStr Dflt("<undefined>");
    for (int i=0; i<Colnames.Len(); i++) {
      TStr TVal = Colnames.GetVal(i);
      if (TVal == Col) {
        Colnames.Del(i);
        Colnames.Ins(i,Name);
      } else if (TVal == Name) {
        Colnames.Del(i);
        Colnames.Ins(i,Dflt);
      }
    }
  }

  TInt GetColIndex(TStr Col) {
    TInt cnt = 0;
    for (TVec<TStr>::TIter i = Colnames.BegI(); i<Colnames.EndI(); i++) {
      if (*i == Col) return cnt;
      cnt++;
    }
    return -1;
  }

  RingoTable SelectEqOne(TStr Col, TInt Val) {
    // Selects rows where the value in the column Col is Val
    TInt Idx = GetColIndex(Col);
    RingoTable NewTable;
    NewTable.Colnames = Colnames;
    for (TIter Row = BegI(); Row<EndI(); Row++) {
      if ((*Row)->GetVal(Idx) == Val) {
        NewTable.Add(*Row);
      }
    }
    Clr();
    return NewTable;
  }

  RingoTable SelectIneqTwo(TStr Col1, TStr Col2) {
    // Selects rows where the values in columns Col1 and Col2 are distinct
    TInt Idx1 = GetColIndex(Col1);
    TInt Idx2 = GetColIndex(Col2);
    RingoTable NewTable;
    NewTable.Colnames = Colnames;
    for (TIter Row = BegI(); Row<EndI(); Row++) {
      if ((*Row)->GetVal(Idx1) != (*Row)->GetVal(Idx2)) {
        NewTable.Add(*Row);
      }
    }
    Clr();
    return NewTable;
    /*for (TIter Row = NewTable.BegI(); Row<NewTable.EndI(); Row++) {
      Add(*Row);
    }*/
  }

  RingoTable Join(TStr Col1, RingoTable Table, TStr Col2) {
    TInt Idx1 = GetColIndex(Col1);
    TInt Idx2 = Table.GetColIndex(Col2);
    RingoTable Result;
    // Create schema
    for (TVec<TStr>::TIter Name = Colnames.BegI(); Name < Colnames.EndI(); Name++) {
      Result.Colnames.Add(*Name);
    }
    for (TVec<TStr>::TIter Name = Table.Colnames.BegI(); Name < Table.Colnames.EndI(); Name++) {
      Result.Colnames.Add(*Name);
    }
    // Hash all rows
    THash<TInt,TVec<TVec<TInt>*> > Hash;
    for (TIter Row = BegI(); Row<EndI(); Row++) {
      TInt Val = (*Row)->GetVal(Idx1);
      TVec<TVec<TInt>*> Dat;
      Hash.IsKeyGetDat(Val,Dat);
      Dat.Add(*Row);
      Hash.AddDat(Val,Dat);
    }
    // Find pairs of rows
    for (TIter Row2 = Table.BegI(); Row2 < Table.EndI(); Row2++) {
      TInt CommonVal = (*Row2)->GetVal(Idx2);
      TVec<TVec<TInt>*> Dat;
      Hash.IsKeyGetDat(CommonVal,Dat);
      for (TVec<TVec<TInt>*>::TIter Row1 = Dat.BegI(); Row1 < Dat.EndI(); Row1++) {
        // Add new row to the result
        TVec<TInt> *NewRow = new TVec<TInt>;
        for (TVec<TInt>::TIter Val = (*Row1)->BegI(); Val < (*Row1)->EndI(); Val++) {
          NewRow->Add(*Val);
        }
        for (TVec<TInt>::TIter Val = (*Row2)->BegI(); Val < (*Row2)->EndI(); Val++) {
          NewRow->Add(*Val);
        }
        Result.Add(NewRow);
      }
    }
    return Result;
  }

  RingoTable GetUniquePairs(TStr Col1, TStr Col2) {
    TInt Idx1 = GetColIndex(Col1);
    TInt Idx2 = GetColIndex(Col2);
    RingoTable Result;
    Result.Colnames.Add(Col1);
    Result.Colnames.Add(Col2);
    THash<TVec<TInt>,TInt> Hash;
    int Cnt = 0;
    for (TVec<TVec<TInt>*>::TIter Row = BegI(); Row < EndI(); Row++) {
      TVec<TInt> Dat;
      Dat.Add((*Row)->GetVal(Idx1));
      Dat.Add((*Row)->GetVal(Idx2));
      Hash.AddDat(Dat,Cnt);
      Cnt++;
    }
//    for (int i = Hash.FFirstKeyId(); Hash.FNextKeyId(i);) {
    for (THashKeyDatI<TVec<TInt>,TInt> iter = Hash.BegI(); iter<Hash.EndI(); iter++) {
      Result.Add(GetVal(iter.GetDat()));
    }
    return Result;
  }

  void PrintColumns() {
    for (int i=0; i < Colnames.Len(); i++) {
      printf("%s ", Colnames.GetVal(i).CStr());
    }
    printf("\n");
  }

  void PrintNFirstRows(int N) {
    for (int i=0; i<N; i++) {
      TVec<TInt> *Row = GetVal(i);
      for (TVec<TInt>::TIter Val = Row->BegI(); Val<Row->EndI(); Val++) {
        printf("%d\t",(*Val)());
      }
      printf("\n");
    }
  }

  void DeallocRows() {
    for (TVec<TVec<TInt>*>::TIter Row = BegI(); Row < EndI(); Row++) {
      delete *Row;
    }
  }
};

TStr getFileName(TStr basename, TInt size) {
  return DIR + basename + "_" + size.GetStr() + ".hashed.tsv";
}

void printResult(double timeSec, const char *operation, RingoTable table) {
  printf("%s:\t%.3f sec (rows: %d)\n", operation, timeSec, table.Len());
}

void buildQAGraph(int size) {
  TStr fname = getFileName("posts", size);
  // 1) Load
  TExeTm t1;
  RingoTable Posts(fname);
  printResult(t1.GetSecs(), "Read", Posts);
  // 2) Copy
  TExeTm t2;
  RingoTable Posts2 = Posts;
  printResult(t2.GetSecs(), "Copy", Posts2);
  Posts.Rename("OwnerUserId","UserId1");
  // 3) Select
  TExeTm t3;
  RingoTable Selected = Posts.SelectEqOne("PostTypeId",2);
  printResult(t3.GetSecs(), "Select", Selected);
  // 4) Join
  TExeTm t4;
  RingoTable Joined = Selected.Join("ParentId",Posts2,"Id");
  printResult(t4.GetSecs(), "Join", Joined);
  // 5) Group
  TExeTm t5;
  RingoTable Final = Joined.GetUniquePairs("UserId1","OwnerUserId");
  printResult(t5.GetSecs(), "Group", Final);
  // Overall time
  printResult(t2.GetSecs(), "Total (without Read)", Final);
  // Deallocate heap objects
  Posts.DeallocRows();
  Joined.DeallocRows();
}

void buildCommonCommentsGraph(int size) {
  TStr fname = getFileName("comments", size);
  // 1) Load
  TExeTm t1;
  RingoTable Comments(fname);
  printResult(t1.GetSecs(), "Read", Comments);
  // 2) Copy
  TExeTm t2;
  RingoTable Comments2 = Comments;
  printResult(t2.GetSecs(), "Copy", Comments2);
  Comments.Rename("UserId","UserId1");
  Comments2.Rename("UserId","UserId2");
  // 3) Join
  TExeTm t3;
  RingoTable Joined = Comments.Join("PostId",Comments2,"PostId");
  printResult(t3.GetSecs(), "Join", Joined);
  // 4) Select
  TExeTm t4;
  RingoTable Selected = Joined.SelectIneqTwo("UserId1","UserId2");
  printResult(t4.GetSecs(), "Select", Selected);
  // 5) Group
  TExeTm t5;
  RingoTable Final = Selected.GetUniquePairs("UserId1","UserId2");
  printResult(t5.GetSecs(), "Group", Final);
  // Overall time
  printResult(t2.GetSecs(), "Total (without Read)", Final);
  // Deallocate heap objects
  Comments.DeallocRows();
  Joined.DeallocRows();
}

int main(int argc, char **argv) {
  int sizes[] = {10,30,100,300,1000,3000,10000};
  // Q&A Graph
  for (unsigned int i=0; i<sizeof(sizes)/sizeof(unsigned int); i++) {
    int size = sizes[i];
    printf("Building Q&A graph with input size %d...\n", size);
    buildQAGraph(size);
  }
  // Common Comments Graph
  for (unsigned int i=0; i<sizeof(sizes)/sizeof(unsigned int); i++) {
    int size = sizes[i];
    printf("Building common comments graph with input size %d...\n", size);
    buildCommonCommentsGraph(size);
  }
}

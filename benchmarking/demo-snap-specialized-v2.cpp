#include "Snap.h"

#define DIR "/lfs/local/0/ringo/testfiles/"
#define ITERATIONS 10
#define QA_FILES 7
#define COMM_FILES 7
#define QA_OPS 5
#define COMM_OPS 5

double qa_results[QA_FILES][ITERATIONS][QA_OPS];
double comm_results[COMM_FILES][ITERATIONS][COMM_OPS];

class RingoTable : public TVec<TVec<TInt>*> {

public:
  TVec<TStr> Colnames;

  RingoTable() {}

  RingoTable(TStr fname) {
    //printf("%s",fname.CStr());
    TSsParser Ss(fname, ssfTabSep);
    Ss.Next();
    for (int i=0; i<Ss.GetFlds(); i++) {
      Colnames.Add(Ss.GetFld(i));
    }
	int rowLen = Ss.GetFlds();
	// give TVec c'tor initial size
    for (; Ss.Next(); ) {
      TVec<TInt> *row = new TVec<TInt>(rowLen);
      for (int i=0; i<rowLen; i++) {
        row->GetVal(i) = Ss.GetInt(i);
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

  void SelectEqOne(TStr Col, TInt Val, RingoTable& Result) {
    // Selects rows where the value in the column Col is Val
    TInt Idx = GetColIndex(Col);
    Result.Colnames = Colnames;
    for (TIter Row = BegI(); Row<EndI(); Row++) {
      if ((*Row)->GetVal(Idx) == Val) {
        Result.Add(*Row);
      }
    }
  }

  void SelectIneqTwo(TStr Col1, TStr Col2, RingoTable& Result) {
    // Selects rows where the values in columns Col1 and Col2 are distinct
    TInt Idx1 = GetColIndex(Col1);
    TInt Idx2 = GetColIndex(Col2);
    Result.Colnames = Colnames;
    for (TIter Row = BegI(); Row<EndI(); Row++) {
      if ((*Row)->GetVal(Idx1) != (*Row)->GetVal(Idx2)) {
        Result.Add(*Row);
      }
    }
  }

  void Join(TStr Col1, RingoTable& Table, TStr Col2, RingoTable& Result) {
    TInt Idx1 = GetColIndex(Col1);
    TInt Idx2 = Table.GetColIndex(Col2);
    TInt L1 = GetVal(0)->Len();  // length of row of table 1
    TInt L2 = Table[0]->Len();  // length of row of table 2
    Result.Colnames = TStrV(L1+L2,0);
    // Create schema
    for (TVec<TStr>::TIter Name = Colnames.BegI(); Name < Colnames.EndI(); Name++) {
      Result.Colnames.Add(*Name);
    }
    for (TVec<TStr>::TIter Name = Table.Colnames.BegI(); Name < Table.Colnames.EndI(); Name++) {
      Result.Colnames.Add(*Name);
    }
    // Hash all rows
    THash<TInt,TIntV> Hash;
    for (TInt i = 0; i < Len(); i++) {
      TInt Val = GetVal(i)->GetVal(Idx1);
      if(Hash.IsKey(Val)){
        Hash.GetDat(Val).Add(i);
      } else{
        TIntV X = TIntV(1);
        X[0] = i;
        Hash.AddDat(Val,X);
      }
    }
    // Find pairs of rows
    for (TInt j = 0; j < Table.Len(); j++) {
      TInt CommonVal = Table[j]->GetVal(Idx2);
      TIntV Dat;
      Hash.IsKeyGetDat(CommonVal,Dat);
      // Dat is a vector of the indices of rows in this table for which GetVal(Idx1) == CommonVal
      for (TIntV::TIter Row1 = Dat.BegI(); Row1 < Dat.EndI(); Row1++) {
        // Add new row to the result
        TIntV* NewRow = new TIntV(L1+L2, 0);
        for (TIntV::TIter Val = GetVal(*Row1)->BegI(); Val < GetVal(*Row1)->EndI(); Val++) {
          NewRow->Add(*Val);
        }
        for (TVec<TInt>::TIter Val = Table[j]->BegI(); Val < Table[j]->EndI(); Val++) {
          NewRow->Add(*Val);
        }
        Result.Add(NewRow);
      }
    }
  }

  void GetUniquePairs(TStr Col1, TStr Col2, RingoTable& Result) {
    TInt Idx1 = GetColIndex(Col1);
    TInt Idx2 = GetColIndex(Col2);
    Result.Colnames = Colnames;
    THash<TVec<TInt>,TInt> Hash;
    int Cnt = 0;
    for (TVec<TVec<TInt>*>::TIter Row = BegI(); Row < EndI(); Row++) {
      TVec<TInt> Dat(2,0);
      Dat.Add((*Row)->GetVal(Idx1));
      Dat.Add((*Row)->GetVal(Idx2));
      Hash.AddDat(Dat,Cnt);
      Cnt++;
    }
    for (THashKeyDatI<TVec<TInt>,TInt> iter = Hash.BegI(); iter<Hash.EndI(); iter++) {
      Result.Add(GetVal(iter.GetDat()));
    }
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

void printResult(double timeSec, const char *operation, RingoTable& table) {
  printf("%s:\t%.3f sec (rows: %d)\n", operation, timeSec, table.Len());
}

void buildQAGraph(int size, int fileIdx) {
  TStr fname = getFileName("posts", size);
  for(int i = 0; i < ITERATIONS; i++){
    // 0) Load
    TExeTm t0;
    RingoTable Posts(fname);
    //printResult(t1.GetSecs(), "Read", Posts);
    qa_results[fileIdx][i][0] = t0.GetSecs();
	// 1) Copy
    TExeTm t1;
    RingoTable Posts2 = Posts;
    qa_results[fileIdx][i][1] = t1.GetSecs();
	Posts.Rename("OwnerUserId","UserId1");
    // 2) Select
    TExeTm t2;
    RingoTable Selected;
    Posts.SelectEqOne("PostTypeId",2, Selected);
    //printResult(t3.GetSecs(), "Select", *Selected);
     qa_results[fileIdx][i][2] = t2.GetSecs();
    // 3) Join
    TExeTm t3;
    RingoTable Joined;
    Selected.Join("ParentId",Posts2,"Id", Joined);
    //printResult(t4.GetSecs(), "Join", *Joined);
    qa_results[fileIdx][i][3] = t3.GetSecs();
    // 4) Group
    TExeTm t4;
    RingoTable Final;
    Joined.GetUniquePairs("UserId1","OwnerUserId", Final);
    //printResult(t5.GetSecs(), "Group", *Final);
    qa_results[fileIdx][i][4] = t4.GetSecs();
    // Overall time
    //printResult(t3.GetSecs(), "Total (without Read)", *Final);
    // Deallocate heap objects
    Posts.DeallocRows();
    Joined.DeallocRows();
  }
}

void buildCommonCommentsGraph(int size, int fileIdx) {
  TStr fname = getFileName("comments", size);
  for(int i = 0; i < ITERATIONS; i++){
    // 0) Load
    TExeTm t0;
    RingoTable Comments(fname);
    //printResult(t0.GetSecs(), "Read", Comments);
    comm_results[fileIdx][i][0] = t0.GetSecs();
    // 1) Copy
    TExeTm t1;
    RingoTable Comments2 = Comments;
    comm_results[fileIdx][i][1] = t1.GetSecs();
    //printResult(t1.GetSecs(), "Copy", Comments2);
    Comments.Rename("UserId","UserId1");
    Comments2.Rename("UserId","UserId2");
    // 2) Join
    TExeTm t2;
    RingoTable Joined;
    Comments.Join("PostId",Comments2,"PostId", Joined);
    comm_results[fileIdx][i][2] = t2.GetSecs();
    //printResult(t3.GetSecs(), "Join", Joined);
    // 3) Select
    TExeTm t3;
    RingoTable Selected;
    Joined.SelectIneqTwo("UserId1","UserId2", Selected);
    comm_results[fileIdx][i][3] = t3.GetSecs();
    //printResult(t4.GetSecs(), "Select", Selected);
    // 4) Group
    TExeTm t4;
    RingoTable Final;
    Selected.GetUniquePairs("UserId1","UserId2", Final);
    comm_results[fileIdx][i][4] = t4.GetSecs();
    //printResult(t5.GetSecs(), "Group", Final);
    // Overall time
    //printResult(t2.GetSecs(), "Total (without Read)", Final);
    // Deallocate heap objects
    Comments.DeallocRows();
    Joined.DeallocRows();
  }
}

int main(int argc, char **argv) {
  int sizes[] = {10,30,100,300,1000,3000,10000};

  for(int i = 0; i < QA_FILES; i++){
    for(int j = 0; j < ITERATIONS; j++){
      for(int k = 0; k < QA_OPS; k++){
        qa_results[i][j][k] = 0;
      }
    }
  }

  for(int i = 0; i < COMM_FILES; i++){
    for(int j = 0; j < ITERATIONS; j++){
      for(int k = 0; k < COMM_OPS; k++){
        comm_results[i][j][k] = 0;
      }
    }
  }

  // Q&A Graph
  for (unsigned int i=0; i<sizeof(sizes)/sizeof(unsigned int); i++) {
    int size = sizes[i];
    printf("\nBuilding Q&A graph with input size %d...\n", size);

    buildQAGraph(size, i);

    printf("Done with QA graph size %d. Stats:\n", size);
    printf("Load: ");
    double load_avg = 0;
    for(int j = 0; j < ITERATIONS; j++){
      printf("%f ", qa_results[i][j][0]);
      load_avg += qa_results[i][j][0];
    }
    load_avg = load_avg/ITERATIONS;
    printf("\naverage load time: %f seconds\n", load_avg);
	
	printf("Copy: ");
    double copy_avg = 0;
    for(int j = 0; j < ITERATIONS; j++){
      printf("%f ", qa_results[i][j][1]);
      copy_avg += qa_results[i][j][1];
    }
    copy_avg = copy_avg/ITERATIONS;
    printf("\naverage copy time: %f seconds\n", copy_avg);

    printf("Select: ");
    double select_avg = 0;
    for(int j = 0; j < ITERATIONS; j++){
      printf("%f ", qa_results[i][j][2]);
      select_avg += qa_results[i][j][2];
    }
    select_avg = select_avg/ITERATIONS;
    printf("\naverage select time: %f seconds\n", select_avg);

    printf("Join: ");
    double join_avg = 0;
    for(int j = 0; j < ITERATIONS; j++){
      printf("%f ", qa_results[i][j][3]);
      join_avg += qa_results[i][j][3];
    }
    join_avg = join_avg/ITERATIONS;
    printf("\naverage join time: %f seconds\n", join_avg);

    printf("Group+Unique: ");
    double group_avg = 0;
    for(int j = 0; j < ITERATIONS; j++){
      printf("%f ", qa_results[i][j][4]);
      group_avg += qa_results[i][j][4];
    }
    group_avg = group_avg/ITERATIONS;
    printf("\naverage group+unique time: %f seconds\n", group_avg);
  }

  // Common Comments Graph
  for (unsigned int i=0; i<sizeof(sizes)/sizeof(unsigned int); i++) {
    int size = sizes[i];
    printf("\nBuilding common comments graph with input size %d...\n", size);

    buildCommonCommentsGraph(size, i);

    printf("Done with Comments graph size %d. Stats:\n", size);
    printf("Load: ");
    double load_avg = 0;
    for(int j = 0; j < ITERATIONS; j++){
      printf("%f ", comm_results[i][j][0]);
      load_avg += comm_results[i][j][0];
    }
    load_avg = load_avg/ITERATIONS;
    printf("\naverage load time: %f seconds\n", load_avg);

    printf("Copy: ");
    double copy_avg = 0;
    for(int j = 0; j < ITERATIONS; j++){
      printf("%f ", comm_results[i][j][1]);
      copy_avg += comm_results[i][j][1];
    }
    copy_avg = copy_avg/ITERATIONS;
    printf("\naverage copy time: %f seconds\n", copy_avg);

    printf("Join: ");
    double join_avg = 0;
    for(int j = 0; j < ITERATIONS; j++){
      printf("%f ", comm_results[i][j][2]);
      join_avg += comm_results[i][j][2];
    }
    join_avg = join_avg/ITERATIONS;
    printf("\naverage join time: %f seconds\n", join_avg);
	
	printf("Select: ");
    double select_avg = 0;
    for(int j = 0; j < ITERATIONS; j++){
      printf("%f ", comm_results[i][j][3]);
      select_avg += comm_results[i][j][3];
    }
    select_avg = select_avg/ITERATIONS;
    printf("\naverage select time: %f seconds\n", select_avg);

    printf("Group+Unique: ");
    double group_avg = 0;
    for(int j = 0; j < ITERATIONS; j++){
      printf("%f ", comm_results[i][j][4]);
      group_avg += comm_results[i][j][4];
    }
    group_avg = group_avg/ITERATIONS;
    printf("\naverage group+unique time: %f seconds\n", group_avg);
  }
}
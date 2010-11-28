/////////////////////////////////////////////////
// BagOfWords-Learning
class TBowLearn{
public:
  // majority model
  static void TrainMajorityMd(const PBowDocBs& BowDocBs, TFltV& MdCatExWgtV);
  static double TestMajorityMd(const PBowDocBs& BowDocBs,
   TFltV& MdCatExWgtV, FILE* OutFile, int& TestExs);

  // inverted-model
  static void TrainInvertedMd(const PBowDocBs& BowDocBs,
   TFltVV& MdWordCatWgtVV, TFltV& MdWordWgtV, TFltV& MdCatWgtV);
  static double TestInvertedMd(const PBowDocBs& BowDocBs,
   TFltVV& MdWordCatWgtVV, TFltV& MdWordWgtV,
   TFltV& MdCatWgtV, FILE* OutFile, int& TestExs);

  // simialrity-model
  static void TrainSimMd(const PBowDocBs& BowDocBs,
   const TVec<TIntV>& DIdVV, TBowSpVV& ConceptSpVV);
  static double TestSimMd(const PBowDocBs& BowDocBs,
   const TBowSpVV& ConceptSpVV, FILE* OutFile, int& TestExs);

  // naive-bayes
  static void TrainNBayesMd(const PBowDocBs& BowDocBs,
   TFltVV& MdWordCatWgtVV, TFltV& MdCatWgtV, TFltV& MdCatExWgtV);
  static double TestNBayesMd(const PBowDocBs& BowDocBs,
   TFltVV& MdWordCatWgtVV, TFltV& MdCatWgtV,
   TFltV& MdCatExWgtV, FILE* OutFile, FILE* ErrFile, int& TestExs);

  // binary-naive-bayes
  static void TrainBinNBayesMd(const PBowDocBs& BowDocBs,
   TFltVV& MdWordCatWgtVV, TFltV& MdCatWgtV,
   TFltV& MdCatExWgtV, const TFileId& OutFile);
  static void TestBinNBayesMd(const PBowDocBs& BowDocBs,
   const TStr& TargetCatNm,
   TFltVV& MdWordCatWgtVV, TFltV& MdCatWgtV, TFltV& MdCatExWgtV,
   const TFileId& TrainOutFile, const TFileId& TestOutFile,
   const TFileId& ErrFile);

  // winnow
  static void TrainWinnowMd(const PBowDocBs& BowDocBs,
   const TStr& TargetCatNm, TFltV& ExpWgtV,
   const TFileId& OutFile, const double& Beta=0.95);
  static void TestWinnowMd(const PBowDocBs& BowDocBs,
   const TStr& TargetCatNm, const TFltV& ExpWgtV,
   const TFileId& TrainOutFile, const TFileId& TestOutFile,
   const TFileId& ErrFile, const double& Beta=0.95);

  // perceptron
  static void TrainPerceptronMd(const PBowDocBs& BowDocBs,
   const TStr& TargetCatNm, TFltV& ExpWgtV,
   const TFileId& OutFile, const double& Beta=0.9, const double& VoteTsh=0.1);
  static void TestPerceptronMd(const PBowDocBs& BowDocBs,
   const TStr& TargetCatNm, const TFltV& ExpWgtV,
   const TFileId& TrainOutFile, const TFileId& TestOutFile,
   const TFileId& ErrFile, const double& Beta=0.9);
};


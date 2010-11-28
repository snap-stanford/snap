//////////////////////////////////////////////////////////////////////////
// Kernel utilities
class TKernelUtil {
public:
    // calculates kernel matrix for Set
    static void CalcKernelMatrix(PSVMTrainSet Set, TFltVV& K);
    // centers kernel matrix ('Kernel methods and P.A.', page 115)
    static void CenterKernelMatrix(TFltVV& K);
};

//////////////////////////////////////////////////////////////////////////
// Partial-Gram-Schmidt (alias Incomplete Cholsky of kernel matrix)
ClassTP(TPartialGS, PPartialGS) //{
public:
    TVec<TFltV> R;  // R from book Kernel Methods, page 126 (if K kernel matrix than K == R'R)
                    // R[i] == i-th row starting with i-th element!
private:
    TIntV IdV;      // IDs (from BigSet) of vectors from which we calculated columns of R
    TFltV NormV;    // norm^2 of q_i before normalized
    TFltV VecNormV; // norm^2 of elements from Set after projection to new basis
public:
    // performes Dim-steps of gram-schmidt on BigSet or until resitual norm is smaller than Eps
    TPartialGS(PSVMTrainSet BigSet, const int& Dim, const double& Eps);
    static PPartialGS New(PSVMTrainSet BigSet, const int& Dim, const double& Eps) {
        return new TPartialGS(BigSet, Dim, Eps); }
    // performes Dim-steps of gram-schmidt on BigSet
    static PPartialGS New(PSVMTrainSet BigSet, const int& Dim) {
        return new TPartialGS(BigSet, Dim, 0.0); }
    // performes  of gram-schmidt on BigSet until residual norm is smaller than Eps
    static PPartialGS New(PSVMTrainSet BigSet, const double& Eps) {
        return new TPartialGS(BigSet, BigSet->Len(), Eps); }

    TPartialGS(TSIn &SIn) {
        R.Load(SIn); IdV.Load(SIn); NormV.Load(SIn); VecNormV.Load(SIn); }
    static PPartialGS Load(TSIn &SIn) { return new TPartialGS(SIn); }
    void Save(TSOut &SOut) {
        R.Save(SOut); IdV.Save(SOut); NormV.Save(SOut); VecNormV.Save(SOut); }

    // returns kernel between i-th and j-th element
    double GetKernel(const int& VecId1, const int& VecId2);
    // number of columns
    int GetCols() const { return IdV.Len(); }
    // number of rows
    int GetRows() const { return R.Len(); }
    // returns row
    const TFltV& GetRow(const int& RowId) { IAssert(RowId < R.Len()); return R[RowId]; }
    // get documents in new basis
    void GetDocVV(TFltVV& DocVV);
    // return basis vector q_n as linear combination of documetns:
    //   q_n = c_1 * x_1 + ... + c_l * x_l, kjer je l == GetRowN()
    void GetBasisV(TVec<TFltV>& q);
    // dot prodoct between ColId-th Col of R and vector w
    double DotProdoctWithCol(const int& ColId, const TFltV& w);

    void GetIdV(TIntV& Out) { Out = IdV; }
    const TIntV& GetIdV() { return IdV; }

    void Dump(const TStr& FName);
};

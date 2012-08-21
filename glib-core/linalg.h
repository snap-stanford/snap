///////////////////////////////////////////////////////////////////////
// forward declarations
class TLinAlg;
class TLAMisc;

///////////////////////////////////////////////////////////////////////
// Matrix
class TMatrix {
private:
    bool Transposed;
protected:
    virtual void PMultiply(const TFltVV& B, int ColId, TFltV& Result) const = 0;
    virtual void PMultiply(const TFltV& Vec, TFltV& Result) const = 0;
    virtual void PMultiplyT(const TFltVV& B, int ColId, TFltV& Result) const = 0;
    virtual void PMultiplyT(const TFltV& Vec, TFltV& Result) const = 0;

    virtual int PGetRows() const = 0;
    virtual int PGetCols() const = 0;
public:
    TMatrix(): Transposed(false) {}
    virtual ~TMatrix() { }

    // Result = A * B(:,ColId)
    void Multiply(const TFltVV& B, int ColId, TFltV& Result) const {
        if (Transposed) { PMultiplyT(B, ColId, Result); }
        else { PMultiply(B, ColId, Result); }
    }
    // Result = A * Vec
    void Multiply(const TFltV& Vec, TFltV& Result) const {
        if (Transposed) { PMultiplyT(Vec, Result); }
        else { PMultiply(Vec, Result); }
    }
    // Result = A' * B(:,ColId)
    void MultiplyT(const TFltVV& B, int ColId, TFltV& Result) const {
        if (Transposed) { PMultiply(B, ColId, Result); }
        else { PMultiplyT(B, ColId, Result); }
    }
    // Result = A' * Vec
    void MultiplyT(const TFltV& Vec, TFltV& Result) const{
        if (Transposed) { PMultiply(Vec, Result); }
        else { PMultiplyT(Vec, Result); }
    }

    // number of rows
    int GetRows() const { return Transposed ? PGetCols() : PGetRows(); }
    // number of columns
    int GetCols() const { return Transposed ? PGetRows() : PGetCols(); }

    void Transpose() { Transposed = !Transposed; }
};

///////////////////////////////////////////////////////////////////////
// Sparse-Column-Matrix
//  matrix is given with columns as sparse vectors
class TSparseColMatrix: public TMatrix {
public:
    // number of rows and columns of matrix
    int RowN, ColN;
    // vector of sparse columns
    TVec<TIntFltKdV> ColSpVV;
protected:
    // Result = A * B(:,ColId)
    virtual void PMultiply(const TFltVV& B, int ColId, TFltV& Result) const;
    // Result = A * Vec
    virtual void PMultiply(const TFltV& Vec, TFltV& Result) const;
    // Result = A' * B(:,ColId)
    virtual void PMultiplyT(const TFltVV& B, int ColId, TFltV& Result) const;
    // Result = A' * Vec
    virtual void PMultiplyT(const TFltV& Vec, TFltV& Result) const;

    int PGetRows() const { return RowN; }
    int PGetCols() const { return ColN; }

public:
    TSparseColMatrix(): TMatrix() {}
    TSparseColMatrix(TVec<TIntFltKdV> _ColSpVV): TMatrix(), ColSpVV(_ColSpVV) {}
    TSparseColMatrix(TVec<TIntFltKdV> _ColSpVV, const int& _RowN, const int& _ColN): 
		TMatrix(), RowN(_RowN), ColN(_ColN), ColSpVV(_ColSpVV) {}
    // loads Matlab sparse matrix format: row, column, value.
    //   Indexes start with 1.
    void Save(TSOut& SOut) {
        SOut.Save(RowN); SOut.Save(ColN); ColSpVV.Save(SOut); }
    void Load(TSIn& SIn) {
        SIn.Load(RowN); SIn.Load(ColN); ColSpVV = TVec<TIntFltKdV>(SIn); }
};

///////////////////////////////////////////////////////////////////////
// Sparse-Row-Matrix
//  matrix is given with rows as sparse vectors
class TSparseRowMatrix: public TMatrix {
public:
    // number of rows and columns of matrix
    int RowN, ColN;
    // vector of sparse rows
    TVec<TIntFltKdV> RowSpVV;
protected:
    // Result = A * B(:,ColId)
    virtual void PMultiply(const TFltVV& B, int ColId, TFltV& Result) const;
    // Result = A * Vec
    virtual void PMultiply(const TFltV& Vec, TFltV& Result) const;
    // Result = A' * B(:,ColId)
    virtual void PMultiplyT(const TFltVV& B, int ColId, TFltV& Result) const;
    // Result = A' * Vec
    virtual void PMultiplyT(const TFltV& Vec, TFltV& Result) const;

    int PGetRows() const { return RowN; }
    int PGetCols() const { return ColN; }

public:
    TSparseRowMatrix(): TMatrix() {}
    TSparseRowMatrix(TVec<TIntFltKdV> _RowSpVV): TMatrix(), RowSpVV(_RowSpVV) {}
    TSparseRowMatrix(TVec<TIntFltKdV> _RowSpVV, const int& _RowN, const int& _ColN): 
		TMatrix(), RowN(_RowN), ColN(_ColN), RowSpVV(_RowSpVV) {}
	// loads Matlab sparse matrix format: row, column, value.
    //   Indexes start with 1.
    TSparseRowMatrix(const TStr& MatlabMatrixFNm);
    void Save(TSOut& SOut) {
        SOut.Save(RowN); SOut.Save(ColN); RowSpVV.Save(SOut); }
    void Load(TSIn& SIn) {
        SIn.Load(RowN); SIn.Load(ColN); RowSpVV = TVec<TIntFltKdV>(SIn); }
};

///////////////////////////////////////////////////////////////////////
// Full-Col-Matrix
//  matrix is given with columns of full vectors
class TFullColMatrix: public TMatrix {
public:
    // number of rows and columns of matrix
    int RowN, ColN;
    // vector of sparse columns
    TVec<TFltV> ColV;
protected:
    // Result = A * B(:,ColId)
    virtual void PMultiply(const TFltVV& B, int ColId, TFltV& Result) const;
    // Result = A * Vec
    virtual void PMultiply(const TFltV& Vec, TFltV& Result) const;
    // Result = A' * B(:,ColId)
    virtual void PMultiplyT(const TFltVV& B, int ColId, TFltV& Result) const;
    // Result = A' * Vec
    virtual void PMultiplyT(const TFltV& Vec, TFltV& Result) const;

    int PGetRows() const { return RowN; }
    int PGetCols() const { return ColN; }

public:
    TFullColMatrix(): TMatrix() {}
    // loads matrix saved in matlab with command:
    //  save -ascii Matrix.dat M
    TFullColMatrix(const TStr& MatlabMatrixFNm);
    void Save(TSOut& SOut) { ColV.Save(SOut); }
    void Load(TSIn& SIn) { ColV.Load(SIn); }
};

//////////////////////////////////////////////////////////////////////
// Basic Linear Algebra Operations
class TLinAlg {
public:
    // <x,y>
    static double DotProduct(const TFltV& x, const TFltV& y);
    // <X(:,ColIdX), Y(:,ColIdY)>
    static double DotProduct(const TFltVV& X, int ColIdX, const TFltVV& Y, int ColIdY);
    // <X(:,ColId), Vec>
    static double DotProduct(const TFltVV& X, int ColId, const TFltV& Vec);
    // sparse dot products:
    // <x,y> where x AND y are sparse
    static double DotProduct(const TIntFltKdV& x, const TIntFltKdV& y);
    // <x,y> where only y is sparse
    static double DotProduct(const TFltV& x, const TIntFltKdV& y);
    // <X(:,ColId),y> where only y is sparse
    static double DotProduct(const TFltVV& X, int ColId, const TIntFltKdV& y);

    // z := p * x + q * y
    static void LinComb(const double& p, const TFltV& x,
        const double& q, const TFltV& y, TFltV& z);
    // z := p * x + (1 - p) * y
    static void ConvexComb(const double& p, const TFltV& x, const TFltV& y, TFltV& z);

    // z := k * x + y
    static void AddVec(const double& k, const TFltV& x, const TFltV& y, TFltV& z);
    // z := k * x + y
    static void AddVec(const double& k, const TIntFltKdV& x, const TFltV& y, TFltV& z);
    // y := k * x + y
    static void AddVec(const double& k, const TIntFltKdV& x, TFltV& y);
    // Y(:,Col) += k * X(:,Col)
    static void AddVec(double k, const TFltVV& X, int ColIdX, TFltVV& Y, int ColIdY);
    // Result += k * X(:,Col)
    static void AddVec(double k, const TFltVV& X, int ColId, TFltV& Result);
	// z = x + y
    static void AddVec(const TIntFltKdV& x, const TIntFltKdV& y, TIntFltKdV& z);

    // Result = SUM(x)
    static double SumVec(const TFltV& x);
    // Result = SUM(k*x + y)
    static double SumVec(double k, const TFltV& x, const TFltV& y);

    // Result = ||x-y||^2 (Euclidian)
    static double EuclDist2(const TFltV& x, const TFltV& y);
    // Result = ||x-y|| (Euclidian)
    static double EuclDist(const TFltV& x, const TFltV& y);

    // ||x||^2 (Euclidian)
    static double Norm2(const TFltV& x);
    // ||x|| (Euclidian)
    static double Norm(const TFltV& x);
    // x := x / ||x||
    static void Normalize(TFltV& x);

    // ||x||^2 (Euclidian), x is sparse
    static double Norm2(const TIntFltKdV& x);
    // ||x|| (Euclidian), x is sparse
    static double Norm(const TIntFltKdV& x);
    // x := x / ||x||, x is sparse
    static void Normalize(TIntFltKdV& x);

    // ||X(:,ColId)||^2 (Euclidian)
    static double Norm2(const TFltVV& X, int ColId);
    // ||X(:,ColId)|| (Euclidian)
    static double Norm(const TFltVV& X, int ColId);

    // L1 norm of x (Sum[|xi|, i = 1..n])
    static double NormL1(const TFltV& x);
    // L1 norm of k*x+y (Sum[|k*xi+yi|, i = 1..n])
    static double NormL1(double k, const TFltV& x, const TFltV& y);
    // L1 norm of x (Sum[|xi|, i = 1..n])
    static double NormL1(const TIntFltKdV& x);
    // x := x / ||x||_inf
    static void NormalizeL1(TFltV& x);
    // x := x / ||x||_inf
    static void NormalizeL1(TIntFltKdV& x);

    // Linf norm of x (Max{|xi|, i = 1..n})
    static double NormLinf(const TFltV& x);
    // Linf norm of x (Max{|xi|, i = 1..n})
    static double NormLinf(const TIntFltKdV& x);
    // x := x / ||x||_inf
    static void NormalizeLinf(TFltV& x);
    // x := x / ||x||_inf, , x is sparse
    static void NormalizeLinf(TIntFltKdV& x);

    // y := k * x
    static void MultiplyScalar(const double& k, const TFltV& x, TFltV& y);
    // y := k * x
    static void MultiplyScalar(const double& k, const TIntFltKdV& x, TIntFltKdV& y);

    // y := A * x
    static void Multiply(const TFltVV& A, const TFltV& x, TFltV& y);
    // C(:, ColId) := A * x
    static void Multiply(const TFltVV& A, const TFltV& x, TFltVV& C, int ColId);
    // y := A * B(:, ColId)
    static void Multiply(const TFltVV& A, const TFltVV& B, int ColId, TFltV& y);
    // C(:, ColIdC) := A * B(:, ColIdB)
    static void Multiply(const TFltVV& A, const TFltVV& B, int ColIdB, TFltVV& C, int ColIdC);

    // y := A' * x
    static void MultiplyT(const TFltVV& A, const TFltV& x, TFltV& y);

    // C = A * B
    static void Multiply(const TFltVV& A, const TFltVV& B, TFltVV& C);
	
	// D = alpha * A(') * B(') + beta * C(')
	typedef enum { GEMM_NO_T = 0, GEMM_A_T = 1, GEMM_B_T = 2, GEMM_C_T = 4 } TLinAlgGemmTranspose;
	static void Gemm(const double& Alpha, const TFltVV& A, const TFltVV& B, const double& Beta, 
		const TFltVV& C, TFltVV& D, const int& TransposeFlags);
	
	// B = A^(-1)
	typedef enum { DECOMP_SVD } TLinAlgInverseType;
	static void Inverse(const TFltVV& A, TFltVV& B, const TLinAlgInverseType& DecompType);
	// subtypes of finding an inverse
	static void InverseSVD(const TFltVV& A, TFltVV& B);

	// transpose matrix - B = A'
	static void Transpose(const TFltVV& A, TFltVV& B);

    // performes Gram-Schmidt ortogonalization on elements of Q
    static void GS(TVec<TFltV>& Q);
    // Gram-Schmidt on columns of matrix Q
    static void GS(TFltVV& Q);

    // rotates vector (OldX,OldY) for angle Angle (in radians!)
    static void Rotate(const double& OldX, const double& OldY, const double& Angle, double& NewX, double& NewY);

    // checks if set of vectors is ortogonal
    static void AssertOrtogonality(const TVec<TFltV>& Vecs, const double& Threshold);
    static void AssertOrtogonality(const TFltVV& Vecs, const double& Threshold);
};

//////////////////////////////////////////////////////////////////////
// Numerical-Recepies-Exception
class TNSException {
public:
    TStr Message;
public:
    TNSException(const TStr& Msg): Message(Msg) {}
};

//////////////////////////////////////////////////////////////////////
// Numerical-Linear-Algebra (copied from Numerical Recepies)
class TNumericalStuff {
private:
  static double sqr(double a);
  static double sign(double a, double b);

  // Computes (a^2 + b^2)^(1/2) without
  // destructive underflow or overflow.
  static double pythag(double a, double b);

  //displays error message to screen
  static void nrerror(const TStr& error_text);

public:
    // Householder reduction of a real, symmetric matrix a[1..n][1..n].
    // On output, a is replaced by the orthogonal matrix Q eecting the
    // transformation. d[1..n] returns the diagonal elements of the
    // tridiagonal matrix, and e[1..n] the o-diagonal elements, with
    // e[1]=0. Several statements, as noted in comments, can be omitted
    // if only eigenvalues are to be found, in which case a contains no
    // useful information on output. Otherwise they are to be included.
    static void SymetricToTridiag(TFltVV& a, int n, TFltV& d, TFltV& e);

	// QL algorithm with implicit shifts, to determine the eigenvalues
	// and eigenvectors of a real, symmetric, tridiagonal matrix, or of
	// a real, symmetric matrix previously reduced by tred2 x11.2. On
	// input, d[1..n] contains the diagonal elements of the tridiagonal
	// matrix. On output, it returns the eigenvalues. The vector e[1..n]
	// inputs the subdiagonal elements of the tridiagonal matrix, with
	// e[1] arbitrary. On output e is destroyed. When finding only the
	// eigenvalues, several lines may be omitted, as noted in the comments.
	// If the eigenvectors of a tridiagonal matrix are desired, the matrix
	// z[1..n][1..n] is input as the identity matrix. If the eigenvectors
	// of a matrix that has been reduced by tred2 are required, then z is
	// input as the matrix output by tred2. In either case, the kth column
	// of z returns the normalized eigenvector corresponding to d[k].
	static void EigSymmetricTridiag(TFltV& d, TFltV& e, int n, TFltVV& z);

	// Given a positive-dedinite symmetric matrix A(n,n), this routine
	// constructs its Cholesky decomposition, A = L * L^T . On input, only
	// the upper triangle of A need be given; it is not modified. The
	// Cholesky factor L is returned in the lower triangle of A, except for
	// its diagonal elements which are returned in p(n).
	static void CholeskyDecomposition(TFltVV& A, TFltV& p);

	// Solves the set of n linear equations A * x = b, where A is a
	// positive-definite symmetric matrix. A(n,n) and p[1..n] are input
	// as the output of the routine choldc. Only the lower triangle of A
	// is accessed. b(n) is input as the right-hand side vector. The
	// solution vector is returned in x(n). A  and p are not modified and
	// can be left in place for successive calls with diferent right-hand
	// sides b. b is not modified unless you identify b and x in the calling
	// sequence, which is allowed.
	static void CholeskySolve(const TFltVV& A, const TFltV& p, const TFltV& b, TFltV& x);

	// Solves system of linear equations A * x = b, where A is symetric
	// positive-definite matrix. A is first decomposed using
	// CholeskyDecomposition and after solved using CholeskySolve. Only
	// upper triangle of A need be given and it is not modified. However,
	// lower triangle is modified!
	static void SolveSymetricSystem(TFltVV& A, const TFltV& b, TFltV& x);

    // solve system A x_i = e_i for i = 1..n, where A and p are output
    // from CholeskyDecomposition. Result is stored to upper triangule
    // (possible since inverse of symetric matrix is also symetric! Sigh...)
    static void InverseSubstitute(TFltVV& A, const TFltV& p);

    // Calculates inverse of symetric positiv definit matrix
    // Matrix is given as upper triangule of A, result is stored
    // in upper triangule of A. Lower triangule is random (actually
    // it has part of Choleksy decompositon of A)
    static void InverseSymetric(TFltVV& A);

    // calcualtes inverse of upper triagonal matrix A
    // lower triangle is messed up...
    static void InverseTriagonal(TFltVV& A);

    // Given a matrix a[1..n][1..n], this routine replaces it by the LU
    // decomposition of a rowwise permutation of itself. a and n are input.
    // a is output, arranged as in equation (2.3.14) above; indx[1..n] is
    // an output vector that records the row permutation efected by the partial
    // pivoting; d is output as +-1 depending on whether the number of row
    // interchanges was even or odd, respectively. This routine is used in
    // combination with lubksb to solve linear equations or invert a matrix.
    static void LUDecomposition(TFltVV& A, TIntV& indx, double& d);

    // Solves the set of n linear equations A*X = B. Here a[1..n][1..n] is input,
    // not as the matrix A but rather as its LU decomposition, determined by the
    // routine ludcmp. indx[1..n] is input as the permutation vector returned by
    // ludcmp. b[1..n] is input as the right-hand side vector B, and returns with
    // the solution vector X. a, n, and indx are not modified by this routine and
    // can be left in place for successive calls with diferent right-hand sides b.
    // This routine takes into account the possibility that b will begin with many
    // zero elements, so it is efficient for use in matrix inversion.
    static void LUSolve(const TFltVV& A, const TIntV& indx, TFltV& b);

    // Solves system of linear equations A * x = b. A is first decomposed using
    // LUDecomposition and after solved using LUSolve. A is modified!
    static void SolveLinearSystem(TFltVV& A, const TFltV& b, TFltV& x);
};

///////////////////////////////////////////////////////////////////////
// Sparse-SVD
//   Calculates singular-value-decompositon for sparse matrixes.
//   If A is a matrix than A is decomposed to A = U S V'
//   where S is diagonal with singular values on diagonal and U
//   and V are ortogonal (U'*U = V'*V = I).
typedef enum { ssotNoOrto, ssotSelective, ssotFull } TSpSVDReOrtoType;
class TSparseSVD {
private:
    // Result = Matrix' * Matrix * Vec(:,ColId)
    static void MultiplyATA(const TMatrix& Matrix,
        const TFltVV& Vec, int ColId, TFltV& Result);
    // Result = Matrix' * Matrix * Vec
    static void MultiplyATA(const TMatrix& Matrix,
        const TFltV& Vec, TFltV& Result);
public:
    // calculates NumEig eigen values of symetric matrix
    // if SvdMatrixProductP than matrix Matrix'*Matrix is used
    static void SimpleLanczos(const TMatrix& Matrix,
        const int& NumEig, TFltV& EigValV,
        const bool& DoLocalReortoP = false,
        const bool& SvdMatrixProductP = false);
    // fast, calculates NumEig largers eigen values and vectors
    // kk should be something like 4*NumEig
    // if SvdMatrixProductP than matrix Matrix'*Matrix is used
    static void Lanczos(const TMatrix& Matrix,
        int NumEig, int Iters, const TSpSVDReOrtoType& ReOrtoType,
        TFltV& EigValV, TFltVV& EigVecVV,
        const bool& SvdMatrixProductP = false);
    static void Lanczos2(const TMatrix& Matrix,
        int MaxNumEig, int MaxSecs, const TSpSVDReOrtoType& ReOrtoType,
        TFltV& EigValV, TFltVV& EigVecVV,
        const bool& SvdMatrixProductP = false);

    // calculates only singular values (based on SimpleLanczos)
    static void SimpleLanczosSVD(const TMatrix& Matrix,
        const int& CalcSV, TFltV& SngValV,
        const bool& DoLocalReortoP = false);
    // fast, calculates NumSV largers SV (based on Lanczos)
    static void LanczosSVD(const TMatrix& Matrix,
        int NumSV, int Iters, const TSpSVDReOrtoType& ReOrtoType,
        TFltV& SgnValV, TFltVV& LeftSgnVecVV, TFltVV& RightSgnVecVV);

    // slow - ortogonal iteration
    static void OrtoIterSVD(const TMatrix& Matrix,
        int NumSV, int IterN, TFltV& SgnValV);

    // projects sparse vector to space spanned by columns of matrix U
    static void Project(const TIntFltKdV& Vec, const TFltVV& U, TFltV& ProjVec);
};

//////////////////////////////////////////////////////////////////////
// Sigmoid  --  made by Janez(TM)
//  (y = 1/[1 + exp[-Ax+B]])
class TSigmoid {
private:
    TFlt A;
    TFlt B;
private:
  // Evaluates how well the sigmoid function fits the data.
  // J(A, B) = - ln prod_i P(Y = y_i | Z = z_i).  The 'data' parameter
  // should contain (z_i, y_i) pairs.  Smaller J means a better fit.
  static double EvaluateFit(const TFltIntKdV& data, const double A, const double B);
  // Computes not only J but also its partial derivatives.
  static void EvaluateFit(const TFltIntKdV& data, const double A,
        const double B, double& J, double& JA, double& JB);
  // Let J(lambda) = J(A + lambda U, B + lambda V).
    // This function computes J and its first and second derivatives.
  // They can be used to choose a good lambda (using Newton's method)
    // when minimizing J. -- This method has not been tested yet.
  static void EvaluateFit(const TFltIntKdV& data, const double A,
        const double B, const double U, const double V, const double lambda,
    double& J, double& JJ, double& JJJ);
public:
    TSigmoid() { };
    TSigmoid(const double& A_, const double& B_): A(A_), B(B_) { };
	// Tries to find a pair (A, B) that minimizes J(A, B).
    // Uses gradient descent.
    TSigmoid(const TFltIntKdV& data);

    TSigmoid(TSIn& SIn) { A.Load(SIn); B.Load(SIn); }
    void Load(TSIn& SIn) { A.Load(SIn); B.Load(SIn); }
    void Save(TSOut& SOut) const {A.Save(SOut); B.Save(SOut);}

    double GetVal(const double& x) const {
        return 1.0 / (1.0 + exp(-A * x + B)); }
    double operator()(const double& x) const {
        return GetVal(x); }

    void GetSigmoidAB(double& A_, double& B_) { A_=A; B_=B; }
};

//////////////////////////////////////////////////////////////////////
// Useful stuff (hopefuly)
class TLAMisc {
public:
	// Dumps vector to file so Excel can read it
    static void SaveCsvTFltV(const TFltV& Vec, TSOut& SOut);
	// Dumps sparse vector to file so Matlab can read it
    static void SaveMatlabTFltIntKdV(const TIntFltKdV& SpV, const int& ColN, TSOut& SOut);
	// Dumps vector to file so Matlab can read it
    static void SaveMatlabTFltV(const TFltV& m, const TStr& FName);
	// Dumps vector to file so Matlab can read it
    static void SaveMatlabTIntV(const TIntV& m, const TStr& FName);
	// Dumps column ColId from m to file so Matlab can read it
    static void SaveMatlabTFltVVCol(const TFltVV& m, int ColId, const TStr& FName);
	// Dumps matrix to file so Matlab can read it
    static void SaveMatlabTFltVV(const TFltVV& m, const TStr& FName);
	// Dumps main minor rowN x colN to file so Matlab can read it
	static void SaveMatlabTFltVVMjrSubMtrx(const TFltVV& m, int rowN, int colN, const TStr& FName);
    // loads matlab full matrix
    static void LoadMatlabTFltVV(const TStr& FNm, TVec<TFltV>& ColV);
    // loads matlab full matrix
    static void LoadMatlabTFltVV(const TStr& FNm, TFltVV& MatrixVV);
    // prints vector to screen
    static void PrintTFltV(const TFltV& Vec, const TStr& VecNm);
	// print matrixt to screen
	static void PrintTFltVV(const TFltVV& A, const TStr& MatrixNm);
    // prints vector to screen
    static void PrintTIntV(const TIntV& Vec, const TStr& VecNm);
    // fills vector with random numbers
    static void FillRnd(TFltV& Vec) { TRnd Rnd(0); FillRnd(Vec, Rnd); }
    static void FillRnd(TFltV& Vec, TRnd& Rnd);
    // set all components
    static void Fill(TFltVV& M, const double& Val);
    // sets all compnents to zero
    static void FillZero(TFltV& Vec) { Vec.PutAll(0.0); }
    static void FillZero(TFltVV& M) { Fill(M, 0.0); }
    // set matrix to identity
    static void FillIdentity(TFltVV& M);
    static void FillIdentity(TFltVV& M, const double& Elt);
    // sums elements in vector
    static int SumVec(const TIntV& Vec);
    static double SumVec(const TFltV& Vec);
    // converts full vector to sparse
    static void ToSpVec(const TFltV& Vec, TIntFltKdV& SpVec,
        const double& CutWordWgtSumPrc = 0.0);
    // converts sparse vector to full
    static void ToVec(const TIntFltKdV& SpVec, TFltV& Vec, const int& VecLen);
};

//////////////////////////////////////////////////////////////////////
// Template-ised Sparse Operations
template <class TKey, class TDat>
class TSparseOps {
private:
	typedef TVec<TKeyDat<TKey, TDat> > TKeyDatV;
public:
	static void SparseMerge(const TKeyDatV& SrcV1, const TKeyDatV& SrcV2, TKeyDatV& DstV) {
		DstV.Clr();
		const int Src1Len = SrcV1.Len();
		const int Src2Len = SrcV2.Len();
		int Src1N = 0, Src2N = 0;
		while (Src1N < Src1Len && Src2N < Src2Len) {
			if (SrcV1[Src1N].Key < SrcV2[Src2N].Key) { 
				DstV.Add(SrcV1[Src1N]); Src1N++;
			} else if (SrcV1[Src1N].Key > SrcV2[Src2N].Key) { 
				DstV.Add(SrcV2[Src2N]); Src2N++;
			} else { 
				DstV.Add(TKeyDat<TKey, TDat>(SrcV1[Src1N].Key, SrcV1[Src1N].Dat + SrcV2[Src2N].Dat));
				Src1N++;  Src2N++; 
			}
		}
		while (Src1N < Src1Len) { DstV.Add(SrcV1[Src1N]); Src1N++; }
		while (Src2N < Src2Len) { DstV.Add(SrcV2[Src2N]); Src2N++; }
	}
};

typedef TSparseOps<TInt, TFlt> TSparseOpsIntFlt;

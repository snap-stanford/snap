namespace TSnap {
/// Fills the numpyvec array with TIntV vector values. 
/// Note that only the first n values are filled. 
void TIntVToNumpy(TIntV& IntV, int* IntNumpyVec, int n) {
		int limit = min(IntV.Len(), n);

    for (int i=0; i < limit; i++) {
        IntNumpyVec[i] = IntV[i];
		}
}


/// Fills the numpyvec array with TFltV vector values. 
/// Note that only the first n values are filled. 
void TFltVToNumpy(TFltV& FltV, float* FltNumpyVec, int n) {
		int limit = min(FltV.Len(), n);

    for (int i=0; i < limit; i++) {
        FltNumpyVec[i] = FltV[i];
		}
}
}

namespace TSnap {
/// Fills the numpyvec array with TIntV vector values. 
/// Note that only the first n values are filled. 
void TIntVToNumpy(TIntV& IntV, int* IntNumpyVecOut, int n) {
		int limit = MIN(IntV.Len(), n);

    for (int i=0; i < limit; i++) {
        IntNumpyVecOut[i] = IntV[i];
		}
}


/// Fills the numpyvec array with TFltV vector values. 
/// Note that only the first n values are filled. 
void TFltVToNumpy(TFltV& FltV, float* FltNumpyVecOut, int n) {
		int limit = MIN(FltV.Len(), n);

    for (int i=0; i < limit; i++) {
        FltNumpyVecOut[i] = static_cast<float>(FltV[i]);
		}
}

/// Fills the IntV with Numpy Array values.
/// Assumes that IntV is of size n. 
void NumpyToTIntV(TIntV& IntV, int* IntNumpyVecIn, int n) {
		for (int i = 0; i < n; ++i) {
				IntV[i] = IntNumpyVecIn[i];
		}
}

/// Fills the FltV with Numpy Array values.
/// Assumes that the vector is of size n. 
void NumpyToTFltV(TFltV& FltV, float* FltNumpyVecIn, int n) {
		for (int i = 0; i < n; ++i) {
				FltV[i] = FltNumpyVecIn[i];
		}
}
}

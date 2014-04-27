#ifndef NUMPY_H
#define NUMPY_H

namespace TSnap {
	/// Converts TIntV to Numpy array.
	void TIntVToNumpy(TIntV& IntV, int* IntNumpyVec, int n);
	/// Converts TFltV to Numpy array.
	void TFltVToNumpy(TFltV& FltV, float* FltNumpyVec, int n);
}

#endif

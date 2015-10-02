#ifndef NUMPY_H
#define NUMPY_H

namespace TSnap {
	/// Converts TIntV to Numpy array.
	void TIntVToNumpy(TIntV& IntV, int* IntNumpyVecOut, int n);
	/// Converts TFltV to Numpy array.
	void TFltVToNumpy(TFltV& FltV, float* FltNumpyVecOut, int n);
	/// Converts NumpyArray to TIntV
	void NumpyToTIntV(TIntV& IntV, int* IntNumpyVecIn, int n);
	/// Converts NumpyArray to TFltV
	void NumpyToTFltV(TFltV& FltV, float* FltNumpyVecIn, int n);
}

#endif //NUMPY_H

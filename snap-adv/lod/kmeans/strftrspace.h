/*
 * strftrspace.h
 *
 *  Created on: Jul 6, 2010
 *      Author: tadej
 */

#ifndef STRFTRSPACE_H_
#define STRFTRSPACE_H_
#include "../stdafx.h"


typedef TInt TStrFSSize;

ClassTP(TStrFeatureSpace, PStrFeatureSpace) // {
	TVec<TStrFSSize> Space;
	TStrHash<TStrFSSize> ISpace;

public:
	static PStrFeatureSpace New();
	TStrFeatureSpace();
	~TStrFeatureSpace();
	TStrFeatureSpace(TSIn& In);
	static PStrFeatureSpace Load(TSIn& SIn);

	void Save(TSOut& Out) const;

	const TStrFSSize GetId(const char *Feature) const;
	const TStrFSSize GetId(const TStr& Feature) const;
	bool GetIfExistsId(const char * Feature, TStrFSSize& Id) const;
	bool GetIfExistsId(const TStr& Feature, TStrFSSize& Id) const;

	void GetAddIds(const TStrV& Features, TVec<TStrFSSize>& Ids);
	void GetIds(const TStrV& Features, TVec<TStrFSSize>& Ids) const;
	void GetAddIdFreqs(const TVec<const char *>& Features,  TVec<TKeyDat<TStrFSSize, TInt> >& IdFreqs);
	void GetAddIdFreqs(const TStrV& Features, TVec<TKeyDat<TStrFSSize, TInt> >& IdFreqs);
	void GetIdFreqs(const TVec<const char *>& Features,  TVec<TKeyDat<TStrFSSize, TInt> >& IdFreqs) const;
	void GetIdFreqs(const TStrV& Features,  TVec<TKeyDat<TStrFSSize, TInt> >& IdFreqs) const;

	TStrFSSize GetAddId(const TStr& Feature);
	TStrFSSize GetAddId(const char *Feature);
	const char *Get(TStrFSSize) const;
	void Get(const TVec<TStrFSSize>& Ids, TStrV& Features) const;

	TStrFSSize Len() const;

	void ToStr(const TIntFltKdV& FeatureIds, TChA& ChA, int k = 10, char Sep = ' ') const;
	void ToStr(const TFltV& FeatureIds, TChA& ChA, int k = 10, char Sep = ' ') const;
	void ToStr(const TVec<TStrFSSize>& FeatureIds, TChA& ChA, char Sep = ' ') const;

	TStr ToStr(const TVec<TStrFSSize>& FeatureIds, char Sep = ' ') const;
	TStr ToStr(const TFltV& FeatureIds, int k = 10, char Sep = ' ') const;
	TStr ToStr(const TIntFltKdV& FeatureIds, int k = 10, char Sep = ' ') const;

	void FromStr(const TStr& Serialized, TIntFltKdV& FeatureIdsWgts, char Sep = ' ') const;
	void FromAddStr(const TStr& Serialized, TIntFltKdV& FeatureIdsWgts, char Sep = ' ');

	void SaveTxt(TSOut& Out) const;

};

#endif /* STRFTRSPACE_H_ */

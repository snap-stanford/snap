/*
 * ftrspace.cpp
 *
 *  Created on: Jun 7, 2010
 *      Author: tadej
 */
//#include "stdafx.h"
#include <stdafx.h>
#include "strftrspace.h"

TStrFeatureSpace::TStrFeatureSpace() {
}

TStrFeatureSpace::~TStrFeatureSpace() {
}

PStrFeatureSpace TStrFeatureSpace::New() {
	return PStrFeatureSpace(new TStrFeatureSpace());
}

TStrFeatureSpace::TStrFeatureSpace(TSIn& In) {
	TInt Size(In);
	Space.Gen(Size,0);
	for (TStrFSSize i = 0; i < Size; i++) {
		int StrLen;
		In.Load(StrLen);
		char *Bf = new char[StrLen + 1];
		In.GetBf(Bf, StrLen);
		Bf[StrLen] = '\0';
		//printf("%d: [%s]\n", StrLen, Bf);
		int KeyId = ISpace.AddDat(Bf, i);
		Space.Add(ISpace.GetKeyOfs(KeyId));
		delete[] Bf;
	}
}


PStrFeatureSpace TStrFeatureSpace::Load(TSIn& SIn) {
	return PStrFeatureSpace(new TStrFeatureSpace(SIn));
}

void TStrFeatureSpace::Save(TSOut& Out) const {
	TStrFSSize Len = Space.Len();
	Out.Save(Len);
	for (TStrFSSize i = 0; i < Space.Len(); i++) {
		const char *Ftr = ISpace.KeyFromOfs(Space[i]);
		TInt StrLen = strlen(Ftr);
		StrLen.Save(Out);
		Out.PutBf(Ftr, StrLen);
	}
}

bool TStrFeatureSpace::GetIfExistsId(const TStr& Feature, TStrFSSize& Id) const {
	return ISpace.IsKeyGetDat(Feature, Id);
}

bool TStrFeatureSpace::GetIfExistsId(const char * Feature, TStrFSSize& Id) const {
	return ISpace.IsKeyGetDat(Feature, Id);
}

const TStrFSSize TStrFeatureSpace::GetId(const char *Feature) const {
	return ISpace.GetDat(Feature);
}

const TStrFSSize TStrFeatureSpace::GetId(const TStr& Feature) const {
	return ISpace.GetDat(Feature);
}

void TStrFeatureSpace::GetIds(const TStrV& Features, TVec<TStrFSSize>& Ids) const {
	for (int i = 0; i < Features.Len(); i++) {
		int KeyId = ISpace.GetKeyId(Features[i]);
		if (KeyId == -1) {
			printf("Warning: unknown token: %s\n", Features[i].CStr());
		} else {
			Ids.Add(ISpace[KeyId]);
		}
	}
}

void TStrFeatureSpace::GetAddIds(const TStrV& Features, TVec<TStrFSSize>& Ids) {
	for (int i = 0; i < Features.Len(); i++) {
		Ids.Add(GetAddId(Features[i]));
	}
}

void TStrFeatureSpace::GetAddIdFreqs(const TStrV& Features,  TVec<TKeyDat<TStrFSSize, TInt> >& IdFreqs) {
	TIntH Freqs;
	for (int i = 0; i < Features.Len(); i++) {
		int Id = GetAddId(Features[i]);

		TInt Freq = 0;
		if (Freqs.IsKeyGetDat(Id, Freq)) {
			Freqs.AddDat(Id, Freq + 1);
		} else {
			Freqs.AddDat(Id, 1);
		}
	}

	IdFreqs.Gen(Freqs.Len());
	for (int i = 0; i < Freqs.Len(); i++) {
		TInt Key, Freq;
		Freqs.GetKeyDat(i, Key, Freq);
		IdFreqs[i].Key = Key;
		IdFreqs[i].Dat = Freq;
	}
	IdFreqs.Sort();
}

void TStrFeatureSpace::GetAddIdFreqs(const TVec<const char *>& Features,  TVec<TKeyDat<TStrFSSize, TInt> >& IdFreqs) {
	TIntH Freqs;
	for (int i = 0; i < Features.Len(); i++) {
		int Id = GetAddId(Features[i]);

		TInt Freq = 0;
		if (Freqs.IsKeyGetDat(Id, Freq)) {
			Freqs.AddDat(Id, Freq + 1);
		} else {
			Freqs.AddDat(Id, 1);
		}
	}

	IdFreqs.Gen(Freqs.Len());
	for (int i = 0; i < Freqs.Len(); i++) {
		TInt Key, Freq;
		Freqs.GetKeyDat(i, Key, Freq);
		IdFreqs[i].Key = Key;
		IdFreqs[i].Dat = Freq;
	}
	IdFreqs.Sort();
}

void TStrFeatureSpace::GetIdFreqs(const TStrV& Features,  TVec<TKeyDat<TStrFSSize, TInt> >& IdFreqs) const {
	TIntH Freqs;
	for (int i = 0; i < Features.Len(); i++) {
		TInt Id;
		if (GetIfExistsId(Features[i].CStr(), Id)) {
			TInt Freq = 0;
			if (Freqs.IsKeyGetDat(Id, Freq)) {
				Freqs.AddDat(Id, Freq + 1);
			} else {
				Freqs.AddDat(Id, 1);
			}
		}
	}

	IdFreqs.Gen(Freqs.Len());
	for (int i = 0; i < Freqs.Len(); i++) {
		TInt Key, Freq;
		Freqs.GetKeyDat(i, Key, Freq);
		IdFreqs[i].Key = Key;
		IdFreqs[i].Dat = Freq;
	}
	IdFreqs.Sort();
}

void TStrFeatureSpace::GetIdFreqs(const TVec<const char *>& Features,  TVec<TKeyDat<TStrFSSize, TInt> >& IdFreqs) const {
	TIntH Freqs;
	for (int i = 0; i < Features.Len(); i++) {
		TInt Id;
		if (GetIfExistsId(Features[i], Id)) {
			TInt Freq = 0;
			if (Freqs.IsKeyGetDat(Id, Freq)) {
				Freqs.AddDat(Id, Freq + 1);
			} else {
				Freqs.AddDat(Id, 1);
			}
		}
	}

	IdFreqs.Gen(Freqs.Len());
	for (int i = 0; i < Freqs.Len(); i++) {
		TInt Key, Freq;
		Freqs.GetKeyDat(i, Key, Freq);
		IdFreqs[i].Key = Key;
		IdFreqs[i].Dat = Freq;
	}
	IdFreqs.Sort();
}


void TStrFeatureSpace::Get(const TVec<TStrFSSize>& Ids, TStrV& Features) const {
	for (int i = 0; i < Ids.Len(); i++) {
		Features.Add(Get(Ids[i]));
	}
}

TStrFSSize TStrFeatureSpace::GetAddId(const char *Feature) {
	TStrFSSize Id;
	if (ISpace.IsKeyGetDat(Feature, Id)) {
		return Id;
	} else {
		Id = Space.Len();
		int KeyId = ISpace.AddDat(Feature, Id);
		//const char *Literal = ISpace.GetKey(KeyId);
		Space.Add(ISpace.GetKeyOfs(KeyId));
		return Id;
	}
}

TStrFSSize TStrFeatureSpace::GetAddId(const TStr& Feature) {
	TStrFSSize Id;
	if (ISpace.IsKeyGetDat(Feature, Id)) {
		return Id;
	} else {
		Id = Space.Len();
		int KeyId = ISpace.AddDat(Feature, Id);
		//const char *Literal = ISpace.GetKey(KeyId);
		Space.Add(ISpace.GetKeyOfs(KeyId));
		return Id;
	}
}
const char *TStrFeatureSpace::Get(TStrFSSize id) const {
	return ISpace.KeyFromOfs(Space[id]);
}

TStrFSSize TStrFeatureSpace::Len() const {
	return Space.Len();
}


TStr TStrFeatureSpace::ToStr(const TVec<TStrFSSize>& FeatureIds, char Sep) const {
	TChA ChA;
	ToStr(FeatureIds, ChA, Sep);
	return ChA;
}

void TStrFeatureSpace::ToStr(const TVec<TStrFSSize>& FeatureIds, TChA& ChA, char Sep) const {
	for (TStrFSSize i = 0; i < FeatureIds.Len(); i++) {
		ChA += ISpace.KeyFromOfs(Space[FeatureIds[i]]);
		if (i < FeatureIds.Len() - 1) {
			ChA += Sep;
		}
	}
}

TStr TStrFeatureSpace::ToStr(const TFltV& FeatureIds, int k, char Sep) const {
	TChA ChA;
	ToStr(FeatureIds, ChA, k, Sep);
	return ChA;
}

void TStrFeatureSpace::ToStr(const TFltV& FeatureIds, TChA& ChA, int k, char Sep) const {
	TIntSet TakenIndexes(k);
	int Len = TMath::Mn(FeatureIds.Len(), k);
	for (int i = 0; i < Len; i++) {
		double MxVal = TFlt::Mn;
		int MxIndex = 0;
		for (int j = 0; j < FeatureIds.Len(); j++) {
			const TFlt& FeatureVal = FeatureIds[j];
			if (FeatureVal > MxVal) {
				if (!TakenIndexes.IsKey(j)) {
					MxVal = FeatureVal;
					MxIndex = j;
				}
			}
		}
		TakenIndexes.AddKey(MxIndex);

		ChA += ISpace.KeyFromOfs(Space[MxIndex]);
		ChA += ':';
		ChA += TFlt::GetStr(MxVal, "%2.6f");;
		if (i < Len) {
			ChA += Sep;
		}
	}
}



TStr TStrFeatureSpace::ToStr(const TIntFltKdV& FeatureIds, int k, char Sep) const {
	TChA ChA;
	ToStr(FeatureIds, ChA, k, Sep);
	return ChA;
}

void TStrFeatureSpace::ToStr(const TIntFltKdV& FeatureIds, TChA& ChA, int k, char Sep) const {
	TIntSet TakenIndexes(k);
	int Len = TMath::Mn(FeatureIds.Len(), k);
	for (int i = 0; i < Len; i++) {
		double MxVal = TFlt::Mn;
		int MxIndex = 0;
		for (int j = 0; j < FeatureIds.Len(); j++) {
			const TIntFltKd& Feature = FeatureIds[j];
			if (Feature.Dat > MxVal) {
				if (!TakenIndexes.IsKey(Feature.Key)) {
					MxVal = Feature.Dat;
					MxIndex = Feature.Key;
				}
			}
		}
		TakenIndexes.AddKey(MxIndex);

		ChA += ISpace.KeyFromOfs(Space[MxIndex]);
		ChA += ':';
		ChA += TFlt::GetStr(MxVal, "%2.6f");
		if (i < Len) {
			ChA += Sep;
		}
	}
}

void TStrFeatureSpace::FromAddStr(const TStr& Serialized, TIntFltKdV& Vec, char Sep) {
	TStrV Toks;
	Serialized.SplitOnAllCh(Sep, Toks, true);
	Vec.Gen(Toks.Len());
	for (int i = 0; i < Toks.Len(); i++) {
		TStr Key, Value;
		Toks[i].SplitOnCh(Key, ':', Value);
		int FeatureId = GetAddId(Key);
		double FeatureWgt;
		if (Value.IsFlt(FeatureWgt)) {
			Vec[i].Key = FeatureId;
			Vec[i].Dat = FeatureWgt;
		} else {
			EFailR((Value + TStr(" is not a valid floating point number.")).CStr());
		}
	}
	Vec.Sort();
}

void TStrFeatureSpace::FromStr(const TStr& Serialized, TIntFltKdV& Vec, char Sep) const {
	TStrV Toks;
	Serialized.SplitOnAllCh(Sep, Toks, true);
	Vec.Gen(Toks.Len(),0);
	for (int i = 0; i < Toks.Len(); i++) {
		TStr Key, Value;
		Toks[i].SplitOnCh(Key, ':', Value);
		TStrFSSize FeatureId;
		if (GetIfExistsId(Key, FeatureId)) {
			double FeatureWgt;
			if (Value.IsFlt(FeatureWgt)) {
				TIntFltKd& Kv = Vec[Vec.Add()];
				Kv.Key = FeatureId;
				Kv.Dat = FeatureWgt;
			} else {
				EFailR((Value + TStr(" is not a valid floating point number.")).CStr());
			}
		}
	}

	Vec.Sort();
}

void TStrFeatureSpace::SaveTxt(TSOut& Out) const {
	for (int i = 0; i < Len(); i++) {
		Out.PutInt(i);
		Out.PutCh('\t');
		const char *item = Get(i);
		Out.PutBf(item, strlen(item));
		Out.PutLn();
	}
}









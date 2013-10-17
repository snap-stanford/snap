%pythoncode %{

#
# define __getitem__ for [] addressing
#

def getitem(self, i):
    return self.GetVal(i)

#
# define iterator for TVec
#

class IterVec:
    def __init__(self, vec):
        self.vec = vec
        self.count = -1

    def __iter__(self):
        return self

    def next(self):
        if self.count+1 < self.vec.Len():
            self.count += 1
            return self.vec[self.count]

        raise StopIteration

def itervec(self):
    return IterVec(self)

# expand TVec types with methods __iter__ and __getitem__

TIntV.__getitem__ = getitem
TIntV.__iter__ = itervec
TFltV.__getitem__ = getitem
TFltV.__iter__ = itervec
TIntPrV.__getitem__ = getitem
TIntPrV.__iter__ = itervec
TFltPrV.__getitem__ = getitem
TFltPrV.__iter__ = itervec
TIntFltKdV.__getitem__ = getitem
TIntFltKdV.__iter__ = itervec
TCnComV.__getitem__ = getitem
TCnComV.__iter__ = itervec

TCnCom.__getitem__ = getitem
TCnCom.__iter__ = itervec
%}

#if SNAP_ALL
%pythoncode %{
TBoolV.__getitem__ = getitem
TBoolV.__iter__ = itervec
TChV.__getitem__ = getitem
TChV.__iter__ = itervec
TUChV.__getitem__ = getitem
TUChV.__iter__ = itervec
TUIntV.__getitem__ = getitem
TUIntV.__iter__ = itervec
TUInt64V.__getitem__ = getitem
TUInt64V.__iter__ = itervec
TSFltV.__getitem__ = getitem
TSFltV.__iter__ = itervec
TAscFltV.__getitem__ = getitem
TAscFltV.__iter__ = itervec
TStrV.__getitem__ = getitem
TStrV.__iter__ = itervec
TChAV.__getitem__ = getitem
TChAV.__iter__ = itervec
TIntTrV.__getitem__ = getitem
TIntTrV.__iter__ = itervec
TIntQuV.__getitem__ = getitem
TIntQuV.__iter__ = itervec
TFltTrV.__getitem__ = getitem
TFltTrV.__iter__ = itervec
TIntKdV.__getitem__ = getitem
TIntKdV.__iter__ = itervec
TUChIntPrV.__getitem__ = getitem
TUChIntPrV.__iter__ = itervec
TUChUInt64PrV.__getitem__ = getitem
TUChUInt64PrV.__iter__ = itervec
TIntUInt64PrV.__getitem__ = getitem
TIntUInt64PrV.__iter__ = itervec
TIntUInt64KdV.__getitem__ = getitem
TIntUInt64KdV.__iter__ = itervec
TIntFltPrV.__getitem__ = getitem
TIntFltPrV.__iter__ = itervec
TIntFltPrKdV.__getitem__ = getitem
TIntFltPrKdV.__iter__ = itervec
TFltIntPrV.__getitem__ = getitem
TFltIntPrV.__iter__ = itervec
TFltUInt64PrV.__getitem__ = getitem
TFltUInt64PrV.__iter__ = itervec
TFltStrPrV.__getitem__ = getitem
TFltStrPrV.__iter__ = itervec
TAscFltStrPrV.__getitem__ = getitem
TAscFltStrPrV.__iter__ = itervec
TIntStrPrV.__getitem__ = getitem
TIntStrPrV.__iter__ = itervec
TIntIntStrTrV.__getitem__ = getitem
TIntIntStrTrV.__iter__ = itervec
TIntIntFltTrV.__getitem__ = getitem
TIntIntFltTrV.__iter__ = itervec
TIntFltIntTrV.__getitem__ = getitem
TIntFltIntTrV.__iter__ = itervec
TIntStrIntTrV.__getitem__ = getitem
TIntStrIntTrV.__iter__ = itervec
TIntKdV.__getitem__ = getitem
TIntKdV.__iter__ = itervec
TUIntIntKdV.__getitem__ = getitem
TUIntIntKdV.__iter__ = itervec
TIntPrFltKdV.__getitem__ = getitem
TIntPrFltKdV.__iter__ = itervec
TIntStrKdV.__getitem__ = getitem
TIntStrKdV.__iter__ = itervec
TIntStrPrPrV.__getitem__ = getitem
TIntStrPrPrV.__iter__ = itervec
TIntStrVPrV.__getitem__ = getitem
TIntStrVPrV.__iter__ = itervec
TIntIntVIntTrV.__getitem__ = getitem
TIntIntVIntTrV.__iter__ = itervec
TIntIntIntVTrV.__getitem__ = getitem
TIntIntIntVTrV.__iter__ = itervec
TUInt64IntPrV.__getitem__ = getitem
TUInt64IntPrV.__iter__ = itervec
TUInt64FltPrV.__getitem__ = getitem
TUInt64FltPrV.__iter__ = itervec
TUInt64StrPrV.__getitem__ = getitem
TUInt64StrPrV.__iter__ = itervec
TUInt64IntKdV.__getitem__ = getitem
TUInt64IntKdV.__iter__ = itervec
TUInt64FltKdV.__getitem__ = getitem
TUInt64FltKdV.__iter__ = itervec
TUInt64StrKdV.__getitem__ = getitem
TUInt64StrKdV.__iter__ = itervec
TFltBoolKdV.__getitem__ = getitem
TFltBoolKdV.__iter__ = itervec
TFltIntKdV.__getitem__ = getitem
TFltIntKdV.__iter__ = itervec
TFltUInt64KdV.__getitem__ = getitem
TFltUInt64KdV.__iter__ = itervec
TFltIntPrKdV.__getitem__ = getitem
TFltIntPrKdV.__iter__ = itervec
TFltKdV.__getitem__ = getitem
TFltKdV.__iter__ = itervec
TFltStrKdV.__getitem__ = getitem
TFltStrKdV.__iter__ = itervec
TFltStrPrPrV.__getitem__ = getitem
TFltStrPrPrV.__iter__ = itervec
TFltIntIntTrV.__getitem__ = getitem
TFltIntIntTrV.__iter__ = itervec
TFltFltStrTrV.__getitem__ = getitem
TFltFltStrTrV.__iter__ = itervec
TAscFltIntPrV.__getitem__ = getitem
TAscFltIntPrV.__iter__ = itervec
TAscFltIntKdV.__getitem__ = getitem
TAscFltIntKdV.__iter__ = itervec
TStrPrV.__getitem__ = getitem
TStrPrV.__iter__ = itervec
TStrIntPrV.__getitem__ = getitem
TStrIntPrV.__iter__ = itervec
TStrFltPrV.__getitem__ = getitem
TStrFltPrV.__iter__ = itervec
TStrIntKdV.__getitem__ = getitem
TStrIntKdV.__iter__ = itervec
TStrFltKdV.__getitem__ = getitem
TStrFltKdV.__iter__ = itervec
TStrAscFltKdV.__getitem__ = getitem
TStrAscFltKdV.__iter__ = itervec
TStrTrV.__getitem__ = getitem
TStrTrV.__iter__ = itervec
TStrQuV.__getitem__ = getitem
TStrQuV.__iter__ = itervec
TStrFltFltTrV.__getitem__ = getitem
TStrFltFltTrV.__iter__ = itervec
TStrStrIntTrV.__getitem__ = getitem
TStrStrIntTrV.__iter__ = itervec
TStrKdV.__getitem__ = getitem
TStrKdV.__iter__ = itervec
TStrStrVPrV.__getitem__ = getitem
TStrStrVPrV.__iter__ = itervec
TStrVIntPrV.__getitem__ = getitem
TStrVIntPrV.__iter__ = itervec
TFltIntIntIntQuV.__getitem__ = getitem
TFltIntIntIntQuV.__iter__ = itervec
TIntStrIntIntQuV.__getitem__ = getitem
TIntStrIntIntQuV.__iter__ = itervec
TIntIntPrPrV.__getitem__ = getitem
TIntIntPrPrV.__iter__ = itervec
PFltV.__getitem__ = getitem
PFltV.__iter__ = itervec
PAscFltV.__getitem__ = getitem
PAscFltV.__iter__ = itervec
PStrV.__getitem__ = getitem
PStrV.__iter__ = itervec
TBoolVV.__getitem__ = getitem
TBoolVV.__iter__ = itervec
TChVV.__getitem__ = getitem
TChVV.__iter__ = itervec
TIntVV.__getitem__ = getitem
TIntVV.__iter__ = itervec
TSFltVV.__getitem__ = getitem
TSFltVV.__iter__ = itervec
TFltVV.__getitem__ = getitem
TFltVV.__iter__ = itervec
TStrVV.__getitem__ = getitem
TStrVV.__iter__ = itervec
TIntPrVV.__getitem__ = getitem
TIntPrVV.__iter__ = itervec
TIntVVV.__getitem__ = getitem
TIntVVV.__iter__ = itervec
TFltVVV.__getitem__ = getitem
TFltVVV.__iter__ = itervec
#TIntQV.__getitem__ = getitem
#TIntQV.__iter__ = itervec
TStrV.__getitem__ = getitem
TStrV.__iter__ = itervec
%}
#endif


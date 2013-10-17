%pythoncode %{

#
# define iterator for THash
#

class IterHash:
    def __init__(self, hash):
        self.hash = hash
        self.iter = None

    def __iter__(self):
        return self

    def next(self):
        if not self.iter:
            self.iter = self.hash.BegI()
            if not self.iter:
                raise StopIteration
            return self.iter

        if self.iter.IsEnd():
            raise StopIteration

        self.iter.Next()

        if self.iter.IsEnd():
            raise StopIteration
     
        return self.iter

def iterhash(self):
    return IterHash(self)

TIntH.__iter__ = iterhash
TIntIntH.__iter__ = iterhash
TIntFltH.__iter__ = iterhash
TIntStrH.__iter__ = iterhash
TIntPrFltH.__iter__ = iterhash
%}

#if SNAP_ALL
%pythoncode %{
TUInt64H.__iter__ = iterhash
TIntBoolH.__iter__ = iterhash
TIntUInt64H.__iter__ = iterhash
TIntIntVH.__iter__ = iterhash
TIntIntHH.__iter__ = iterhash
TIntFltPrH.__iter__ = iterhash
TIntFltTrH.__iter__ = iterhash
TIntFltVH.__iter__ = iterhash
TIntStrVH.__iter__ = iterhash
TIntIntPrH.__iter__ = iterhash
TIntIntPrVH.__iter__ = iterhash
TUInt64StrVH.__iter__ = iterhash
TIntPrIntH.__iter__ = iterhash
TIntPrIntVH.__iter__ = iterhash
TIntPrIntPrVH.__iter__ = iterhash
TIntTrIntH.__iter__ = iterhash
TIntVIntH.__iter__ = iterhash
TUIntH.__iter__ = iterhash
TIntPrIntH.__iter__ = iterhash
TIntPrIntVH.__iter__ = iterhash
TIntTrFltH.__iter__ = iterhash
TIntPrStrH.__iter__ = iterhash
TIntPrStrVH.__iter__ = iterhash
TIntStrPrIntH.__iter__ = iterhash
TFltFltH.__iter__ = iterhash
TStrH.__iter__ = iterhash
TStrBoolH.__iter__ = iterhash
TStrIntH.__iter__ = iterhash
TStrIntPrH.__iter__ = iterhash
TStrIntVH.__iter__ = iterhash
TStrUInt64H.__iter__ = iterhash
TStrUInt64VH.__iter__ = iterhash
TStrIntPrVH.__iter__ = iterhash
TStrFltH.__iter__ = iterhash
TStrFltVH.__iter__ = iterhash
TStrStrH.__iter__ = iterhash
TStrStrPrH.__iter__ = iterhash
TStrStrVH.__iter__ = iterhash
TStrStrPrVH.__iter__ = iterhash
TStrStrKdVH.__iter__ = iterhash
TStrIntFltPrH.__iter__ = iterhash
TStrStrIntPrVH.__iter__ = iterhash
TStrStrIntKdVH.__iter__ = iterhash
TStrPrBoolH.__iter__ = iterhash
TStrPrIntH.__iter__ = iterhash
TStrPrFltH.__iter__ = iterhash
TStrPrStrH.__iter__ = iterhash
TStrPrStrVH.__iter__ = iterhash
TStrTrIntH.__iter__ = iterhash
TStrIntPrIntH.__iter__ = iterhash
TStrVH.__iter__ = iterhash
TStrVIntVH.__iter__ = iterhash
TStrVStrH.__iter__ = iterhash
TStrVStrVH.__iter__ = iterhash
%}
#endif


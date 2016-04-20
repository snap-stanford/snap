namespace TSnap {
PNGraph CascGraphSource(PTable P,const TStr C1,const TStr C2,const TStr C3,const TStr C4,const TInt W);

PNGraph CascGraphTime(PTable P,const TStr C1,const TStr C2,const TStr C3,const TStr C4,const TInt W);

PNGraph CascGraph(PTable P,const TStr C1,const TStr C2,const TStr C3,const TStr C4,const TInt W,bool SortParam = true);

void CascFind(PNGraph Graph,PTable P,const TStr C1,const TStr C2,const TStr C3,const TStr C4,TVec<TIntV> &TopCascVV,bool Print = false);

#ifdef USE_OPENMP
void CascFindMP(PNGraph Graph,PTable P,const TStr C1,const TStr C2,const TStr C3,const TStr C4,TVec<TIntV> &TopCascVV);
#endif
}

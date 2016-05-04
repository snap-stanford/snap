namespace TSnap {
/// Takes as input the column names of the PTable \c P as \c C1, \c C2,\c C3 and \c C4 and returns a directed graph of \c W-adjacent events. For graph generation events are sorted by \c C1. 
PNGraph CascGraphSource(PTable P,const TStr C1,const TStr C2,const TStr C3,const TStr C4,const TInt W);
/// Takes as input the column names of the PTable \c P as \c C1, \c C2,\c C3 and \c C4 and returns a directed graph of \c W-adjacent events. For graph generation events are sorted by \c C3. 
PNGraph CascGraphTime(PTable P,const TStr C1,const TStr C2,const TStr C3,const TStr C4,const TInt W);
/// Takes as input the column names of the PTable \c P as \c C1, \c C2, \c C3 and \c C4 and returns a directed graph of \c W-adjacent events. By default calls CascGraphSource. Toggle SortParam to use CascGraphTime.
PNGraph CascGraph(PTable P,const TStr C1,const TStr C2,const TStr C3,const TStr C4,const TInt W,bool SortParam = true);
/// Takes as input a directed graph and returns all the top cascades in \c TopCascVV.
void CascFind(PNGraph Graph,PTable P,const TStr C1,const TStr C2,const TStr C3,const TStr C4,TVec<TIntV> &TopCascVV,bool Print = false);
/// Parallel implementaion of CascFind takes as input a directed graph and returns all the top cascades in \c TopCascVV.
#ifdef USE_OPENMP
void CascFindMP(PNGraph Graph,PTable P,const TStr C1,const TStr C2,const TStr C3,const TStr C4,TVec<TIntV> &TopCascVV);
#endif
}

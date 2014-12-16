namespace TSnap {
/// Girvan-Newman community detection algorithm based on Betweenness centrality.
/// See: Girvan M. and Newman M. E. J., Community structure in social and biological networks, Proc. Natl. Acad. Sci. USA 99, 7821-7826 (2002)
int FastCorePeriphery(PUNGraph& Graph, TIntIntH& out);
int FastCorePeripheryGC(PUNGraph& Graph, TIntIntH& out);
double BorgattiEverettMeasure(PUNGraph& Graph, TIntIntH& out, double coresize, int type);
double PearsonCorrelation(PUNGraph& Graph, TIntIntH& out, int coresize);
}
#ifndef SNAP_TYPEDEFS_H
#define SNAP_TYPEDEFS_H

#include "stdafx.h"

typedef TNodeEdgeNet<TInt,TInt> TGraph;
typedef TPt<TGraph> PGraph;

struct TIntFltKdComp {
  bool operator() (const TIntFltKd &e1, const TIntFltKd &e2) const
  {
    return e1.Dat > e2.Dat;
  }
};

#endif

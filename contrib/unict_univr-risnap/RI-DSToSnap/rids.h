/*
Copyright (c) 2015 by Rosalba Giugno

This library contains portions of other open source products covered by separate
licenses. Please see the corresponding source files for specific terms.

RI is provided under the terms of The MIT License (MIT):

Permission is hereby granted, free of charge, to any person obtaiNIdng a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furNIdshed to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef RI3_H
#define	RI3_H

#include <map>
#include <math.h>
#include <stdint.h>
//#include <sstream>
#include "Snap.h"

typedef uint32_t u_size_t;
typedef int32_t s_size_t;
typedef u_size_t sbitset_block;

enum TMamaParent { mpParentTypeIn, mpParentTypeOut, mpParentTypeNull };
enum TMatch { mIso, mIndSub, mMono };

//#//////////////////////////////////////////////
///
class TIDer{
private:
	std::map<std::string, int> TIMap;
public:
	TIDer() { }
	~TIDer(){ }

	s_size_t IdFor(std::string* Value) {
		int Ret = 0;
		if(Value == NULL) {
			Ret = 0;
		} else {
			std::map<std::string, int>::iterator IT = TIMap.find(*Value);
			if(IT == TIMap.end()) {
				TIMap.insert(*(new std::pair<std::string, int>(*Value, ((int)TIMap.size())+1)));
				Ret = ((int)TIMap.size());
			} else {
				Ret = IT->second;
			}
		}
		return Ret;
	}
};

//#//////////////////////////////////////////////
///
class TNodeList {
public:
  /// ##
  virtual ~TNodeList() {
  };
  /// ##
  virtual int GetNode(int Id) = 0;
  virtual void AddNode(int Id){};
};

//#//////////////////////////////////////////////
///
template<class TNodeData, class TEdgeData>
class TInAdjList : public TNodeList {
private:
  TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Net;
  int NodeId;
public:
  /// ##
  TInAdjList(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &NNet, int NNodeId):
    Net(NNet), NodeId(NNodeId) { }
  /// ##
  int GetNode(int Id) {
    return Net->GetNI(NodeId).GetInNId(Id);
  }
};

//#//////////////////////////////////////////////
///
template<class TNodeData, class TEdgeData>
class TOutAdjList : public TNodeList {
private:
  TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Net;
  int NodeId;
public:
  /// ##
  TOutAdjList(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &NNet, int NNodeId):
    Net(NNet), NodeId(NNodeId) { }
  /// ##
  int GetNode(int Id) {
    return Net->GetNI(NodeId).GetOutNId(Id);
  }
};

//#//////////////////////////////////////////////
/// List of all graph's nodes ##All_nodes_list
template<class TNodeData, class TEdgeData>
class TAllNodesList : public TNodeList {
private:
  TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Net;
  int NodeId;
public:
  /// ##
  TAllNodesList(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &NNet):
    Net(NNet) { }
  /// ##
  int GetNode(int Id) {
    return Net->GetNI(Id).GetId();
  }
};

//#//////////////////////////////////////////////
/// List of candidates graph's nodes ##Candidates_list
template<class TNodeData, class TEdgeData>
class TCandidatesList : public TNodeList {
private:
  TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Net;
  int NodeId;
  int* NodesList;
  int curr;
public:
  
  TCandidatesList(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &NNet, int Size):
    Net(NNet),
    NodesList(new int[Size]),
    curr(0) { }

  
  int GetNode(int Id) { return Net->GetNI(NodesList[Id]).GetId(); }
  
  void AddNode(int Id) {
    NodesList[curr] = Id;
    curr++;
  }
  
};

/// ##GetAdjSize
template<class TNodeData, class TEdgeData>
int GetAdjSize(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Graph, int Id) {
  return Graph->GetNI(Id).GetDeg();
}

/// ##GetInAdjSize
template<class TNodeData, class TEdgeData>
int GetInAdjSize(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Graph, int Id) {
  return Graph->GetNI(Id).GetInDeg();
}

/// ##GetOutAdjSize
template<class TNodeData, class TEdgeData>
int GetOutAdjSize(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Graph, int Id) {
  return Graph->GetNI(Id).GetOutDeg();
}

/// ##GetNodeInAdjList
template<class TNodeData, class TEdgeData>
int GetNodeInAdjList(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Graph, int NId, int N) {
  typename TNodeEDatNet<TNodeData, TEdgeData>::TNodeI NI;
  NI = Graph->GetNI(NId);
  return NI.GetNbrNId(N);
};

/// ##GetNthOutAdjList
template<class TNodeData, class TEdgeData>
int GetNthOutAdjList(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Graph, int NId, int N) {
  return Graph->GetNI(NId).GetOutNId(N);
}

/// ##GetNthInAdjList
template<class TNodeData, class TEdgeData>
int GetNthInAdjList(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Graph, int NId, int N) {
  return Graph->GetNI(NId).GetInNId(N);
}
template<class TNodeData, class TEdgeData>
int GetEdgeId(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Graph, int NId, int N) {
  //TODO Array bidimensionale [NOfNodes]
  return Graph->GetEI(NId, N).GetId();
}

//#//////////////////////////////////////////////
///
class TSBitSet {
static const size_t bytes_per_block = sizeof(sbitset_block);
static const size_t bits_per_block = sizeof(sbitset_block)*8;

public:
	sbitset_block* Bits;
	size_t NBlocks;

public:
	TSBitSet() {
		Bits = (sbitset_block*)calloc(1, bytes_per_block);
		NBlocks = 1;
	}

	TSBitSet(const TSBitSet &c) {
		Bits = (sbitset_block*)malloc(c.NBlocks * bytes_per_block);
		memcpy(Bits, c.Bits, c.NBlocks * bytes_per_block);
		NBlocks = c.NBlocks;
	}
	TSBitSet(TSBitSet &c) {
		Bits = (sbitset_block*)malloc(c.NBlocks * bytes_per_block);
		memcpy(Bits, c.Bits, c.NBlocks * bytes_per_block);
		NBlocks = c.NBlocks;
	}

	~TSBitSet() {
		delete [] Bits;
	}


	void PrintI(std::ostream &os) {
		for(size_t i = 0; i < NBlocks * bits_per_block; i++){
			os<<"("<<i<<":"<<Get(i)<<")";
		}
	}

	void PrintNumbers() {
		std::cout<<"[";
		for(size_t i=0; i < NBlocks * bits_per_block; i++) {
			if(Get(i))
				std::cout<<i<<" ";
		}
		std::cout<<"]";
	}
	void PrintInfo() { }

	bool IsEmpty() {
		for(size_t i = 0; i < NBlocks; i++) {
			if(Bits[i]!=0){
				return false;
			}
		}
		return true;
	}

	int CountOnes() {
		int Count = 0;
		for(TIterator IT = FirstOnes(); IT != End(); IT.next_ones()) {
			Count++;
		}
		return Count;
	}

	bool AtLeastOne() {
		TIterator IT = FirstOnes();
		return IT != End();
	}

	bool Get(size_t Pos) {
		if(Pos >= 0 && Pos < (NBlocks * bits_per_block)) {
			return Bits[Pos/bits_per_block] &
              (1<<(bits_per_block - (Pos%bits_per_block)-1));
		}
		return 0;
	}

	void Resize(size_t NSize) {
		//TODO not used but nedded for future version
//		if(new_size < _nblocks * bitsof_block){
//		}
//		else
		if(NSize > NBlocks * bits_per_block){
			size_t NNBlocks = (size_t)(ceil(NSize / bits_per_block) +1);
			sbitset_block* NBits = (sbitset_block*)calloc(NNBlocks, bytes_per_block);
			memcpy(NBits, Bits, NBlocks * bytes_per_block);
			NBlocks = NNBlocks;
			delete [] Bits;
			Bits = NBits;
		}
	}

	void ResizeAsis(size_t NSize) {
		//TODO not used but nedded for future version
//		if(new_size < _nblocks * bitsof_block){
//		}
//		else
		if(NSize > NBlocks * bits_per_block) {
			size_t NNBlocks = (size_t)(ceil(NSize / bits_per_block) +1);
			delete [] Bits;
			Bits = (sbitset_block*)malloc(NNBlocks * bytes_per_block);
			NBlocks = NNBlocks;
		}
	}

	void Set(size_t Pos, bool Value) {
		if(Pos >= 0) {
			if(Pos >= NBlocks * bits_per_block) { Resize(Pos+1); }
			if(Value != Get(Pos)) {
				this->Bits[Pos/bits_per_block] ^=
                1<<(bits_per_block - (Pos%bits_per_block)-1);
      }
		}
	}


	TSBitSet& operator&=(TSBitSet &bs){
		size_t WBlokcs = NBlocks <= bs.NBlocks ? NBlocks : bs.NBlocks;
		for(size_t i = 0; i < WBlokcs; i++) {
			Bits[i] &= bs.Bits[i];
    }
		for(size_t i = WBlokcs; i< NBlocks; i++) {
			Bits[i] = 0;
    }
		return *this;
	}

	TSBitSet& operator|=(TSBitSet &bs){
		size_t WBlokcs = NBlocks <= bs.NBlocks ? NBlocks : bs.NBlocks;
		for(size_t i = 0; i < WBlokcs; i++) {
			Bits[i] |= bs.Bits[i];
    }
		if(bs.NBlocks > NBlocks) {
			Resize(bs.NBlocks * bits_per_block);
			for(size_t i = WBlokcs; i<NBlocks; i++) {
				Bits[i] = bs.Bits[i];
      }
		}
		return *this;
	}

	TSBitSet& operator =(TSBitSet &b){
		delete [] Bits;
		NBlocks = b.NBlocks;
		Bits = (sbitset_block*)malloc(NBlocks * bytes_per_block);
		memcpy(Bits, b.Bits, NBlocks * bytes_per_block);
		return *this;
	}

	bool operator!=(TSBitSet &b){
		size_t wblokcs = NBlocks <= b.NBlocks ? NBlocks : b.NBlocks;
		for(size_t i = 0; i<wblokcs; i++) {
			if(Bits[i] != b.Bits[i]) { return true;}
		}
    
		if(wblokcs > NBlocks) {
			for(size_t i = wblokcs; i<b.NBlocks; i++) {
				if(b.Bits[i] != 0){ return true; }
			}
		} else {
			for(size_t i = wblokcs; i<NBlocks; i++) {
				if(Bits[i] != 0){ return true; }
			}
		}
		return false;
	}

	bool EmptyAND(TSBitSet &b){
		size_t WBlokcs = NBlocks <= b.NBlocks ? NBlocks : b.NBlocks;
		for(size_t i = 0; i < WBlokcs; i++) {
			if((Bits[i] & b.Bits[i]) != 0) { return false; }
		}
		return true;
	}

	void SetAll(size_t Size, bool Value){
		ResizeAsis(Size);
		sbitset_block svalue = 0x0;
		sbitset_block mask = Value;
		for(size_t i=0; i < bits_per_block; i++) {
			svalue |= mask;
			mask = mask << 1;
		}
		for(size_t i=0; i<NBlocks; i++) {
			Bits[i] = svalue;
		}
	}


	/* ============================================================
	 * Iterators
	 * ============================================================
	 */
	class TIterator{
		TSBitSet &Sb;
		u_size_t WBlock;
		unsigned short Shift;
		public:
			u_size_t First;	//position
			bool Second;	//value
		public:
			TIterator(TSBitSet &sb) : Sb(sb) {
				WBlock = 0;
				Shift = bits_per_block - 1;
				GetComps();
			}
			TIterator(TSBitSet &sb, u_size_t wblock) : Sb(sb), WBlock(wblock) {
				Shift = bits_per_block - 1;
				GetComps();
			}

		private:
			void GetComps() {
				First = (WBlock * bits_per_block) + bits_per_block - Shift - 1;
				if(WBlock < Sb.NBlocks) {
					Second =  Sb.Bits[WBlock] & 1<<(Shift);
				} else {
					Second = false;
				}
			}
		public:
			TIterator& operator++() {
				if(Shift == 0) {
					WBlock++;
					Shift = bits_per_block - 1;
				} else {
					Shift--;
				}

				GetComps();
				return (*this);
			}
      
			void operator++ (int) {
			   ++(*this);
			}
      
			void next_ones() {
				do {
					if(Shift == 0) {
						WBlock++;
						Shift = bits_per_block - 1;
						while(WBlock < Sb.NBlocks && Sb.Bits[WBlock] == 0)
							WBlock++;
					} else {
						Shift--;
					}
					GetComps();
				} while(!Second && WBlock<Sb.NBlocks);
			}

			const bool operator==(const TIterator &IT) {
				return &Sb == &(IT.Sb) && WBlock == IT.WBlock && Shift == IT.Shift;
			}
			const bool operator!=(const TIterator &IT) {
				if( &Sb != &(IT.Sb) || WBlock != IT.WBlock || Shift != IT.Shift) {
					return true;
        }
				return false;
			}
	};

	TIterator Begin() {
		if(IsEmpty()) { return End(); }
		return TIterator(*this);
	}

	TIterator End() {
		return TIterator(*this, NBlocks);
	}

	TIterator FirstOnes() {
		if(IsEmpty()) { return End(); }
    
		TIterator IT(*this);
		if(IT.Second != 1) { IT.next_ones(); }
    
		return IT;
	}
};

template<class TNodeData, class TEdgeData>
class TMatchingMachine {
  
public:
  //#//////////////////////////////////////////////
  ///
  class TMaMaEdge {
  private:
    int Source;
    int Target;
    TEdgeData* Attr;
    int Id;
  public:
    int GetSource() const { return Source; }
    int GetTarget() const { return Target; }
    TEdgeData* GetAttr() const { return Attr; }
    int GetId() const { return Id; }
    
    void SetSource(const int ValSource){ Source = ValSource; }
    void SetTarget(const int ValTarget){ Target = ValTarget; }
    void SetAttr(TEdgeData* ValAttr) { Attr = ValAttr; }
    void SetId(int Id) { this->Id = Id; }
    
    TMaMaEdge(int NSource, int NTarget, TEdgeData* NAttr, int NId) {
      Source = NSource;
      Target = NTarget;
      Attr = NAttr;
      Id = NId;
    }
    
    TMaMaEdge() {
      Source = -1;
      Target = -1;
      Attr = NULL;
      Id = -1;
    }
  };
  
protected:
  int NofSn;
  TNodeData** NodesAttrs;
  int* EdgesSizes;
  int* OutEdgesSizes;
	int* InEdgesSizes;
  TMaMaEdge** Edges;
  int* MapNodeToState;
  int* MapStateToNode;
  int* ParentState;
  TMamaParent* ParentType;
  TSBitSet* Domains;
	int* DomainsSize;
  
private:
  
  int WCompare(int i, int j, int** Weights) {
    for (int w = 0; w < 3; w++) {
      if (Weights[i][w] != Weights[j][w]) {
        return Weights[j][w] - Weights[i][w];
      }
    }
    return i-j;
  }
  
public:
  
  TMatchingMachine(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Query,
    TSBitSet* NDomains, int* NDomainsSize) {
      NofSn = Query->GetNodes();
      NodesAttrs = new TNodeData*[NofSn];
      EdgesSizes = (int*) calloc(NofSn, sizeof(int));
      OutEdgesSizes = (int*) calloc(NofSn, sizeof(int));
		  InEdgesSizes = (int*) calloc(NofSn, sizeof(int));
      Edges = new TMaMaEdge*[NofSn];
      MapNodeToState = (int*) calloc(NofSn, sizeof(int));
      MapStateToNode = (int*) calloc(NofSn, sizeof(int));
      ParentState = (int*) calloc(NofSn, sizeof(int));
      ParentType = new TMamaParent[NofSn];
      Domains = NDomains;
      DomainsSize = NDomainsSize;
      
  }
  /// ##
  virtual ~TMatchingMachine() {
    delete[] NodesAttrs;
    for (int i = 0; i < NofSn; i++) {
      delete[] Edges[i];
    }
    delete[] Edges;
    free(EdgesSizes);
    free(OutEdgesSizes);
		free(InEdgesSizes);
    free(MapNodeToState);
    free(MapStateToNode);
    free(ParentState);
    delete[] ParentType;
  }
  /// ##
  void Build(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &SSG) {
    enum TNodeFlag { nsCore, nsCNeigh, nsUnv };
    TNodeFlag* NodeFlags = new TNodeFlag[NofSn];
    int** Weights = new int*[NofSn];
    int* ParentNode = (int*) calloc(NofSn, sizeof(int));
    TMamaParent* ParentTypes = new TMamaParent[NofSn];
    int NofSingleDomains = 0;

    for (int i = 0; i < NofSn; i++) {
      NodeFlags[i] = nsUnv;
      Weights[i] = new int[3];
      Weights[i][0] = 0;
      Weights[i][1] = 0;
      Weights[i][2] = GetOutAdjSize(SSG, i) + GetInAdjSize(SSG, i);
      ParentNode[i] = -1;
      ParentTypes[i] = mpParentTypeNull;
      
      if(DomainsSize[i] == 1) { NofSingleDomains++; }
    }

    int SId = 0;
    int n;
    int NIT; int NId;
    int NNIT; int NNId;
    int NQueueL = 0, NQueueR = 0;
    int MaxId, MaxV;
    int Tmp;
    
    if(NofSingleDomains != 0) {
      NQueueR = NofSingleDomains;
      for(int n = 0; n < NofSn; n++) {
				if(DomainsSize[n] == 1) {

					MapStateToNode[SId] = n;
					MapNodeToState[n] = SId;
					ParentTypes[n] = mpParentTypeNull;
					ParentNode[n] = -1;

					NQueueL++;
					//update nodes' flags & Weights
					NodeFlags[n] = nsCore;
					NIT = 0;
          
					while(NIT < GetOutAdjSize(SSG, n)){
						NId = GetNthOutAdjList(SSG, n, NIT);
						if(NId != n && DomainsSize[NId] > 1){
							Weights[NId][0]++;
							Weights[NId][1]--;

							if(NodeFlags[NId] == nsUnv) {
								NodeFlags[NId] = nsCNeigh;
								ParentNode[NId] = n;
								if(NIT < GetOutAdjSize(SSG, n)) {
									ParentTypes[NId] = mpParentTypeOut;
								} else {
									ParentTypes[NId] = mpParentTypeIn;
                }
								//add to queue
								MapStateToNode[NQueueL] = NId;
								MapNodeToState[NId] = NQueueL;
								NQueueR++;

								NNIT = 0;
								while(NNIT < GetOutAdjSize(SSG, NId)){
									NNId = GetNthOutAdjList(SSG, NId, NNIT);
									Weights[NNId][1]++;
									NNIT++;
								}
							}
						}
						NIT++;
					}
          
          NIT = 0;
					while(NIT < GetInAdjSize(SSG, n)){
						NId = GetNthInAdjList(SSG, n, NIT);
						if(NId != n && DomainsSize[NId] > 1){
							Weights[NId][0]++;
							Weights[NId][1]--;

							if(NodeFlags[NId] == nsUnv) {
								NodeFlags[NId] = nsCNeigh;
								ParentNode[NId] = n;
								if(NIT < GetOutAdjSize(SSG, n)) {
									ParentTypes[NId] = mpParentTypeOut;
								} else {
									ParentTypes[NId] = mpParentTypeIn;
                }
								//add to queue
								MapStateToNode[NQueueR] = NId;
								MapNodeToState[NId] = NQueueR;
								NQueueR++;

								NNIT = 0;
								while(NNIT < GetInAdjSize(SSG, NId)){
									NNId = GetNthInAdjList(SSG, NId, NNIT);
									Weights[NNId][1]++;
									NNIT++;
								}
							}
						}
						NIT++;
					}
					SId++;
				}
			}
    }
    
    while(SId < NofSn) {
			if(NQueueL == NQueueR) {
				//if queue is empty....
				MaxId = -1;
				MaxV = -1;
				NIT = 0;
				while(NIT < NofSn) {
					if(NodeFlags[NIT] == nsUnv && Weights[NIT][2] > MaxV) {
						MaxV = Weights[NIT][2];
						MaxId = NIT;
					}
					NIT++;
				}
        
				MapStateToNode[SId] = MaxId;
				MapNodeToState[MaxId] = SId;
				ParentTypes[MaxId] = mpParentTypeNull;
				ParentNode[MaxId] = -1;
				NQueueR++;
				n = MaxId;
				NIT = 0;
        
				while(NIT < GetOutAdjSize(SSG, n)) {
					NId = GetNthOutAdjList(SSG, n, NIT);
					if(NId != n) { Weights[NId][1]++; }
					NIT++;
				}
        while(NIT < GetInAdjSize(SSG, n)) {
					NId = GetNthInAdjList(SSG, n, NIT);
					if(NId != n) { Weights[NId][1]++; }
					NIT++;
				}
			}

			if(NQueueL != NQueueR-1) {
				MaxId = NQueueL;
				for(int mi = MaxId+1; mi < NQueueR; mi++) {
					if(WCompare(MapStateToNode[mi], MapStateToNode[MaxId], Weights) < 0) {
						MaxId = mi;
					}
				}
				Tmp = MapStateToNode[NQueueL];
				MapStateToNode[NQueueL] = MapStateToNode[MaxId];
				MapStateToNode[MaxId] = Tmp;
			}

			n = MapStateToNode[SId];
			MapNodeToState[n] = SId;

			//move queue left limit
			NQueueL++;
			//update nodes' flags & Weights
			NodeFlags[n] = nsCore;
			NIT = 0;
			while(NIT < GetOutAdjSize(SSG, n)) {
				NId = GetNthOutAdjList(SSG, n, NIT);
				if(NId != n) {
					Weights[NId][0]++;
					Weights[NId][1]--;

					if(NodeFlags[NId] == nsUnv) {
						NodeFlags[NId] = nsCNeigh;
						ParentNode[NId] = n;
						if(NIT < GetOutAdjSize(SSG, n)) {
							ParentTypes[NId] = mpParentTypeOut;
            } else {
							ParentTypes[NId] = mpParentTypeIn;
            }
						//add to queue
						MapStateToNode[NQueueR] = NId;
						MapNodeToState[NId] = NQueueR;
						NQueueR++;

						NNIT = 0;
						while(NNIT < GetOutAdjSize(SSG, NId)) {
							NNId = GetNthOutAdjList(SSG, NId, NNIT);
							Weights[NNId][1]++;
							NNIT++;
						}
					}
				}
				NIT++;
			}
      
      NIT = 0;
			while(NIT < GetInAdjSize(SSG, n)) {
				NId = GetNthInAdjList(SSG, n, NIT);
				if(NId != n) {
					Weights[NId][0]++;
					Weights[NId][1]--;

					if(NodeFlags[NId] == nsUnv) {
						NodeFlags[NId] = nsCNeigh;
						ParentNode[NId] = n;
						if(NIT < GetOutAdjSize(SSG, n)) {
							ParentTypes[NId] = mpParentTypeOut;
            } else {
							ParentTypes[NId] = mpParentTypeIn;
            }
						//add to queue
						MapStateToNode[NQueueR] = NId;
						MapNodeToState[NId] = NQueueR;
						NQueueR++;

						NNIT = 0;
						while(NNIT < GetInAdjSize(SSG, NId)) {
							NNId = GetNthInAdjList(SSG, NId, NNIT);
							Weights[NNId][1]++;
							NNIT++;
						}
					}
				}
				NIT++;
			}
			SId++;
		}
    
    int ECount, OutECount, InECount; int i;
    for (SId = 0; SId < NofSn; SId++) {
      n = MapStateToNode[SId];

      NodesAttrs[SId] = &SSG->GetNI(n).GetDat();
      if (ParentNode[n] != -1) {
        ParentState[SId] = MapNodeToState[ParentNode[n]];
      } else {
        ParentState[SId] = -1;
      }
      ParentType[SId] = ParentTypes[n];
      
      ECount = 0;
      OutECount = 0;
      for (i = 0; i < GetOutAdjSize(SSG, n); i++) {
        if (MapNodeToState[GetNthOutAdjList(SSG, n, i)] < SId) {
            ECount++;
            OutECount++;
        }
      }
      InECount = 0;
      for (i = 0; i < GetInAdjSize(SSG, n); i++) {
        if (MapNodeToState[GetNthInAdjList(SSG, n, i)] < SId) {
            ECount++;
            InECount++;
        }
      }
      
      EdgesSizes[SId] = ECount;
      OutEdgesSizes[SId] = OutECount;
			InEdgesSizes[SId] = InECount;
      Edges[SId] = new TMaMaEdge[ECount];
      ECount = 0;
      
      for (i = 0; i < GetOutAdjSize(SSG, n); i++) {
        int m = GetNthOutAdjList(SSG, n, i);
        if (MapNodeToState[m] < SId) {
          Edges[SId][ECount].SetSource(MapNodeToState[n]);
          Edges[SId][ECount].SetTarget(MapNodeToState[m]);
          Edges[SId][ECount].SetAttr(&SSG->GetEI(n, m).GetDat());
          //Edges[SId][ECount].SetId(GetEdgeId(SSG, n, i));
          ECount++;
        }
      }
      for (i = 0; i < GetInAdjSize(SSG, n);i++) {
        int m = GetNthInAdjList(SSG, n, i);
        if (MapNodeToState[m] < SId) {

          Edges[SId][ECount].SetTarget(MapNodeToState[n]);
          Edges[SId][ECount].SetSource(MapNodeToState[m]);
          Edges[SId][ECount].SetAttr(&SSG->GetEI(m, n).GetDat());
          //Edges[SId][ECount].SetId(GetEdgeId(SSG, n, i));
          ECount++;
        }
      }
    }

    delete[] NodeFlags;
    for (int i = 0; i < NofSn; i++) {
      delete[] Weights[i];
    }
    delete[] Weights;
    free(ParentNode);
    delete[] ParentTypes;

  }
  
  void print() {
    printf("| MatchingMachine:  nof states %d\n", NofSn);
    printf("| \tmap state_to_node(");
    
    for (int i = 0; i < NofSn; i++) {
      printf("[%d: %d]", i, MapStateToNode[i]);
    }
    printf(")\n");
    printf("| \tmap node_to_state(");
    for (int i = 0; i < NofSn; i++) {
      printf("[: %d]", i, MapNodeToState[i]);
    }
    printf(")\n");
    printf("| \tstates (node)(parent state, parent type)\n");
    for (int i = 0; i < NofSn; i++) {
      printf("| \t\t[%d] (%d) (%d, ", i, MapStateToNode[i], ParentState[i]);
      switch (ParentType[i]) {
        case mpParentTypeIn:
          printf("IN");
          break;
        case mpParentTypeOut:
          printf("OUT");
          break;
        case mpParentTypeNull:
          printf("NULL");
          break;
      }
      printf(")\n");
      printf("| \t\t\tchecking[%d] ", EdgesSizes[i]);
      for (int j = 0; j < EdgesSizes[i]; j++) {
        printf("{s(%d,%d):", Edges[i][j].GetSource(), Edges[i][j].GetTarget());
        printf("n(%d,%d)}", MapStateToNode[Edges[i][j].GetSource()],
          MapStateToNode[Edges[i][j].GetTarget()]);
      }
      printf("\n");
    }
  }
  
  TMaMaEdge** GetEdges() const { return Edges; }
  int* GetEdgesSizes() const { return EdgesSizes; }
  int* GetOutEdgesSizes() const { return OutEdgesSizes; }
  int* GetInEdgesSizes() const { return InEdgesSizes; }
  int* GetMapNodeToState() const { return MapNodeToState; }
  int* GetMapStateToNode() const { return MapStateToNode; }
  TNodeData** GetNodesAttrs() const { return NodesAttrs; }
  int GetNofSn() const { return NofSn; }
  int* GetParentState() const { return ParentState;}
  TMamaParent* GetParentType() const {
    return ParentType;
  }
  
  void SetEdges(TMaMaEdge** edges) { this->Edges = edges; }
  void SetEdgesSizes(int* edges_sizes) { this->EdgesSizes = edges_sizes; }
  void SetMapNodeToState(int* MapNodeToState) {
    this->MapNodeToState = MapNodeToState;
  }
  
  void SetMapStateToNode(int* MapStateToNode) {
    this->MapStateToNode = MapStateToNode;
  }
  
  void SetNodesAttrs(TNodeData** nodes_attrs) {
    this->NodesAttrs = nodes_attrs;
  }
  
  void SetNofSn(int nof_sn) { this->NofSn = nof_sn; }
  void SetParentState(int* parent_state) {
    this->ParentState = parent_state;
  }
  void SetParentType(TMamaParent* parent_type) {
    this->ParentType = parent_type;
  }
};

//#//////////////////////////////////////////////
///
class TMatchListener {
protected:
  long MatchCount;
public:
  TMatchListener() { MatchCount = 0; }
  virtual ~TMatchListener() { };
  /// ##
  long GetMatchCount() const { return MatchCount; }
  /// ##
  void SetMatchCount(long NMatchCount) { MatchCount = NMatchCount; }
  /// ##
  virtual void Match(int n, int* QIds, int* RIds) = 0;
};

//#//////////////////////////////////////////////
///
class TEmptyMatchListener : public TMatchListener {
public:
  /// ##
  TEmptyMatchListener() : TMatchListener() { }
  /// ##
  virtual void Match(int n, int* QIds, int* RIds) {
    MatchCount++;
  };
};

//#//////////////////////////////////////////////
///
class TConsoleMatchListener : public TMatchListener {
public:
  /// ##
  TConsoleMatchListener() : TMatchListener() { }
  /// ##
  virtual void Match(int n, int* QIds, int* RIds) {
    MatchCount++;
    printf("{");
    for (int i = 0; i < n; i++) {
      printf("(%d,%d)", QIds[i], RIds[i]);
    }
    printf("}\n");
  }
};

template<class TNodeData, class TEdgeData>
class TSolver {
protected:
  TMatchingMachine<TNodeData, TEdgeData> &MaMa;
  TPt<TNodeEDatNet<TNodeData, TEdgeData> > &RGraph;
  TPt<TNodeEDatNet<TNodeData, TEdgeData> > &QGraph;
  TMatchListener &MatchListener;
  TSBitSet* Domains;
  int* DomainsSize;
private:
  //long Steps;
  //long TriedCouples;
  //long MatchedCouples;
public:
  
  TSolver(TMatchingMachine<TNodeData, TEdgeData> &NMaMa,
    TPt<TNodeEDatNet<TNodeData, TEdgeData> > &NRGraph,
    TPt<TNodeEDatNet<TNodeData, TEdgeData> > &NQGraph,
    TMatchListener &NMatchListener, TSBitSet* NDomains, int* NDomainsSize ) :
      MaMa(NMaMa), RGraph(NRGraph), QGraph(NQGraph),
      MatchListener(NMatchListener), Domains(NDomains),
      DomainsSize(NDomainsSize) {
    //Steps = 0;
    //TriedCouples = 0;
    //MatchedCouples = 0;
  }
      
  virtual ~TSolver() { }
  
  //long GetSteps() const { return Steps; }
  //long GetTriedCouples() const { return TriedCouples; }
  //long GetMatchedCouples() const { return MatchedCouples; }
  
  void Solve() {
    int IId;
    int NofSn = MaMa.GetNofSn();
    //TNodeData** NodesAttrs = MaMa.GetNodesAttrs();
    //int* EdgesSizes = MaMa.GetEdgesSizes();
    //typename TMatchingMachine<TNodeData, TEdgeData>::TMaMaEdge** Edges = MaMa.GetEdges();
    //int* MapNodeToState = MaMa.GetMapNodeToState();
    int* MapStateToNode = MaMa.GetMapStateToNode();
    int* ParentState = MaMa.GetParentState();
    TMamaParent* ParentType = MaMa.GetParentType();

    TNodeList** Candidates = new TNodeList*[NofSn];
    int* CandidatesIt = new int[NofSn];
    int* CandidatesSize = new int[NofSn];
    int* Solution = new int[NofSn];

    for (IId = 0; IId < NofSn; IId++) {
      Solution[IId] = -1;
    }

    bool* Matched = (bool*) calloc(RGraph->GetNodes(), sizeof(bool));
    for(int i=0; i < NofSn; i++){
			if(ParentType[i] == mpParentTypeNull){
				int n = MapStateToNode[i];
        Candidates[i] = new TCandidatesList<TNodeData, TEdgeData>(RGraph,
          DomainsSize[n]);

				for(TSBitSet::TIterator IT = Domains[n].FirstOnes();
                IT != Domains[n].End(); IT.next_ones()){
					Candidates[i]->AddNode(IT.First);
				}

				CandidatesSize[i] = DomainsSize[n];
				CandidatesIt[i] = -1;
			}
		}

    int PSId = -1;
    int SId = 0;
    int CId = -1;
    int Sip1;
    
    while (SId != -1) {
      if (PSId >= SId) { Matched[Solution[SId]] = false; }

      CId = -1;
      CandidatesIt[SId]++;

      while (CandidatesIt[SId] < CandidatesSize[SId]) {
        CId = Candidates[SId]->GetNode(CandidatesIt[SId]);
        Solution[SId] = CId;

        if ((!Matched[CId]) &&
             Domains[MapStateToNode[SId]].Get(CId) &&
             EdgesCheck(SId, CId, Solution, Matched)) {
          break;
        } else {
          CId = -1;
        }
        CandidatesIt[SId]++;
      }
      
      if (CId == -1) {
        PSId = SId;
        delete Candidates[SId];
        SId--;
      } else {
        //MatchedCouples++;

        if (SId == NofSn -1) {
          MatchListener.Match(NofSn, MapStateToNode, Solution);
          PSId = SId;
        } else {
          Matched[Solution[SId]] = true;
          Sip1 = SId + 1;
          if (ParentType[Sip1] == mpParentTypeNull) {
          } else {
            if (ParentType[Sip1] == mpParentTypeIn) {
              Candidates[Sip1] = new TInAdjList<TNodeData, TEdgeData>(RGraph,
                Solution[ParentState[Sip1]]);
              CandidatesSize[Sip1] = GetInAdjSize(RGraph,
                Solution[ParentState[Sip1]]);
            } else {//(parent_type[sip1] == MAMA_PARENTTYPE::mpParentTypeOut)
              Candidates[Sip1] = new TOutAdjList<TNodeData, TEdgeData>(RGraph,
                Solution[ParentState[Sip1]]);
              CandidatesSize[Sip1] = GetOutAdjSize(RGraph,
                Solution[ParentState[Sip1]]);
            }
          }
          CandidatesIt[SId + 1] = -1;
          PSId = SId;
          SId++;
        }
      }
    }
  }
  
  virtual bool EdgesCheck(int SId, int CId, int* Solution, bool* Matched)=0;
};

//#//////////////////////////////////////////////
///
template<class TNodeData, class TEdgeData>
class TInducedSubGISolver : public TSolver<TNodeData, TEdgeData> {
using TSolver<TNodeData, TEdgeData>::RGraph;
using TSolver<TNodeData, TEdgeData>::QGraph;
using TSolver<TNodeData, TEdgeData>::MaMa;

public:
  
  TInducedSubGISolver(TMatchingMachine<TNodeData, TEdgeData> &NMaMa,
    TPt<TNodeEDatNet<TNodeData, TEdgeData> > &NRGraph,
    TPt<TNodeEDatNet<TNodeData, TEdgeData> > &NQGraph,
    TMatchListener &NMatchListener,
    TSBitSet* NDomains,
    int* NDomainsSize) : TSolver<TNodeData, TEdgeData>(NMaMa,
      NRGraph, NQGraph, NMatchListener, NDomains, NDomainsSize) {
  }
  
  virtual bool EdgesCheck(int SId, int CId, int* Solution, bool* Matched) {
    int RSource, RTarget, Source, Target, EId;
    int IId;
    
    for (int me = 0; me < MaMa.GetEdgesSizes()[SId]; me++) {
      Source = MaMa.GetEdges()[SId][me].GetSource();
      Target = MaMa.GetEdges()[SId][me].GetTarget();
      RSource = Solution[Source];
			RTarget = Solution[Target];
			EId = Solution[MaMa.GetEdges()[SId][me].GetId()];

      for (IId = 0; IId < GetOutAdjSize(RGraph, RSource); IId++) {
        if (GetNthOutAdjList(RGraph, RSource, IId) == RTarget) {
          if (RGraph->GetEI(RSource, GetNthOutAdjList(RGraph, RSource, IId)).GetDat() != 
               *MaMa.GetEdges()[SId][me].GetAttr()) {
            return false;
          } else {
            break;
          }
        } else if(GetNthOutAdjList(RGraph, RSource, IId) > RTarget) {
            return false;
        }
      }
    }
    
    int Count = 0;
    for (IId = 0; IId < GetOutAdjSize(RGraph, CId); IId++) {
      if (Matched[GetNthOutAdjList(RGraph, CId, IId)]) {
        Count++;
        if (Count > MaMa.GetOutEdgesSizes()[SId])
          return false;
      }
    }
    Count = 0;
    for (IId = 0; IId < GetInAdjSize(RGraph, CId); IId++) {
      if (Matched[GetNthInAdjList(RGraph, CId, IId)]) {
        Count++;
        if (Count > MaMa.GetInEdgesSizes()[SId])
          return false;
      }
    }
    return true;
  }
};

//#//////////////////////////////////////////////
///
template <class TNodeData, class TEdgeData>
class SubGISolver : public TSolver<TNodeData, TEdgeData> {
using TSolver<TNodeData, TEdgeData>::RGraph;
using TSolver<TNodeData, TEdgeData>::QGraph;
using TSolver<TNodeData, TEdgeData>::MaMa;
public:
  
  SubGISolver(TMatchingMachine<TNodeData, TEdgeData> &NMaMa,
    TPt<TNodeEDatNet<TNodeData, TEdgeData> > &NRGraph,
    TPt<TNodeEDatNet<TNodeData, TEdgeData> > &NQGraph,
    TMatchListener &NMatchListener,
    TSBitSet* NDomains,
    int* NDomainsSize) : TSolver<TNodeData, TEdgeData>(NMaMa,
      NRGraph, NQGraph, NMatchListener, NDomains, NDomainsSize) {
  }
  
  virtual bool EdgesCheck(int SId, int CId, int* Solution, bool* Matched) {
    int RSource, RTarget, Source, Target, EId;
    int IId;
    for (int me = 0; me < MaMa.GetEdgesSizes()[SId]; me++) {
      Source = MaMa.GetEdges()[SId][me].GetSource();
      Target = MaMa.GetEdges()[SId][me].GetTarget();
      RSource = Solution[Source];
			RTarget = Solution[Target];
      EId = Solution[MaMa.GetEdges()[SId][me].GetId()];

      for (IId = 0; IId < GetOutAdjSize(RGraph, RSource); IId++) {
        if (GetNthOutAdjList(RGraph, RSource, IId) == RTarget) {
          if (RGraph->GetEI(RSource, GetNthOutAdjList(RGraph, RSource, IId)).GetDat() !=
               *MaMa.GetEdges()[SId][me].GetAttr()) {
            return false;
          } else {
            break;
          }
        } else if(GetNthOutAdjList(RGraph, RSource, IId) > RTarget){
					return false;
				}
      }
      if (IId >= GetOutAdjSize(RGraph, RSource)) {
        return false;
      }
    }
    return true;
  }
};

template <class TNodeData, class TEdgeData>
bool InitDomains(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Target,
  TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Pattern,
	TSBitSet* Domains,
	bool Iso) {

	if(Iso) {
		for(int q = 0; q < Pattern->GetNodes(); q++){
			for(int r = Target->GetNodes() - 1; r >= 0; r--){
				if(GetOutAdjSize(Target, r) == GetOutAdjSize(Pattern, q) &&
          GetInAdjSize(Target, r) == GetInAdjSize(Pattern, q) &&
          Target->GetNI(r).GetDat() == Pattern->GetNI(q).GetDat()) {
					Domains[q].Set(r, true);
				}
			}
			if(Domains[q].IsEmpty()) { return false; }
		}
	} else {
		for(int q=0; q < Pattern->GetNodes(); q++) {
			for(int r = Target->GetNodes()-1; r >= 0; r--) {
				if(GetOutAdjSize(Target, r) >= GetOutAdjSize(Pattern, q) &&
          GetInAdjSize(Target, r) >= GetInAdjSize(Pattern, q) &&
          Target->GetNI(r).GetDat() == Pattern->GetNI(q).GetDat()) {
					Domains[q].Set(r, true);
				}
			}
			if(Domains[q].IsEmpty()){ return false; }
		}
	}

	int ra, qb, rb;
	bool NotFound;

	//1°-level neighborhood and edges labels
	for(int qa = 0; qa < Pattern->GetNodes(); qa++) {
    for(TSBitSet::TIterator qaIT = Domains[qa].FirstOnes(); qaIT !=
            Domains[qa].End(); qaIT.next_ones()){
			ra = qaIT.First;
			//for each edge qa->qb  check if exists ra->rb
			for(int i_qb = 0; i_qb < GetOutAdjSize(Pattern, qa); i_qb++) {
				qb = GetNodeInAdjList(Pattern, qa, i_qb);
				NotFound = true;

				for(int i_rb=0; i_rb < GetOutAdjSize(Target, ra); i_rb++) {
					rb = GetNthOutAdjList(Target, ra, i_rb);
          
          int m = GetNthOutAdjList(Pattern, qa, i_qb);
          int s = GetNthOutAdjList(Target, ra, i_rb);
					if(Domains[qb].Get(rb) &&
            Pattern->GetEI(qa, m).GetDat() == Target->GetEI(ra, s).GetDat()){
						NotFound = false;
						break;
					}
				}

				if(NotFound) {
					Domains[qa].Set(ra, false);
					break;
				}
			}
		}
		if(Domains[qa].IsEmpty()) { return false; }
	}
	return true;
};

template<class TNodeData, class TEdgeData>
void Match(TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Reference,
  TPt<TNodeEDatNet<TNodeData, TEdgeData> > &Query,
  TMatchingMachine<TNodeData, TEdgeData> &MatchingMachine,
  TMatchListener &MatchListener,
  TMatch MatchType,
  //long* Steps,
  //long* TriedCouples,
  //long* MatchedCouples,
  TSBitSet* Domains,
  int* DomainsSize) {
    
    TSolver<TNodeData, TEdgeData>* Solver;
    switch (MatchType) {
      case mIso:
      case mIndSub:
        Solver = new TInducedSubGISolver<TNodeData, TEdgeData>(MatchingMachine,
          Reference,
          Query,
          MatchListener,
          Domains,
          DomainsSize);
        break;
      case mMono:
        Solver = new SubGISolver<TNodeData, TEdgeData>(MatchingMachine,
          Reference,
          Query,
          MatchListener,
          Domains,
          DomainsSize);
        break;
    }

    Solver->Solve();
    //*Steps = Solver->GetSteps();
    //*TriedCouples = Solver->GetTriedCouples();
    //*MatchedCouples = Solver->GetMatchedCouples();
    
    delete Solver;
};

template <class TNodeData, class TEdgeData>
int Match(TMatch MatchType, TPt<TNodeEDatNet<TNodeData, TEdgeData> > &RefGraph,
  TPt<TNodeEDatNet<TNodeData, TEdgeData> > &QueGraph);

template <class TNodeData, class TEdgeData>
int Match(TMatch MatchType,
  TPt<TNodeEDatNet<TNodeData, TEdgeData> > &RefGraph,
  TPt<TNodeEDatNet<TNodeData, TEdgeData> > &QueGraph) {

	bool DoBijIso = (MatchType == mIso);

    long MatchCount = 0;
	//long Steps = 0, TriedCouples = 0, MatchCount = 0, MatchedCouples = 0;
	//long TmpSteps = 0, TmpTriedCouples = 0, TmpMatchedCouples = 0;

#ifdef PRINT_MATCHES
		TMatchListener* MatchListener = new TConsoleMatchListener();
#else
		TMatchListener* MatchListener = new TEmptyMatchListener();
#endif
    if(!DoBijIso ||
      (DoBijIso && (QueGraph->GetNodes() == RefGraph->GetNodes()))) {
      
      TSBitSet* Domains = new TSBitSet[QueGraph->GetNodes()];
      bool Domok = InitDomains(RefGraph, QueGraph, Domains, DoBijIso);
      
      if(Domok){
        int* DomainsSize = new int[QueGraph->GetNodes()];
        int dsize;
        for(int ii=0; ii < QueGraph->GetNodes(); ii++){
          dsize = 0;
          for(TSBitSet::TIterator IT = Domains[ii].FirstOnes(); IT!=Domains[ii].End(); IT.next_ones()){
            dsize++;
          }
          DomainsSize[ii] = dsize;
        }
        
        TMatchingMachine<TNodeData, TEdgeData>* MaMa =
          new TMatchingMachine<TNodeData, TEdgeData>(QueGraph, Domains, DomainsSize);
        
        MaMa->Build(QueGraph);
        
        Match(RefGraph,
          QueGraph,
          *MaMa,
          *MatchListener,
          MatchType,
          //&TmpSteps,
          //&TmpTriedCouples,
          //&TmpMatchedCouples,
          Domains,
          DomainsSize);
        
        //Steps += TmpSteps;
        //TriedCouples += TmpTriedCouples;
        //MatchedCouples += TmpMatchedCouples;
        
        delete MaMa;
      }
    }

		if(MatchListener != NULL) { MatchCount += MatchListener->GetMatchCount(); }
    
		delete MatchListener;

#ifdef CSV_FORMAT
	std::cout<<referencefile<<"\t"<<queryfile<<"\t";
	std:cout<<load_t_q<<"\t"<<make_mama_t<<"\t"<<load_t<<"\t"<<match_t<<"\t"<<total_t<<"\t"<<Steps<<"\t"<<TriedCouples<<"\t"<<MatchedCouples<<"\t"<<MatchCount;
#else
	printf("number of found matches: %ld\n", MatchCount);
    //printf("search space size: %ld\n", MatchedCouples);
#endif

	return 0;
};

#endif	/* RI3_H */


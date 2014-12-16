/////////////////////////////////////////////////
// Community detection algorithms
namespace TSnap {


  namespace TSnapDetail {

  } // namespace TSnapDetail



  int FastCorePeriphery(PUNGraph& Graph, TIntIntH& out){

    TIntIntH nodes;
    double Z=0;

    for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++){ // Calculate and store the degrees of each node.
      int deg = NI.GetDeg();
      int id = NI.GetId();
      Z += deg;
      nodes.AddDat(id,deg);
    }

    Z = Z/2;

    nodes.SortByDat(false); // Then sort the nodes in descending order of degree, to get a list of nodes {v1, v2, . . . , vn}.

    double Zbest = 99999900000000000;
    int kbest = 0;
    
    int br=0;
    for (int k=0; k<nodes.Len(); k++){
      br++;
      Z = Z + br - 1 - nodes[k];
      if (Z < Zbest){ // or <=
        Zbest = Z;
        kbest = br;
      }
    }

    int cp = 0;
    br = 0;
    for (THashKeyDatI<TInt, TInt> it = nodes.BegI();  !it.IsEnd(); it++) {
      if (br < kbest)
        cp = 1;
      else
        cp = 0;
      out.AddDat(it.GetKey(), cp);
      br++;
    }

    return kbest;
  }


  int FastCorePeripheryGC(PUNGraph& Graph, TIntIntH& out){
    TIntH GroupNodes; // buildup cpntainer of group nodes
    int *NNodes = new int[Graph->GetNodes()]; // container of neighbouring nodes
    int NNodes_br = 0;

    TIntIntH nodes;
    TIntIntH nodesIds;
    double Z=0;

    for (TUNGraph::TNodeI NI = Graph->BegNI(); NI < Graph->EndNI(); NI++){ // Calculate and store the degrees of each node.
      int deg = NI.GetDeg();
      int id = NI.GetId();
      Z += deg;
      nodes.AddDat(id,deg);
      
    }

    Z = Z/2;

    nodes.SortByDat(false); // Then sort the nodes in descending order of degree, to get a list of nodes {v1, v2, . . . , vn}.

    int br1=0;
    for (THashKeyDatI<TInt,TInt> NI = nodes.BegI(); NI < nodes.EndI(); NI++){
      nodesIds.AddDat(NI.GetKey(),NI.GetKey());
      br1++;
    }

    double Zbest = 99999900000000000;
    //int kbest;
    //int olddeg;
    int br=0;
    for (int k=0; k<nodes.Len(); k++){
      if (k<nodes.Len()-1){
        if (nodes[k]==nodes[k+1]){ // go into same deg mode
          int kmin=-2; int knew=-1;
          while (kmin < 999999 && kmin !=-1 ){
            int kind=-1;
            knew=k;
            kmin=999999;
            while(nodes[k]==nodes[knew] && knew < nodes.Len()-1){
              int inter = Intersect(Graph->GetNI(nodesIds[knew]),NNodes,NNodes_br);
              int deg = nodes[knew];
              //if (((((nodes.Len()-NNodes_br)*(nodes.Len()-NNodes_br)))-(nodes.Len()-NNodes_br))/2<(((br*br)-br)/2))
              if ((deg-inter)<kmin && !GroupNodes.IsKey(nodesIds[knew]))
              { 
                kmin = deg-inter; kind = knew;
              }
              
              knew++;
            }
            
            if (kind!=-1){
              br++;
              Z = Z + br - 1 - nodes[kind];
              if (Z < (Zbest)){ // or <=
                //if (olddeg>nodes[kind])
                  
                //olddeg = nodes[kind];
                Zbest = Z;
                //kbest = br;
                int w = nodes[kind];
                int id = nodesIds[kind];
                GroupNodes.AddDat(id,w);
                NNodes[NNodes_br] = id;
                NNodes_br++;
              }
              else{
                
                break;
              }
            }
          }
          k=knew-1;
        }
        else{
          br++;
          Z = Z + br - 1 - nodes[k];
          if (Z < (Zbest)){ // or <=
            //if (olddeg>nodes[k])
              
            //olddeg = nodes[k];
            Zbest = Z;
            //kbest = br;
            int w = nodes[k];
            int id = nodesIds[k];
            GroupNodes.AddDat(id,w);
            NNodes[NNodes_br] = id;
            NNodes_br++;
          }
        }
      }
      
      else{
        br++;
        Z = Z + br - 1 - nodes[k];
        if (Z < Zbest){ // or <=
          //if (olddeg>nodes[k])
            
          //olddeg = nodes[k];
          Zbest = Z;
          //kbest = br;
          int w = nodes[k];
          int id = nodesIds[k];
          GroupNodes.AddDat(id,w);
          NNodes[NNodes_br] = id;
          NNodes_br++;
        }
      }
    }

    int cp = 0;
    br = 0;
    for (THashKeyDatI<TInt, TInt> it = nodes.BegI();  !it.IsEnd(); it++) {
      if (GroupNodes.IsKey(it.GetKey()))
        cp = 1;
      else
        cp = 0;
      out.AddDat(it.GetKey(), cp);
      br++;
    }

    /*for (THashKeyDatI<TInt, TInt> it = GroupNodes.BegI();  it < GroupNodes.EndI(); it++) {
      out.AddDat(it.GetKey(), 1);
      br++;
    }*/

    //return kbest;
    return GroupNodes.Len();
  }

  double BorgattiEverettMeasure(PUNGraph& Graph, TIntIntH& out, double coresize, int type){
    
    double sum = 0.0;
    for (TUNGraph::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++){ // Calculate and store the degrees of each node.
      int i = EI.GetSrcNId();
      int j = EI.GetDstNId();
      if (type == 1) {
        if (out.GetDat(i) == 1 || out.GetDat(j) == 1)
          sum += 1;
      }
      else {
        if (out.GetDat(i) == 1 && out.GetDat(j) == 1)
          sum += 1;
      }
    }

    return sum/(((coresize*coresize)-coresize)/2);
  }

  double PearsonCorrelation(PUNGraph& Graph, TIntIntH& out, int coresize){
    int br_core1=0,br_periphery1=0,br_core_per1=0;
    for (TUNGraph::TEdgeI EI = Graph->BegEI(); EI < Graph->EndEI(); EI++){ // Calculate and store the degrees of each node.
      int i = EI.GetSrcNId();
      int j = EI.GetDstNId();
      
      if (out.GetDat(i)==1&&out.GetDat(j)==1 && i!=j)
        br_core1++;
      else if (out.GetDat(i)==0&&out.GetDat(j)==0 && i!=j)
        br_periphery1++;
      else
        br_core_per1++;
    }

    double core_quality = (double)br_core1/((((double)coresize*(double)coresize)-(double)coresize)/2);
    int per_size = Graph->GetNodes()-coresize;
    double periphery_quality = (((((double)per_size*(double)per_size)-(double)per_size)/2) - (double)br_periphery1)/((((double)per_size*(double)per_size)-(double)per_size)/2);

    return (double)(core_quality+periphery_quality);
  }

}; //namespace TSnap

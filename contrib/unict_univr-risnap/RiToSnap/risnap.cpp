/* 
 * File:   main.cpp
 * Author: kengi
 *
 * Created on 4 febbraio 2014, 23.17
 */

#include <iostream>
#include <cstdlib>
#include "Snap.h"
#include "rinetmatch.h"
#include "timer.h"

void usage(char* args0) {
  printf("usage %s [iso ind mono] reference query\n", args0);
};

int main(int argc, char** argv) {
  if (argc != 4) {
    usage(argv[0]);
    return -1;
  }

  TMatch matchtype;

  //check input parameter: match type
  std::string par = argv[1];
  if (par == "iso") {
     matchtype = mIso;
  } else if (par == "ind") {
    matchtype = mInd;
  } else if (par == "mono") {
    matchtype = mMono;
  } else {
    usage(argv[0]);
    return -1;
  }

  //start graph load process and timing
  bool loaded = false;
  
  TIMEHANDLE loadTimeHandle, matchTimeHandle, totalTimeHandle;
  double totalTime=0, loadTime, matchTime;
  totalTimeHandle = start_time();
  
  //load the graph from input parameter
  TPt<TNodeEDatNet<TStr, TStr> > RefGraph, QueGraph;

  TFIn RefLoad(argv[2]);
  TFIn QueLoad(argv[3]);
  
  loadTimeHandle = start_time();
  
  RefGraph = TNodeEDatNet<TStr, TStr>::Load(RefLoad);
  QueGraph = TNodeEDatNet<TStr, TStr>::Load(QueLoad);

  loadTime = end_time(loadTimeHandle);
  
  loaded = true;

  if (loaded) {
    
    matchTimeHandle = start_time();
    
    //start the matching process, call Match of type matchtype
    //between reference graph and query graph
    int retid = Match(matchtype, RefGraph, QueGraph);

    matchTime = end_time(matchTimeHandle);
    totalTime = end_time(totalTimeHandle);
    std::cout<<"total time: "<<totalTime<<"\n";
    std::cout<<"matching time: "<<matchTime<<"\n";
    std::cout<<"load time: "<<loadTime<<"\n";
    
    return retid;
    
  } else {
    std::cout << "unable to open reference file\n";
    return -1;
  }

  return 0;
}


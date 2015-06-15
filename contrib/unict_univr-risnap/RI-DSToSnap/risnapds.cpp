/*
Copyright (c) 2015 by Rosalba Giugno

This library contains portions of other open source products covered by separate
licenses. Please see the corresponding source files for specific terms.

RI is provided under the terms of The MIT License (MIT):

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
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

#include <iostream>
#include <cstdlib>

#include "Snap.h"
#include "rids.h"
#include "timer.h"

void usage(char* args0){
	printf("usage %s [iso mono] reference query\n", args0);
};

int main(int argc, char** argv){

	if(argc != 4){
			usage(argv[0]);
			return -1;
		}

	TMatch matchtype;

  //check input parameter: match type
	std::string par = argv[1];
	if(par=="iso") {
		matchtype = mIso;
	} else if(par=="ind") {
		matchtype = mIndSub;
	} else if(par=="mono") {
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
  
	TPt<TNodeEDatNet<TStr, TStr> > RefGraph, QueGraph;

  //load the graph from input parameter
  TFIn RefLoad(argv[2]);
  TFIn QueLoad(argv[3]);
  
  loadTimeHandle = start_time();
  
  RefGraph = TNodeEDatNet<TStr, TStr>::Load(RefLoad);
  QueGraph = TNodeEDatNet<TStr, TStr>::Load(QueLoad);
  
  loadTime = end_time(loadTimeHandle);
  
  //loadede the graph correctly
  loaded = true;
  
  if(loaded) {
    
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
    printf("unable to open reference file\n");
    return -1;
  }

	return 0;
};

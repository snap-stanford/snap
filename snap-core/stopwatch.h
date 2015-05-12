#ifndef STOPWATCH_H
#define STOPWATCH_H

class TStopwatch {
public:
  /// List of all experiments
  typedef enum {LoadTables, Preprocess, ConstructGraph, Compute, Postprocess, StoreOutputs,
      // graph construction
      AllocateColumnCopies, CopyColumns, Sort, Group, MergeNeighborhoods, AddNeighborhoods, AddEdges, Sort2, ComputeOffset,
      // subgraph construction
      ComputeETypes, EstimateSizes, InitGraph, ExtractNbrETypes, CopyNodes, PopulateGraph, ExtractEdges, BuildSubgraph} TExperiment;
  /// The expected number of experiments (must be at least equal to the size of the above list)
  static const int NEXPS = 25;

  static TStopwatch* GetInstance() {
    static TStopwatch instance; // Guaranteed to be destroyed. Instantiated on first use.
    return &instance;
  }

private:
  TStopwatch() {
    for (int i = 0; i < NEXPS; i++) { Mins[i] = 100000000000.0; }
  }; // Constructor? (the {} brackets) are needed here.

  // Dont forget to declare these two. You want to make sure they
  // are unacceptable otherwise you may accidentally get copies of
  // your singleton appearing.
  TStopwatch(TStopwatch const&);              // Don't Implement
  void operator=(TStopwatch const&); // Don't implement

  double Starts[NEXPS];
  int Cnts[NEXPS];
  double Sums[NEXPS];
  double Maxs[NEXPS];
  double Mins[NEXPS];

private:
  double Tick();

public:
  /// Start a new experiment
  void Start(const TExperiment Exp);
  /// Stop the current experiment
  void Stop(const TExperiment Exp);
  /// Returns the number of experiments
  int Cnt(const TExperiment Exp) const;
  /// Returns the total time of all experiments
  double Sum(const TExperiment Exp) const;
  /// Returns the average time of all experiments
  double Avg(const TExperiment Exp) const;
  /// Returns the maximum time of all experiments
  double Max(const TExperiment Exp) const;
  /// Returns the minimum time of all experiments
  double Min(const TExperiment Exp) const;
};

#endif //STOPWATCH_H


#ifndef STOPWATCH_H
#define STOPWATCH_H

class TStopwatch {
public:
  /// List of all experiments
  typedef enum {LoadTables, Preprocess, Compute, Postprocess, StoreOutputs,
      AllocateColumnCopies, CopyColumns, Sort, Group, MergeNeighborhoods, AddNeighborhoods, AddEdges} TExperiment;
  /// The expected number of experiments (must be at least equal to the size of the above list)
  static const int NEXPS = 15;

  static TStopwatch* GetInstance() {
    static TStopwatch instance; // Guaranteed to be destroyed. Instantiated on first use.
    return &instance;
  }

private:
  TStopwatch() {}; // Constructor? (the {} brackets) are needed here.

  // Dont forget to declare these two. You want to make sure they
  // are unacceptable otherwise you may accidentally get copies of
  // your singleton appearing.
  TStopwatch(TStopwatch const&);              // Don't Implement
  void operator=(TStopwatch const&); // Don't implement

  double Starts[NEXPS];
  int Cnts[NEXPS];
  double Sums[NEXPS];

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
};

#endif //STOPWATCH_H


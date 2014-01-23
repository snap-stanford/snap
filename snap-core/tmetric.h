#ifndef METRIC_H
#define METRIC_H

//#//////////////////////////////////////////////
/// Metric class: base class for distance metrics
class TMetric {
protected:
  TStr MetricName; ///< Name of the metric defined by this class
public:
  TMetric(TStr Name) : MetricName(Name) {}
  /// Get the name of this metric
  TStr GetName();
  /// Virtual base function for defining metric on floats
  virtual TFlt NumDist(TFlt,TFlt) { return -1; }
  /// Virtual base function for defining metric on strings
  virtual TFlt StrDist(TStr,TStr) { return -1; }
};

//#//////////////////////////////////////////////
/// Euclidean metric class: compute distance between two floats
class TEuclideanMetric: public TMetric {
public:
  TEuclideanMetric(TStr Name) : TMetric(Name) {}
  /// Calculate the euclidean distance of two floats
  TFlt NumDist(TFlt x1,TFlt x2) { return fabs(x1-x2); }
};

#endif  // METRIC_H

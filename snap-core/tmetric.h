#ifndef METRIC_H
#define METRIC_H

class TMetric{
protected:
	TStr MetricName;
public:
	TMetric(TStr Name);
	TStr GetName();
	virtual TFlt NumDist(TFlt,TFlt) {return -1;}
	virtual TFlt StrDist(TStr,TStr) {return -1;}
};

class TEuclideanMetric: public TMetric{
public:
  TEuclideanMetric(TStr Name):TMetric(Name){}
	TFlt NumDist(TFlt x1,TFlt x2){return fabs(x1-x2);}
};
#endif  // METRIC_H

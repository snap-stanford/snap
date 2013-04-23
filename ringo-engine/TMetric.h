#include <math.h>?>

class TMetric{
protected:
	TStr MetricName;
public:
	TMetric(TStr Name);
	TStr GetName();
	virtual TFlt NumDist(TFlt,TFlt) {return -1;}
	virtual TFlt StrDist(TStr,TStr) {return -1;}
};

class TEucledianMetric: public TMetric{
public:
	TEucledianMetric(){MetricName = "Eucledian";}
	TFlt NumDist(TFlt x1,TFlt x2){return fabs(x1-x2);}
}

The documentation below is somewhat out of date.
Run "perf -h" to see current options.

Input to program is a sequence of lines, one line per case
with format: "TRUE_VALUE whitespace PREDICTED_VALUE"

The file test.data contains sample test data.  The file
test.results contains the results of running:

perf -options < test.data > test.results
or
perf -options -file test.data

TRUE_VALUE and PRED_VALUE can be any numbers, positive
or negative.  The program computes the mean TRUE_VALUE and
mean PRED_VALUE.

If TRUE_VALUE <= MEAN_TRUE_VALUE, the truth for the case is
considered to be a 0, otherwise it is considered a 1. This
lets you use any coding for the TRUE_VALUE's as long as the
value used to represent 0's is less than the value used to
represent 1's.  So you can code things as 0/1, -1/+1, etc.
Note that some metrics such as squared error are calculated
using the TRUE_VALUEs input to the code, not values transformed
to 0/1.

The program computes the ROC curve, ROC area, accuracy (ACC),
Positive Predictive Value (PPV), Negative Predictive Value (NPC),
Sensitivity (SEN), Specificity(SPC), Precision/Recall curve,
F, Precision/Recall Break Even point, Lift, and a number of
other performance metrics.

Computing things like Accuracy requires a prediction threshold
for the PRED_VALUE:

(PRED_VALUE <  thresh) is interpreted as a prediction of 0.
(PRED_VALUE >= thresh) is interpreted as a prediction of 1.

The program takes an optional argument that lets you specify the
prediction threshold.  If you don't specify a value, the program
assumes 0.5, which is the right thing to do if the PRED_VALUES
are properly calibrated probabilities.

The program also computes two different prediction threshold
values itself. The first is done by finding the threshold that
would make the number of predicted 1's match the number of true
1's in the data set.  If 15 of 100 points given to the program
are true 1's, it sorts the points by PRED_VALUE (least first) and
computes a threshold from the average PRED_VALUE of points 84 and
85 in the sort.  This prediction threshold makes 15 of the 100
points be predicted 1.  (If two or more points have the same
PRED_VALUE and these to sort to the place where the threshold
finder picks values, the number of points predicted to be 1 can't
match the number of true 1's.)

The second computed threshold is the prediction threshold that
maximizes the Accuracy.  This threshold is found by trying all
thresholds that fall half way between adjacent prediction values
in the data set, and reporting the one that yields the highest
accuracy.  It is not uncommon for more than one threshold to
yield the same maximum accuracy.  In this case, the program
reports the lowest threshold that achieved maximum accuracy, and
prints a caution that more than one prediction threshold achives
this same accuracy.  When this happens, you can rerun the program
with the -accplot option to see the accuracy for each threshold.
(In Unix "perf -accplot -noroc < infile | sort -n +1 | tail"
will find just those thresholds that yielded highest accuracy.)

The program computes the error measures listed above for the 0.5
threshold (or user supplied threshold) and for the two automatically
computed thresholds.  (Neither affects the ROC curve or its area.)

Be careful using computed thresholds and their associated stats.
It is probably improper to use the threshold computed on a test
set to estimate statistics for that test set.  The threshold
probably should be estimated with the training set, and then
given to the program to calculate test set stats.  That's why the
threshold can be specified as an optional parameter.

The ROC curve is a plot of SENSITIVITY vs. 1-SPECIFICITY as the
prediction threshold sweeps from 0 to 1.  A typical ROC curve for
reasonably accurate predictions looks like this:

    |                                   *
S   |                         *
E   |                 *
N   |           *
S   |
I   |       *
T   |
I   |    *
V   |
I   |  *
T   |
Y   |*
    - - - - - - - - - - - - - - - - - - -
		   1 - SPECIFICITY

If there is no relationship between the prediction and truth, thethe
ROC curve is a diagonal line with area 0.5.  If the prediction
strongly predicts truth, the curve rises quickly and has area near
1.0.  If the prediction strongly predicts anti-truth, the ROC area
is less than 0.5.

Here's a definition of SPECIFICITY, SENSITIVITY, and the other
error measures this program computes such as PRECISION, RECALL,
LIFT, ...  (Some of this is from Constantin's email.)

Here's a confusion matrix for a two class problem:

		 MODEL PREDICTION

	    |       1       0       |
      - - - + - - - - - - - - - - - + - - - - -
TRUE         1   |       A       B       |    A+B
OUTCOME           |                       |
	0   |       C       D       |    C+D
      - - - + - - - - - - - - - - - + - - - - -
	    |      A+C     B+D      |  A+B+C+D


      1 = POSITIVE
      0 = NEGATIVE


      ACC = (A+D) /(A+B+C+D)
      PPV = A / (A+C)
      NPV = D / (B+D)
      SEN = A / (A+B)
      SPE = D / (C+D)
      PRE = A / (A+C)  (PRECISION = PPV)
      REC = A / (A+B)  (RECALL = SENSITIVITY)

A Precision/Recall Curve is a plot of Precision on the vertical
axis vs. Recall on the horizontal axis as the threshold sweeps
through the data.  A Precision/Recall plot attempts to measure
similar behavior as an ROC plot.  But ROC and PR plots are not
the same.  The vertical axis on an ROC plot is the same as the
horizontal axis on a PR plot, but the other axes differ. PR plots
tend to be used by the information retreival communities, wheras
ROC curves tend to be used by the medical, signal processing,
statistics, and machine learning communities.  ROC plots have
some properties that PR plots do not have:  ROCs are monotonic,
always touch both axes, and have well characterized statistical
properties.  PR plots need not be monotonic (but often are), may
not touch the Precision axes (when the cases with the highest
predicted values are class negative), and has less statistical
semantics.  In practice, however, both provide qualitatively
similar views of performance.

Here is a sample Precision/Recall Curve:


    |
P   |     *
R   |  *     *
E   |           *
C   |              *
C   |                *
I   |                    *
S   |                      *
I   |                       *
O   |                         *
N   |                           *
    |                            *
    - - - - - - - - - - - - - - - - - - -
		   RECALL


LIFT and LIFT CURVES measure similar quantities as ROC and
and Precision/Recall.  Lift is a measure of the percent of
the positive class that is in the returned objects.  For
example, if a dataset contains 1000 objects, 200 of which
are the target class (positives), and the threshold is set
so that 30% of the dataset is returned, and that 30% contains
90% of the positives, then LIFT = 0.90/0.30 = 3.  LIFT greater
than 1 means the retreived objects (the objects greater than
threshold) have more targets in them than a random sample
would have.  LIFT less than 1 is bad.  LIFT = 1 is what is
expected from random (uninformed) sampling.  Obviously we
want LIFT as large as possible.  The largest  LIFT occurs
if 100% of the target class is returned without any of the
negatives being included, perfect prediction.  If the target
class is p% of the dataset, and all targets are returned in
a sample also of size p%, LIFT = 100%/p%.  For the example
above where 20% of the data is targets, the maximum lift is
100%/20% = 5.

LIFT sometimes is plotted as a function of the percent of
objects returned.  This is the LIFT plot.  LIFT often is
presented in a table of LIFTS measured when some small but
interesting percent of the objects are returned (e.g., 5%,
10%, 15%, 20%, 25%, ...).  LIFT is a good measure to use in
addition to accuracy.  Suppose the accuracy of a model is
90%.  Is this good performance?  Maybe.  Maybe not.  If it
is possible to acheive 90% accuracy by just predicting the
most common class, then a model with 90% accuracy is nothing
to write home about.  LIFT is less sensitive to the base
prediction rate than accuracy.  For example, if 90% of the
data is 0's, 10% 1's, the base accuracy is 90% if the model
predicts 0 for everything, so 90% accuracy is not really
very good.  If a threshold is used that causes 10% of the
cases to be predicted 1, and if 80% of the cases predicted
to be 1 actually are 1, the accuracy is 0.80*0.1 +

Lift tends to be used in marketing and data mining more
than in machine learning and information retreival because
it is easy to understand, and because it directly measures
how effective the model is at detecting targets with high
reliability.  For example, if you are going to send marketing
flyers to only 10% of your customers, you want that 10% to
include as large a fraction of targets (responders) as
possible.

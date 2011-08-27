#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MAX_ITEMS 50000000
#define MAX_TIES   50000
#define MAX_BINS    1000
#define debug 0
#define eps 1.0e-10
#define nnn -999999

/* Version 5.12 KDDCUP-2004 July 12, 2004 */

/* COPYRIGHT INFO

perf is available at http://www.cs.cornell.edu/~caruana/

Author: Rich Caruana
  caruana@cs.cornell.edu

  Cornell University
  Department of Computer Science
  4157 Upson Hall
  Ithaca, NY 14853
  USA

LICENSING

This program is granted free of charge for research and education
purposes.  You must obtain a license from the author to use it for
commercial purposes.

Please acknowledge perf in any scientific results and reports where
perf was used.  Many of the performance metrics perf calculates are
not always defined the same way by different communities.  By citing
perf you make it clear exactly what metrics you are reporting.  We do
not yet have a report describing perf, so the best way to acknowledge
using to code is:

       R. Caruana, The PERF Performance Evaluation Code,
       http://www.cs.cornell.edu/~caruana/perf

The software must not be modified and distributed without prior
permission of the author.

By using perf you agree to the licensing terms.

NO WARRANTY

BECAUSE THIS PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY
FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW. EXCEPT
WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER
PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE. THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME
THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.

IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR
REDISTRIBUTE THE PROGRAM, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY
GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF
THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED TO
LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY
YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY
OTHER PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED
OF THE POSSIBILITY OF SUCH DAMAGES.

*/

/*
   WARNING!: This code was recently rewritten has not yet been
             thoroughly tested.  If you find errors please send
             email to caruana@cs.cornell.edu

   ACKNOWLEDGEMENTS:
   Over the years, many people have helped with this code.
   We'd like to thank:

   - Alex Niculescu
   - Filip Radlinski
   - Thorsten Joachims
   - Constantin Aliferis
   - Stefano Monti
   - Greg Cooper

   *** WARNING ***

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

*/

float  btrue[MAX_ITEMS];
float  bpred[MAX_ITEMS];
double mean_apr, mean_rkl, mean_rms, mean_top1, mean_roc;
double stddev_apr, stddev_rkl, stddev_rms, stddev_top1, stddev_roc;
int    total_items, cur_block, no_blocks, bitem, more_blocks;
int    block[MAX_ITEMS];
float  true[MAX_ITEMS];
float  pred[MAX_ITEMS];
float  fraction[MAX_ITEMS];    //for the fractional cases to handle ties
double mean_true, mean_pred;
double min_true, max_true, assumed_true;
int    not_min_warned, not_max_warned, targs_all_same;
double pred_thresh;
double sse, rmse, cxe, loge, log_2, norm, norm_err;
double a, b, c, d;
double freq_thresh, threshold;
double max_acc, max_acc_thresh, last_acc_thresh, acc;
double min_cost, min_cost_thresh, last_cost_thresh, cost;
double cost_a, cost_b, cost_c, cost_d;
int    costs, min_cost_count;
double freq_a, freq_b, freq_c, freq_d;
double max_acc_a, max_acc_b, max_acc_c, max_acc_d;
int    max_acc_count;
int    count, no_poz, begin;
int    rank_last, top_sum, top, t01, t10;
double topn;
double factCache[MAX_TIES];

int arg, taken, no_area, thresh, confusion, prcdata, plot;

int ACC_flag, APR_flag, CAL_flag, CST_flag, CXE_flag, TOP_flag, RKL_flag;
int LFT_flag, NPV_flag, NRM_flag, PPV_flag, PRB_flag, PRF_flag, PRE_flag;
int REC_flag, RMS_flag, ROC_flag, R50_flag, SEN_flag, SLQ_flag, SPC_flag;
int SAR_flag, T01_flag, T10_flag;
int BLOCKS_flag;

int acc_plot, cost_plot, roc_plot, pr_plot, lift_plot;

int SET_stats, SET_easy, SET_all;

double slacq_window, slacq_bin_width, slac_w, slac_q, last_p, n_cor, n_tot;
int slacq_tb[MAX_BINS], slacq_tbb[MAX_BINS], bin, no_bins, set, slac_qn;
int no_item, item;
double tt, tf, ft, ff;
int    total_true_0, total_true_1;
double sens, spec, tpf, fpf, tpf_prev, fpf_prev, roc_area, roc50_area;
double precision, recall, precision_prev, recall_prev, pr_break_even;
double lift, frac_above_threshold;
double prcones, apr;
double lasttt, lasttf, lastft, lastff;
int    cnt, onecnt, i;
int data_split, no_remaining;
double data_prc;
double allwacc, allwroc, allwrms;
int loss;
double max_lift, min_lift;
int cal_bin_size, cal_f, cal_t, cal_sum_n;
double sum_p, cal_sum, cal_sumsquares, obs_p, cal_err, cal1_mean_err, cal2_mean_err, plow, phigh;

int tmp_int;
double tmp_double;

/* Print the usage */

void print_usage(char *s) {

  printf("Version 5.12 [KDDCUP-2004 July 12, 2004]\n\n");
  printf("Usage:\n\t   %s [options] [-blocks] < input \n", s);
  printf("\tOR %s [options] [-blocks] -file <input file> \n", s);
  printf("\tOR %s [options] [-blocks] -files <targets file> <predictions file> \n\n", s);

  printf("Allowed options:\n");

  printf("\n  PERFORMANCE MEASURES\n");
  printf("   -ACC             Accuracy\n");
  printf("   -RMS             Root Mean Squared Error\n");
  printf("   -CXE             Mean Cross-Entropy\n");
  printf("   -ROC             ROC area\n");
  printf("   -R50             ROC area up to 50 negative examples\n");
  printf("   -SEN             Sensitivity\n");
  printf("   -SPC             Specificity\n");
  printf("   -NPV             Negative Predictive Value\n");
  printf("   -PPV             Positive Predictive Value\n");
  printf("   -PRE             Precision\n");
  printf("   -REC             Recall\n");
  printf("   -PRF             F1 score\n");
  printf("   -PRB             Precision/Recall Break Even Point\n");
  printf("   -APR             Mean Average Precision\n");
  printf("   -LFT             Lift (at threshold)\n");
  printf("   -TOP1            Top 1: is the top ranked case positive\n");
  printf("   -TOP10           Top 10: is there a positive in the top 10 ranked cases\n");
  printf("   -NTOP <N>        How many positives in the top N ranked cases\n");
  printf("   -RKL             Rank of *last* (poorest ranked) positive case\n");
  printf("   -NRM <arg>       Norm error using L<arg> metric\n");
  printf("   -CST <tp> <fn> <fp> <tn>\n");
  printf("                    Total cost using these cost values, plus min-cost results\n");
  printf("   -SAR <wACC> <wROC> <wRMS>\n");
  printf("                    SAR = (wACC*ACC + wROC*ROC + wRMS(1-RMS))/(wACC+wROC+wRMS)\n");
  printf("                    typically wACC = wROC = wRMS = 1.0\n");
  printf("   -CAL <bin_size>  CA1/CA2 scores\n");
  printf("   -SLQ <bin_width> Slac Q-score\n");

  printf("\n  METRIC GROUPINGS\n");
  printf("   -all             Display most metrics (the default if no options are specified)\n");
  printf("   -easy            ROC, ACC and RMS\n");
  printf("   -stats           Accuracy, confusion table metrics and lift\n");
  printf("   -confusion       Confusion table plus all metrics in stats\n");

  printf("\n  PLOTS (Only one plot can be output at a time)\n");
  printf("   -plot roc        Output ROC plot\n");
  printf("   -plor pr         Output Precision/Recall plot\n");
  printf("   -plot lift       Output Lift versus threshold plot\n");
  printf("   -plot cost       Output Cost versus threshold plot\n");
  printf("   -plot acc        Output Accuracy versus threshold plot\n");

  printf("\n  PARAMETERS\n");
  printf("   -t <arg>         Set threshold [default threshold is 0.5 if not set]\n");
  printf("   -percent <arg>   Set threshold so <arg> percent of data falls above threshold\n");
  printf("                    (is predicted positive)\n");

  printf("\n  INPUT\n");
  printf("   -blocks          Input has BLOCK ID numbers in first column. Calculate performance\n");
  printf("                    for each block and report the mean performance across the blocks.\n");
  printf("                    Only works with APR, TOP1, RKL, ROC, and RMS.\n");
  printf("                    If using separate files for target and predictions input (-files\n");
  printf("                    option), the BLOCK ID numbers must be ithe first column of the\n");
  printf("                    target file, with no block numbers in the predictions file.\n\n");
  printf("   -file  <file>    Read input from one file (1st col targets, 2nd col predictions)\n");
  printf("   -files <targets file> <predictions file>  Read input from two separate files\n");
  printf("\n");
}

void print_confusion_table (a,b,c,d,thresh_text,thresh)
     double a,b,c,d;
     char* thresh_text;
     double thresh;
{
  printf ("\n");
  printf ("       Predicted_1  Predicted_0   %s %9.6lf\n", thresh_text, thresh);
  printf ("True_1  %6.4lf       %6.4lf\n", a, b);
  printf ("True_0  %6.4lf       %6.4lf\n", c, d);
}


/* partition is used by quicksort */

int partition (p,r)
     int p,r;
{
  int i, j;
  float x, tempf;

  x = pred[p];
  i = p - 1;
  j = r + 1;
  while (1)
    {
      do j--; while (!(pred[j] <= x));
      do i++; while (!(pred[i] >= x));
      if (i < j)
  {
    tempf = pred[i];
    pred[i] = pred[j];
    pred[j] = tempf;
    tempf = true[i];
    true[i] = true[j];
    true[j] = tempf;
  }
      else
  return(j);
    }
}

/* vanilla quicksort */

void quicksort (p,r)
     int p,r;
{
  int q;

  if (p < r)
    {
      q = partition (p,r);
      quicksort (p,q);
      quicksort (q+1,r);
    }
}

/* If the option is equal to the wanted string (case ignored), set the flag */
int accept_flag(char *option, char *wanted, int *flag) {

  /* Use strncasecmp to avoid buffer overflows. */
  if (strncasecmp(option, wanted, 20)==0) {
    *flag = 1;
    return 1;
  }
  return 0;
}

/* If the option is equal to the wanted string (case ignored) plus
   anything at the end, set the flag */
int accept_flag_ext(char *option, char *wanted, int *flag) {

  if (strncasecmp(option, wanted, strlen(wanted))==0) {
    *flag = 1;
    return 1;
  }
  return 0;
}

/* If the option is equal to the wanted string, the next argument is
   the floating point value */
int accept_parameter(char *argv[], int *option_n, char *wanted, int *flag, double *value) {

  char *option;
  char *next_option;

  option = argv[*option_n];
  next_option = argv[*option_n+1];

  if (strncasecmp(option, wanted, 20)==0) {
    *flag = 1;
    if (value == NULL) {
      fprintf(stderr, "Error: parameter %s requires an option.\n", option);
      exit(1);
    }
    *value = atof(next_option);
    /* Skip the next option in our processing code */
    (*option_n) ++;
    return 1;
  }
  return 0;
}

void print_output(int flag, char *name, double value) {
  if (flag == 1)
    printf ("%3s   %8.5lf\n", name, value);
}

void print_thresh_output(int flag, char *name, double value, char *t_name, double t_val) {
  if (flag == 1)
    printf("%3s   %8.5lf   %s %9.6lf\n", name, value, t_name, t_val);
}

/* Print standard stats */
void print_stats(double stat_a, double stat_b, double stat_c, double stat_d, char *t_name, double t_val){

  precision = stat_a / (stat_a + stat_c + eps);
  recall    = stat_a / (stat_a + stat_b + eps);

  if (loss == 0) {
    print_thresh_output (ACC_flag, "ACC", (stat_a+stat_d) / ((stat_a+stat_b+stat_c+stat_d)+eps), t_name, t_val);
    print_thresh_output (PPV_flag, "PPV", stat_a / ((stat_a+stat_c)+eps), t_name, t_val);
    print_thresh_output (NPV_flag, "NPV", stat_d / ((stat_b+stat_d)+eps), t_name, t_val);
    print_thresh_output (SEN_flag, "SEN", stat_a / ((stat_a+stat_b)+eps), t_name, t_val);
    print_thresh_output (SPC_flag, "SPC", stat_d / ((stat_c+stat_d)+eps), t_name, t_val);
    print_thresh_output (PRE_flag, "PRE", precision, t_name, t_val);
    print_thresh_output (REC_flag, "REC", recall,    t_name, t_val);
    print_thresh_output (PRF_flag, "PRF", (2.0*precision*recall)/(precision+recall+eps), t_name, t_val);
    print_thresh_output (LFT_flag, "LFT", (stat_a / ((double) total_true_1)) / ((stat_a + stat_c) / ((double) (total_true_1 + total_true_0)) + eps), t_name, t_val);
    if ( SAR_flag )
      printf ("SAR   %8.5lf   %s %9.6lf wacc %9.6lf wroc %9.6lf wrms %9.6lf \n", ((allwacc*(stat_a+stat_d) / ((stat_a+stat_b+stat_c+stat_d)+eps) + allwroc*roc_area + allwrms*(1.0-rmse))) / (allwacc+allwroc+allwrms+eps), t_name, t_val, allwacc, allwroc, allwrms);

  } else if (loss == 1) {

    print_thresh_output (ACC_flag, "ACC", 1.0 - ((stat_a+stat_d) / ((stat_a+stat_b+stat_c+stat_d)+eps)), t_name, t_val);
    print_thresh_output (PPV_flag, "PPV", 1.0 - (stat_a / ((stat_a+stat_c)+eps)), t_name, t_val);
    print_thresh_output (NPV_flag, "NPV", 1.0 - (stat_d / ((stat_b+stat_d)+eps)), t_name, t_val);
    print_thresh_output (SEN_flag, "SEN", 1.0 - (stat_a / ((stat_a+stat_b)+eps)), t_name, t_val);
    print_thresh_output (SPC_flag, "SPC", 1.0 - (stat_d / ((stat_c+stat_d)+eps)), t_name, t_val);
    print_thresh_output (PRE_flag, "PRE", 1.0 - precision, t_name, t_val);
    print_thresh_output (REC_flag, "REC", 1.0 - recall,    t_name, t_val);
    print_thresh_output (PRF_flag, "PRF", 1.0 - (2.0*precision*recall)/(precision+recall+eps), t_name, t_val);
    print_thresh_output (LFT_flag, "LFT", (max_lift - (stat_a / ((double)total_true_1)) / ((stat_a + stat_c) / ((double) (total_true_1 + total_true_0)) + eps))/(max_lift-min_lift+eps), t_name, t_val);
    if ( SAR_flag )
      printf ("SAR   %8.5lf   %s %9.6lf wacc %9.6lf wroc %9.6lf wrms %9.6lf \n", 1.0 - (((allwacc*(stat_a+stat_d) / ((stat_a+stat_b+stat_c+stat_d)+eps) + allwroc*roc_area + allwrms*(1.0-rmse))) / (allwacc+allwroc+allwrms+eps)), t_name, t_val, allwacc, allwroc, allwrms);

  } /* End of loop if loss is set or not */

  print_thresh_output(CST_flag, "CST", cost_a*stat_a + cost_b*stat_b + cost_c*stat_c + cost_d*stat_d, t_name, t_val);

} /* End of print_stats */


double calc_rmse()
{
  int item;
  double sse, rmse;
  sse = 0.0;
  for (item=0; item<no_item; item++)
    {
      sse+= (true[item]-pred[item])*(true[item]-pred[item]);
      if ( (pred[item] > 1.0) && (not_max_warned == 1) ) {
  fprintf(stderr, "Warning: Prediction (%10.6lf) larger than 1.0 when calculating RMS or CXE\n", pred[item]);
  not_max_warned = 0;
      }
      if ( (pred[item] < 0.0) && (not_min_warned == 1) ) {
  fprintf(stderr, "Warning: Prediction (%10.6lf) less than 0.0 when calculating RMS or CXE\n", pred[item]);
  not_min_warned = 0;
      }
    }
  rmse = sqrt (sse / ((double) no_item));
  return(rmse);
}

double calc_mcxe()
{
  int item;
  double cxe, log_2;
  cxe = 0.0;
  log_2 = log(2.0);
  /* loge = log(2.718281828); */
  for (item=0; item<no_item; item++)
    {
      if ( true[item] < mean_true )
  {
    true[item] = 0;
    total_true_0++;
    if ((1-pred[item]) <= 0.0)
      cxe+= 9e99;
    else
      cxe+= -log(1-pred[item])/log_2;
  }
      else
  {
    true[item] = 1;
    total_true_1++;
    if ( pred[item] <= 0.0 )
      cxe+= 9e99;
    else
      cxe+= -log(pred[item])/log_2;
  }
      if ( (pred[item] > 1.0) && (not_max_warned == 1) ) {
  fprintf(stderr, "Warning: Prediction (%10.6lf) larger than 1.0 when calculating RMS or CXE\n", pred[item]);
  not_max_warned = 0;
      }
      if ( (pred[item] < 0.0) && (not_min_warned == 1) ) {
  fprintf(stderr, "Warning: Prediction (%10.6lf) less than 0.0 when calculating RMS or CXE\n", pred[item]);
  not_min_warned = 0;
      }
    }
  cxe = cxe / ((double) no_item);
  return(cxe);
}

double calc_norm()
{
  int item;
  double norm_err;
  norm_err = 0.0;
  for (item=0; item<no_item; item++)
    norm_err+= pow(fabs(true[item]-pred[item]),norm);
  norm_err = pow(norm_err/ ((double) no_item),1/norm);
  return(norm_err);
}

double calc_roca(int do_roc_plot)
{
  double tt, tf, ft, ff;
  double sens, spec, tpf, fpf, tpf_prev, fpf_prev;
  double roc_area;

  tt = 0;
  tf = total_true_1;
  ft = 0;
  ff = total_true_0;

  sens = ((double) tt) / ((double) (tt+tf));
  spec = ((double) ff) / ((double) (ft+ff));
  tpf = sens;
  fpf = 1.0 - spec;
  if ( do_roc_plot )
    printf ("%6.4lf %6.4lf\n", fpf, tpf);
  roc_area = 0.0;
  tpf_prev = tpf;
  fpf_prev = fpf;

  for (item=no_item-1; item>-1; item--)
    {
      tt+= fraction[item];
      tf-= fraction[item];
      ft+= 1 - fraction[item];
      ff-= 1 - fraction[item];
      sens = ((double) tt) / ((double) (tt+tf));
      spec = ((double) ff) / ((double) (ft+ff));
      tpf  = sens;
      fpf  = 1.0 - spec;
      if ( do_roc_plot )
  printf ("%6.4lf %6.4lf\n", fpf, tpf);
      roc_area+= 0.5*(tpf+tpf_prev)*(fpf-fpf_prev);
      tpf_prev = tpf;
      fpf_prev = fpf;
    }
  return(roc_area);
}

double calc_rocn(int no_negs)
{
  double tt, tf, ft, ff;
  double sens, spec, tpf, fpf, tpf_prev, fpf_prev;
  double roc_area;

  tt = 0;
  tf = total_true_1;
  ft = 0;
  ff = total_true_0;

  sens = ((double) tt) / ((double) (tt+tf));
  spec = ((double) ff) / ((double) (ft+ff));
  tpf = sens;
  fpf = 1.0 - spec;
  roc_area = 0.0;
  tpf_prev = tpf;
  fpf_prev = fpf;

  for (item=no_item-1; item>-1; item--)
    {
      tt+= fraction[item];
      tf-= fraction[item];
      ft+= 1 - fraction[item];
      ff-= 1 - fraction[item];
      sens = ((double) tt) / ((double) (tt+tf));
      spec = ((double) ff) / ((double) (ft+ff));
      tpf  = sens;
      fpf  = 1.0 - spec;
      if (ft <= no_negs)
  roc_area += 0.5*(tpf+tpf_prev)*(fpf-fpf_prev)*(total_true_0/no_negs);
      tpf_prev = tpf;
      fpf_prev = fpf;
    }
  return(roc_area);
}

double calc_top1()
{
  if ( fraction[no_item-1] >= (1.0-eps) )
    return(1.0);
  else
    return(0.0);
}

double calc_topn(int n)
{
  int item;
  for (item=no_item-1; item>-1; item--)
    if ( ((no_item - item) <= n) && (fraction[item] >= (1.0-eps)) )
      return(1.0);
  return(0.0);
}

double calc_rank_last()
{
  int rank_last, item;
  rank_last = no_item - 1;
  if ( targs_all_same == 1 )
    return((double) no_item);
  for (item=no_item-1; item>-1; item--)
    if ( fraction[item] > 0 )
      rank_last = no_item - item;
  return((double) rank_last);
}

double calc_top_sum(int topn)
{
  int item;
  double top, top_sum;
  top_sum = 0.0;
  top = (int) (topn+0.01);
  for (item=no_item-1; item>-1; item--)
    if ( (no_item - item) <= top )
      top_sum+= fraction[item];
  return(top_sum);
}

double calc_break_even_point()
/* compute the Break-Even Point. We compute it as the Recall
   when you predict total_true_1 cases as being positive cases
   ties are split by fractional cases */
{
  int item;
  double tt;
  tt = 0.0;
  for ( item = no_item - 1; item >= total_true_0; --item )
    tt += fraction[item];
  return(tt/((double)total_true_1));
}

double calc_slq_score()
/* calculate improved SLAC Q-score using two test set samples and evenly spaced bins */
{
  int bin, item;
  int slacq_tb[MAX_BINS], slacq_tbb[MAX_BINS], slac_qn;
  double slac_q, slac_w;
  for (bin=0; bin<no_bins; bin++)
    {
      slacq_tb[bin]  = 0;
      slacq_tbb[bin] = 0;
    }
  for (item=0; item<no_item; item++)
    {
      bin = floor(pred[item] / (slacq_bin_width+eps));
      if ( true[item] > 0.5 )
  slacq_tb[bin]++;
      else
  slacq_tbb[bin]++;
    }
  slac_q = 0.0;
  slac_qn = 0;
  for (bin=0; bin<no_bins; bin++)
    {
      if ( slacq_tb[bin] > slacq_tbb[bin] )
        slac_w = 1.0 - ( ((double)slacq_tb[bin])  / ( ((double)(slacq_tb[bin]+slacq_tbb[bin])) + eps) );
      else
        slac_w = 1.0 - ( ((double)slacq_tbb[bin]) / ( ((double)(slacq_tb[bin]+slacq_tbb[bin])) + eps) );
      slac_q  += ((double)(slacq_tb[bin]+slacq_tbb[bin])) * (1.0 - 2.0*slac_w) * (1.0 - 2.0*slac_w);
      slac_qn += slacq_tb[bin]+slacq_tbb[bin];
    }
  slac_q = slac_q / ((double)slac_qn);
  return(slac_q);
}
double chooseLog(int n, int m){
  if(m < 0 || m > n)return -1.0E300;
  double ret = factCache[n] - factCache[m] - factCache[n-m];
  return ret;
}
double apr_prob(int j, int k, int n, int m){
  if(j-1 < j-k-1)return 0;
  if(n-j < m-k-1)return 0;
  double ret = 0;
  ret += chooseLog(j-1,k);
  ret += chooseLog(n-j,m-k-1);
  ret -= chooseLog(n,m);
  if(ret > -50)
    return exp(ret);
  else
    return 0;
}
/*
 * a is the number of cases before the tie
 * b is the number of positives before the tie
 * n is the number of cases in the tie
 * m is the number of positives in the tie
 */
double apr_ties(int a, int b, int n, int m){
  int j,k;
  double ret = 0;
  if(m==0)return 0;
  //code works for this case, but this way is faster
  if(n==1)return (b + 1.0) / (a + 1.0);
  if(n >= MAX_TIES){
    fprintf(stderr, "Warning: There are more than %d ties in your prediction, using pessimistic ordering in Average Precision.\n",MAX_TIES);
    for(j = 1; j<=m; j++){
      ret += ((double)b + j) / (a + n - m + j);
    }
    return ret;
  }
  for(j = 1; j<=n; j++){
    for(k = 0; k<j && k<=m; k++){
      double p = apr_prob(j,k,n,m);
      double contrib = ((double)b + k + 1) / (a + j);
      ret += p * contrib;
    }
  }
  return ret;
}

double calc_apr(int pr_plot)
{
/* now let's do the PRECISION/RECALL curve and Mean Average Precision*/
/* note: the approach followed here may not be standard when it comes to ties */
/* note: the approach followed here computes the area under the precision/recall
   curve, not just the average precision at recall = 0,.1,.2,..,1.0 */

  int item,i;
  double tt, tf, ft, ff, apr, precision, recall;
  double t_in_tie, f_in_tie, m, n;
  factCache[0] = 0;
  for(i=1; i<MAX_TIES; i++)
    factCache[i] = factCache[i-1] + log((double)i);

  /* if all items negative, precision and recall are not well defined
     so just return 0.0 and don't do PR plot */
  if ( targs_all_same == 1 && assumed_true == 0.0 ) {
    fprintf(stderr, "Warning: Precision and Recall not well defined if all cases are class 0.\n");
    return(0.0);
  }

  tt = 0;
  tf = total_true_1;
  ft = 0;
  ff = total_true_0;
  apr = 0.0;
  t_in_tie = 0.0;
  f_in_tie = 0.0;

  for (item=no_item-1; item>-1; item--) {
    tt+= fraction[item];
    tf-= fraction[item];
    ft+= 1 - fraction[item];
    ff-= 1 - fraction[item];
    t_in_tie += fraction[item];
    f_in_tie += 1-fraction[item];
    if(item==0 || pred[item] != pred[item-1]){
      n = tt + ft - f_in_tie - t_in_tie;
      m = tt - t_in_tie;
      apr += apr_ties(n+0.5,m+0.5,t_in_tie+f_in_tie+0.5,t_in_tie+0.5);
      t_in_tie = 0;
      f_in_tie = 0;
    }
    precision = tt / (tt+ft);
    recall    = tt / total_true_1;
    if ( pr_plot )
      printf ("%6.4lf %6.4lf\n", recall, precision);
  }
  return(apr/((double) total_true_1));
}

double calc_area_under_pr(int pr_plot)
{
/* now let's do the PRECISION/RECALL curve and Mean Average Precision*/
/* note: the approach followed here may not be standard when it comes to ties */
/* note: the approach followed here computes the area under the precision/recall
   curve, not just the average precision at recall = 0,.1,.2,..,1.0 */

  int item;
  double tt, tf, ft, ff, apr, precision, recall, precision_prev, recall_prev;

  /* if all items positive, precision and recall are not well defined
     so just return 0.0 and don't do PR plot */
  if ( targs_all_same == 1 && assumed_true == 0.0 ) {
    fprintf(stderr, "Warning: Precision and Recall not well defined if all cases are class 0.\n");
    return(0.0);
  }

  tt = 0;
  tf = total_true_1;
  ft = 0;
  ff = total_true_0;
  apr = 0.0;

  precision_prev = 0.0;
  recall_prev = 0.0;

  for (item=no_item-1; item>-1; item--) {
    tt+= fraction[item];
    tf-= fraction[item];
    ft+= 1 - fraction[item];
    ff-= 1 - fraction[item];
    if ((tt+ft) > 0.0)
      precision = tt / (tt+ft);
    else
      precision = 0;
    recall    = tt / total_true_1;
    apr += 0.5*(precision+precision_prev)*(recall-recall_prev);
    if ( pr_plot )
      printf ("%6.4lf %6.4lf\n", recall, precision);
    precision_prev = precision;
    recall_prev = recall;
  }
  return(apr);
}

void calc_lift(int lift_plot)
{
/* let's do the lift curve */

  int item;
  double tt, tf, ft, ff, frac_above_threshold, lift;
  tt = 0;
  tf = total_true_1;
  ft = 0;
  ff = total_true_0;
  for (item=no_item-1; item>-1; item--) {
    tt+= fraction[item];
    tf-= fraction[item];
    ft+= 1 - fraction[item];
    ff-= 1 - fraction[item];
    frac_above_threshold = (tt + ft) / no_item;
    lift = ( tt / (total_true_1 + eps)) / (frac_above_threshold + eps);
    if ( lift_plot )
      printf ("%6.4lf %6.4lf\n", frac_above_threshold, lift);
  }
}

double calc_cal1()
{
/* calculate mean absolute calibration error */
  int item, cal_sum_n, cal_f, cal_t;
  double cal_sum, plow, phigh, sum_p, obs_p, cal_err;

  cal_sum = 0.0;
  cal_sum_n = 0;
  for (i=0; i<19; i++) {
    plow = i * 0.05;
    phigh = plow + 0.1;
    cal_f = 0;
    cal_t = 0;
    sum_p = 0.0;
    for (item=0; item<no_item; item++)
      if ( pred[item] >= plow && pred[item] <= phigh )
  {
    if ( true[item] == 1 )
      cal_t++;
    else
      cal_f++;
    sum_p += pred[item];
  }
    obs_p = ((double) cal_t) / (((double) (cal_t + cal_f)) + eps);
    cal_err = fabs(obs_p - (sum_p / (((double) (cal_t + cal_f)) + eps)));
    cal_sum += cal_err;
    cal_sum_n++;
  }
  return(cal_sum / ((double) cal_sum_n));
}

double calc_cal2(int cal_bin_size)
{
/* calculate mean absolute calibration error */
  int item, cal_sum_n, cal_f, cal_t;
  double cal_sum, sum_p, obs_p, cal_err;

  cal_sum = 0.0;
  cal_sum_n = 0;
  cal_f = 0;
  cal_t = 0;
  sum_p = 0.0;
  for (item=0; item<no_item; item++) {
    if ( true[item] == 1 )
      cal_t++;
    else
      cal_f++;
    sum_p += pred[item];
    if ( item >= (cal_bin_size - 1) ) {
      obs_p = ((double) cal_t) / (((double) (cal_t + cal_f)) + eps);
      cal_err = fabs(obs_p - (sum_p / (((double) (cal_t + cal_f)) + eps)));
      cal_sum += cal_err;
      cal_sum_n++;
      if ( true[item - cal_bin_size + 1] == 1 )
  cal_t--;
      else
  cal_f--;
      sum_p -= pred[item - cal_bin_size + 1];
    }
  }
  return(cal_sum / ((double) cal_sum_n));
}

void start_computations()
{
  /***** HERE WE START THE COMPUTATIONS *****/

  /* Note that some metrics (RMSE, Norm, Cross-Entropy, Slac-Q, Calibration)
     assume that the predicted values (and maybe also the targets) are scaled [0-1]. */

  targs_all_same = 0;
  mean_true = 0.0;
  mean_pred = 0.0;
  max_true = -9e99;
  min_true = +9e99;
  for (item=0; item<no_item; item++)
    {
      mean_true+= true[item];
      mean_pred+= pred[item];
      if ( true[item] < min_true )
  min_true = true[item];
      if ( true[item] > max_true )
  max_true = true[item];
    }
  mean_true = mean_true / ((double) no_item);
  mean_pred = mean_pred / ((double) no_item);

  if (debug) {
    printf("%d pats read. mean_true %6.4lf. mean_pred %6.4lf\n", no_item, mean_true, mean_pred);
    fflush(stdout);
  }

  for (item=0; item<no_item; item++)
    if ( true[item] > min_true && true[item] < max_true ) {
      fprintf(stderr, "Error: More than 2 unique target values detected.  Exiting.\n");
      exit(-1);
    }

  if ( min_true == max_true ) {
    targs_all_same = 1;
    if ( min_true <= 0.0 )
      assumed_true = 0.0;
    else
      assumed_true = 1.0;
    fprintf(stderr, "Warning: All targets in block have same value %8.4lf. Assuming targets are class %d\n", min_true, ((int) assumed_true));
  }

  not_max_warned = 1;
  not_min_warned = 1;
  total_true_0 = 0;
  total_true_1 = 0;

  for (item=0; item<no_item; item++)
    if ( targs_all_same == 1 ) {
      true[item] = assumed_true;
      if ( assumed_true == 1.0 )
  total_true_1++;
      else
  total_true_0++;
    }
    else {
      if ( true[item] < mean_true ) {
  true[item] = 0;
  total_true_0++;
      }
      else {
  true[item] = 1;
  total_true_1++;
      }
    }

  /* now sort data by predicted value */

  quicksort (0,(no_item-1));

  /* get the fractional weights. If there are ties we count the number
     of cases tied and how many positives there are, and we assign
     each case to be #poz/#cases positive and the rest negative */

  item = 0;
  while ( item < no_item )
    {
      begin = item;
      count = 1;
      if ( true[item] == 1 ) no_poz = 1;
      else no_poz = 0;
      while ( (item < no_item - 1) && (fabs( pred[item]- pred[item + 1]) < 1e-15) )
  {
    item++;
    count++;
    if ( true[item] == 1 ) no_poz++;
  }
      for ( i = begin; i <= item; ++i )
  fraction[i] = no_poz*1.0/count;
      item++;
    }

  /* do the accuracy plot, the cost plot and compute max acc thresh */

  tt = 0;
  tf = total_true_1;
  ft = 0;
  ff = total_true_0;
  /* place 1st threshold just above top predicted value */
  threshold = pred[no_item-1] + (pred[no_item-1] - pred[no_item-2]) / 2.0;
  acc = ((double)(tt+ff))/(((double)(no_item)) + eps);
  max_acc = acc;
  cost = cost_a*((double)tt) + cost_b*((double)tf) + cost_c*((double)ft) + cost_d*((double)ff);
  min_cost = cost;
  if ( acc_plot )
    printf ("%lf %lf\n", threshold, acc);
  if ( cost_plot )
    printf ("%lf %lf\n", threshold, cost);

  for (item=no_item-1; item>-1; item--)
    {
      tt+= true[item];
      tf-= true[item];
      ft+= 1 - true[item];
      ff-= 1 - true[item];
      if ( item > 0 )
  threshold = (pred[item] + pred[item-1]) / 2.0;
      else
        /* place last threshold just below bottom predicted value */
  threshold = pred[0] - (pred[1] - pred[0]) / 2.0;
      acc = ((double)(tt+ff))/(((double)(no_item)) + eps);
      cost = cost_a*((double)tt) + cost_b*((double)tf) + cost_c*((double)ft) + cost_d*((double)ff);
      if ( acc_plot )
  printf ("%lf %lf\n", threshold, acc);
      if ( cost_plot )
  printf ("%lf %lf\n", threshold, cost);
      if ( acc > max_acc )
  {
    max_acc = acc;
    max_acc_thresh = threshold;
    last_acc_thresh = threshold;
    max_acc_count = 1;
  }
      if ( (acc == max_acc) && (threshold != last_acc_thresh) )
  {
    max_acc_count++;
    last_acc_thresh = threshold;
  }
      if ( cost < min_cost )
  {
    min_cost = cost;
    min_cost_thresh = threshold;
    last_cost_thresh = threshold;
    min_cost_count = 1;
  }
      if ( (cost == min_cost) && (threshold != last_cost_thresh) )
  {
    min_cost_count++;
    last_cost_thresh = threshold;
  }
    }

  /*  find the prediction threshold such that the predicted number    */
  /*  of 0's and 1's matches the observed number of true 0's and 1's  */

  freq_thresh = (pred[total_true_0]+pred[total_true_0-1])/2.0;

  /* now update some statistics using the various thresholds */

  a = 0;
  b = 0;
  c = 0;
  d = 0;
  freq_a = 0;
  freq_b = 0;
  freq_c = 0;
  freq_d = 0;
  max_acc_a = 0;
  max_acc_b = 0;
  max_acc_c = 0;
  max_acc_d = 0;
  for (item=0; item<no_item; item++)
    {
      if ( true[item] == 1 )
  /* true outcome = 1 */
  {
    if ( pred[item] >= pred_thresh )
      a++;
    else
      b++;
    if ( pred[item] >= freq_thresh )
      freq_a++;
    else
      freq_b++;
    if ( pred[item] >= max_acc_thresh )
      max_acc_a++;
    else
      max_acc_b++;
  }
      else
  /* true outcome = 0 */
  {
    if ( pred[item] >= pred_thresh )
      c++;
    else
      d++;
    if ( pred[item] >= freq_thresh )
      freq_c++;
    else
      freq_d++;
    if ( pred[item] >= max_acc_thresh )
      max_acc_c++;
    else
      max_acc_d++;
  }
    }

  /* set the threshold to predict x% of the cases to be 1's */
  if (prcdata)
    {
      data_split = no_item - (int)(no_item*data_prc/100);
      if(data_split < 0)
  data_split = 0;
      else if (data_split >= no_item)
  data_split = no_item;
      a=b=c=d=0;
      for (item = no_item - 1; item >= data_split; --item)
  {
    a += fraction[item];
    c += 1 - fraction[item];
  }
      b = total_true_1 - a;
      d = total_true_0 - c;
    }

  if (roc_plot)              roc_area = calc_roca(1);
  if (SAR_flag && !roc_plot) roc_area = calc_roca(0);
  if (SAR_flag)              rmse = calc_rmse();
  if (pr_plot)               tmp_double = calc_apr(1);
  if (lift_plot)             calc_lift(1);

  max_lift = no_item*1.0 / (total_true_1 + eps);
  min_lift = 0.0;
}


int main (argc, argv)
     int  argc;
     char  *argv[];

{
  /* Used for -file and -files options */
  char *infile=NULL, *outfile=NULL;
  FILE *f_in=NULL, *f_out=NULL;

  int warned_user=0;

  roc_plot = 0;
  pr_plot = 0;
  lift_plot = 0;
  cost_plot = 0;
  acc_plot = 0;

  thresh = 0;
  pred_thresh = 0.5;

  SET_all = 0;
  SET_stats = 0;
  SET_easy = 0;

  ACC_flag = 0;
  APR_flag = 0;
  CAL_flag = 0;
  CST_flag = 0;
  CXE_flag = 0;
  LFT_flag = 0;
  NPV_flag = 0;
  NRM_flag = 0;
  PPV_flag = 0;
  PRB_flag = 0;
  PRF_flag = 0;
  PRE_flag = 0;
  REC_flag = 0;
  RMS_flag = 0;
  ROC_flag = 0;
  R50_flag = 0;
  SAR_flag = 0;
  SEN_flag = 0;
  SLQ_flag = 0;
  SPC_flag = 0;
  T01_flag = 0;
  T10_flag = 0;
  TOP_flag = 0;
  RKL_flag = 0;

  confusion = 0;
  costs = 0;
  prcdata = 0;
  loss = 0;
  plot = 0;

  allwacc = allwroc = allwrms = 1.0;
  slacq_bin_width = 0.01;
  no_bins = 100;
  cal_bin_size = 100;

  /***** PARAMETER PROCESSING *****/

  arg = 1;
  while ( arg < argc )
    {
      taken = 0;

      /* Accept flag sets the last variable to 1 if the option is set */

      taken += accept_flag(argv[arg], "-acc", &ACC_flag);
      taken += accept_flag(argv[arg], "-rms", &RMS_flag);

      taken += accept_flag(argv[arg], "-sen", &SEN_flag);
      taken += accept_flag(argv[arg], "-spc", &SPC_flag);
      taken += accept_flag(argv[arg], "-npv", &NPV_flag);
      taken += accept_flag(argv[arg], "-ppv", &PPV_flag);

      taken += accept_flag(argv[arg], "-roc", &ROC_flag);
      taken += accept_flag(argv[arg], "-auc", &ROC_flag);

      /* Area under ROC curve until we see 50 negative examples. Used in protein sequence matching evaluation. */
      taken += accept_flag(argv[arg], "-r50", &R50_flag);

      taken += accept_flag(argv[arg], "-pre", &PRE_flag);
      taken += accept_flag(argv[arg], "-rec", &REC_flag);
      taken += accept_flag(argv[arg], "-prf", &PRF_flag);
      taken += accept_flag(argv[arg], "-prb", &PRB_flag);
      taken += accept_flag(argv[arg], "-apr", &APR_flag);

      taken += accept_flag(argv[arg], "-cxe", &CXE_flag);
      taken += accept_flag(argv[arg], "-rkl", &RKL_flag);
      taken += accept_flag(argv[arg], "-lft", &LFT_flag);

      taken += accept_flag(argv[arg], "-top1", &T01_flag);
      taken += accept_flag(argv[arg], "-top10", &T10_flag);

      taken += accept_parameter(argv, &arg, "-ntop", &TOP_flag, &topn);

      /* accept_parameter sets 'norm' to the next value on the command line as well as setting the flag. */
      taken += accept_parameter(argv, &arg, "-nrm", &NRM_flag, &norm);
      taken += accept_parameter(argv, &arg, "-norm", &NRM_flag, &norm);

      if (accept_parameter(argv, &arg, "-percent", &prcdata, &data_prc)) {
  taken++;
  thresh = 0;
  if (data_prc<0||data_prc>100)
    {
      fprintf(stderr,"Error: The argument for -percent must be between 0 and 100.\n");
      fflush(stderr);
      exit(1);
    }
      }

      /* Accept_flag_ext allows anything after the option (eg break_even) */
      taken += accept_flag_ext(argv[arg], "-break", &PRB_flag);
      taken += accept_flag_ext(argv[arg], "-pr_break", &PRB_flag);
      taken += accept_flag_ext(argv[arg], "-prbreak", &PRB_flag);

      /* Selects which plot, custom option processing */
      if (accept_flag(argv[arg], "-plot", &plot)) {
  taken++;

  arg++;
  switch(argv[arg][0]) {
  case 'r': /* ROC plot */
  case 'R':
    roc_plot = 1;
    pr_plot = 0;
    lift_plot = 0;
    cost_plot = 0;
    acc_plot = 0;
    break;

  case 'p': /* Precision/recall plot */
  case 'P':
    roc_plot = 0;
    pr_plot = 1;
    lift_plot = 0;
    cost_plot = 0;
    acc_plot = 0;
    break;

  case 'l': /* Lift versus plot */
  case 'L':
    roc_plot = 0;
    pr_plot = 0;
    lift_plot = 1;
    cost_plot = 0;
    acc_plot = 0;
    break;

  case 'c': /* Cost versus threshold plot */
  case 'C':
    roc_plot = 0;
    pr_plot = 0;
    lift_plot = 0;
    cost_plot = 1;
    acc_plot = 0;
    break;

  case 'a': /* Accuracy versus threshold plot */
  case 'A':
    roc_plot = 0;
    pr_plot = 0;
    lift_plot = 0;
    cost_plot = 0;
    acc_plot = 1;
    break;
  }
      }

      if (accept_flag(argv[arg], "-easy", &SET_easy)) {
  taken++;
  ROC_flag = 1;
  ACC_flag = 1;
  RMS_flag = 1;
      }

      taken += accept_flag(argv[arg], "-conf", &confusion);
      taken += accept_flag(argv[arg], "-confusion", &confusion);
      taken += accept_flag(argv[arg], "-table", &confusion);

      if ( accept_flag(argv[arg], "-sar", &SAR_flag) ) {
  taken++;
  arg++;
  allwacc = atof(argv[arg]);
  arg++;
  allwroc = atof(argv[arg]);
  arg++;
  allwrms = atof(argv[arg]);
      }

      if (accept_flag(argv[arg], "-stats", &SET_stats)) {
  taken++;
  ACC_flag = 1; PPV_flag = 1; NPV_flag = 1; SEN_flag = 1; SPC_flag = 1;
  PRE_flag = 1; REC_flag = 1; PRF_flag = 1; LFT_flag = 1;
      }

      if (accept_flag(argv[arg], "-all", &SET_all)) {
  taken++;
  SET_stats = 1;
  SET_easy = 0;

  ACC_flag = 1;
  APR_flag = 1;
  CAL_flag = 1;
  CXE_flag = 1;
  LFT_flag = 1;
  NPV_flag = 1;
  PPV_flag = 1;
  PRB_flag = 1;
  PRF_flag = 1;
  PRE_flag = 1;
  REC_flag = 1;
  RMS_flag = 1;
  ROC_flag = 1;
  R50_flag = 1;
  SAR_flag = 1;
  SEN_flag = 1;
  SLQ_flag = 1;
  SPC_flag = 1;
  T01_flag = 1;
  T10_flag = 1;
  RKL_flag = 1;
      }

      if ( accept_parameter(argv, &arg, "-t", &thresh, &pred_thresh) ||
     accept_parameter(argv, &arg, "-thresh", &thresh, &pred_thresh) ||
     accept_parameter(argv, &arg, "-threshold", &thresh, &pred_thresh) )
  {
    taken++;
    prcdata = 0;
  }

      if (accept_flag(argv[arg], "-cost", &CST_flag)) {
  taken++;
  arg++;
  cost_a = atof(argv[arg]);
  arg++;
  cost_b = atof(argv[arg]);
  arg++;
  cost_c = atof(argv[arg]);
  arg++;
  cost_d = atof(argv[arg]);
      }

      if (accept_parameter(argv, &arg, "-slacq", &SLQ_flag, &slacq_bin_width) ||
    accept_parameter(argv, &arg, "-slq", &SLQ_flag, &slacq_bin_width)) {
  taken++;
  if (slacq_bin_width < 0) {
    fprintf(stderr, "Error: Slacq bin width must be positive\n");
    exit(1);
  }
  no_bins = floor(eps + 1.0/slacq_bin_width);
  if ( no_bins > MAX_BINS ) {
      printf("Error: SLAC_Q bin width too small: %8.4lf\n", slacq_bin_width);
      exit(1);
  }
      }

      if (accept_parameter(argv, &arg, "-cal", &CAL_flag, &tmp_double)) {
  taken++;
  cal_bin_size = floor(tmp_double);
      }

      taken += accept_flag(argv[arg], "-blocks", &BLOCKS_flag);

      if (accept_flag(argv[arg], "-file", &tmp_int)) {
  taken++;
  if (arg+1 >= argc) {
    fprintf(stderr, "Error: Must specify file name after -file\n");
    exit(1);
  }
  infile = argv[arg+1];
  /* Test if file is readable */
  if ((f_in = fopen(infile, "r")) == NULL) {
    fprintf(stderr, "Error: Can't open input file %s\n", infile);
    exit(1);
  }
  arg += 1;
      }

      if (accept_flag(argv[arg], "-files", &tmp_int)) {
  taken++;
  if (arg+2 >= argc) {
    fprintf(stderr, "Error: Must specify two file names after -files\n");
    exit(1);
  }
  infile = argv[arg+1];
  outfile = argv[arg+2];
  /* Test files are readable */
  if ((f_in = fopen(infile, "r")) == NULL) {
    fprintf(stderr, "Error: Can't open true values file %s\n", infile);
    exit(1);
  }
  if ((f_out = fopen(outfile, "r")) == NULL) {
    fprintf(stderr, "Error: Can't open predicted values file %s\n", outfile);
    exit(1);
  }
  arg += 2;
      }

      taken += accept_flag(argv[arg], "-loss", &loss);

      if (!taken) {
  printf("\nError: Unrecognized program option %s\n", argv[arg]);
  print_usage(argv[0]);
  exit(1);
      }

      arg++;
    } /* End of loop over arguments */

  /* If all flags are 0, do most of the basic stats */
  if ( ACC_flag + APR_flag + CAL_flag + CST_flag + CXE_flag + T01_flag + T10_flag + SAR_flag +
       LFT_flag + NPV_flag + NRM_flag + PPV_flag + PRB_flag + PRF_flag + PRE_flag + TOP_flag +
       RKL_flag + REC_flag + RMS_flag + ROC_flag + R50_flag + SEN_flag + SLQ_flag + SPC_flag == 0) {

    SET_all = 1;
    SET_stats = 1;
    SET_easy = 0;

    ACC_flag = 1;
    APR_flag = 1;
    CAL_flag = 1;
    CST_flag = 0;
    CXE_flag = 1;
    LFT_flag = 1;
    NPV_flag = 1;
    NRM_flag = 0;
    PPV_flag = 1;
    PRB_flag = 1;
    PRF_flag = 1;
    PRE_flag = 1;
    REC_flag = 1;
    RMS_flag = 1;
    ROC_flag = 1;
    R50_flag = 1;
    SAR_flag = 1;
    SEN_flag = 1;
    SLQ_flag = 1;
    SPC_flag = 1;
    T01_flag = 1;
    T10_flag = 1;
    TOP_flag = 0;
    RKL_flag = 1;
  }

  /***** LOAD THE DATA FILES OR READ DATA FROM STANDARD INPUT *****/

  if (BLOCKS_flag &&
      (!(APR_flag || RMS_flag || T01_flag || RKL_flag || ROC_flag)
      || ACC_flag || CAL_flag || CST_flag || CXE_flag || LFT_flag || NPV_flag
      || NRM_flag || PPV_flag || PRB_flag || PRF_flag || PRE_flag || REC_flag
      || R50_flag || SAR_flag || SEN_flag || SLQ_flag || SPC_flag || T10_flag
      || TOP_flag)) {
    fprintf(stderr, "\nError: -blocks only works with -APR, -RMS, -ROC, -RKL, -TOP1 options.\n");
    exit(-1);
  }

  no_item = 0;
  while (1)
    {
      if (BLOCKS_flag == 0) {
  if (infile == NULL) {
    /* Input is from standard input */
    if ((scanf("%f %f", &true[no_item], &pred[no_item])) == EOF)
      break;
  } else if (outfile == NULL) {
    /* Read input from one file */
    if ((fscanf(f_in, "%f %f", &true[no_item], &pred[no_item])) == EOF)
      break;
  } else {
    /* Else read input from two files */
    fscanf(f_in, "%f", &true[no_item]);
    fscanf(f_out, "%f", &pred[no_item]);
    if (feof(f_in)) {
      if (!feof(f_out))
        fprintf(stderr, "Warning: True values file is longer.\n");
      break;
    }
    if (feof(f_out)) {
      fprintf(stderr, "Warning: Predicted values file is longer.\n");
      break;
    }
  }
  /* If prediction is negative with metrics that assume [0-1], then warn user */
  if ( ((pred[no_item] < 0) || (pred[no_item] > 1)) && warned_user==0 &&
       (RMS_flag || CXE_flag || CAL_flag || NRM_flag || SLQ_flag )) {
    fprintf(stderr, "Warning: Prediction out of range [0-1] for RMS, CXE, NRM, SLQ, or CAL metrics.\n");
    warned_user = 1;
    if (SLQ_flag) {
      fprintf(stderr, "Can't calculate SLQ with predictions out of range [0-1].\n");
      SLQ_flag = 0;
    }
    if (CXE_flag) {
      fprintf(stderr, "Cross-entropy doesn't make sense with predictions out of range [0-1].\n");
      CXE_flag = 0;
    }
  }
  /* If prediction is negative and haven't warned user yet and threshold is default (this is
     a flag variable) and want a metric where this matters, then warn the user. */
  if (pred[no_item] < 0 && warned_user==0 && thresh==0 && prcdata==0 &&
      (ACC_flag || SEN_flag || SPC_flag || NPV_flag || PPV_flag || PRE_flag || REC_flag || PRF_flag )) {
    fprintf(stderr, "Warning: Negative prediction but threshold is at default of %f.\n", pred_thresh);
    warned_user = 1;
  }
      }
      else {
  if (infile == NULL) {
    /* Input is from standard input */
    if ((scanf("%d %f %f", &block[no_item], &btrue[no_item], &bpred[no_item])) == EOF)
      break;
  } else if (outfile == NULL) {
    /* Read input from one file */
    if ((fscanf(f_in, "%d %f %f", &block[no_item], &btrue[no_item], &bpred[no_item])) == EOF)
      break;
  } else {
    /* Else read input from two files */
    fscanf(f_in, "%d %f", &block[no_item], &btrue[no_item]);
    fscanf(f_out, "%f", &bpred[no_item]);
    if (feof(f_in)) {
      if (!feof(f_out))
        fprintf(stderr, "Warning: True values file is longer.\n");
      break;
    }
    if (feof(f_out)) {
      fprintf(stderr, "Warning: Predicted values file is longer.\n");
      break;
    }
  }
  if (block[no_item] == nnn) {
    fprintf(stderr, "Error: Can't use %d as a BLOCK ID.  Exiting.\n", nnn);
    exit(-1);
  }
  /* If prediction is negative with metrics that assume [0-1], then warn user */
  if ( ((bpred[no_item] < 0) || (bpred[no_item] > 1)) && warned_user==0 &&
       (RMS_flag || CXE_flag || CAL_flag || NRM_flag || SLQ_flag )) {
    fprintf(stderr, "Warning: Prediction out of range [0-1] for RMS, CXE, NRM, SLQ, or CAL metrics.\n");
    warned_user = 1;
    if (SLQ_flag) {
      fprintf(stderr, "Can't calculate SLQ with predictions out of range [0-1].\n");
      SLQ_flag = 0;
    }
    if (CXE_flag) {
      fprintf(stderr, "Cross-entropy doesn't make sense with predictions out of range [0-1].\n");
      CXE_flag = 0;
    }
  }
  /* If prediction is negative and haven't warned user yet and threshold is default (this is
     a flag variable) and want a metric where this matters, then warn the user. */
  if (bpred[no_item] < 0 && warned_user==0 && thresh==0 && prcdata==0 &&
      (ACC_flag || SEN_flag || SPC_flag || NPV_flag || PPV_flag || PRE_flag || REC_flag || PRF_flag )) {
    fprintf(stderr, "Warning: Negative prediction but threshold is at default of %f.\n", pred_thresh);
    warned_user = 1;
  }
      }
      no_item++;
      if ( no_item >= MAX_ITEMS )
  {
    printf ("Aborting.  Exceeded %d items.\n", MAX_ITEMS);
    exit(1);
  }
    } /* End of loading loop */


  if (BLOCKS_flag == 0)
    {
      start_computations();

      /***** MAIN OUTPUT SECTION *****/

      if ( (SET_stats || ROC_flag) && (roc_plot || acc_plot) )
  printf ("\n");

      if (debug) printf ("%f %f %f %f\n", a,b,c,d);

      if (prcdata)
  print_stats(a, b, c, d, "prc of data", data_prc);
      else
  print_stats(a, b, c, d, "pred_thresh", pred_thresh);

      if ( SET_stats || confusion ) {
  if ( confusion )
    {
      if ( prcdata )
        print_confusion_table (a,b,c,d,"Percent of data", data_prc);
      else
        print_confusion_table (a,b,c,d,"Threshold", pred_thresh);
    }
  printf ("\n");
  if (debug) printf ("%f %f %f %f\n", freq_a,freq_b,freq_c,freq_d);
  print_stats(freq_a, freq_b, freq_c, freq_d, "freq_thresh", freq_thresh);
  if ( confusion )
    print_confusion_table (freq_a,freq_b,freq_c,freq_d,"Threshold",freq_thresh);

  printf ("\n");
  if (debug) printf ("%f %f %f %f\n", max_acc_a,max_acc_b,max_acc_c,max_acc_d);
  if (max_acc_count > 1)
    printf ("Caution: %d Different Thresholds Achieved Max Accuracy.\nRun with -accplot option to see accuracy vs. threshold.\n\n", max_acc_count);
  print_stats(max_acc_a, max_acc_b, max_acc_c, max_acc_d, "max_acc_thresh", max_acc_thresh);
  if ( confusion )
    print_confusion_table (max_acc_a,max_acc_b,max_acc_c,max_acc_d,"threshold",max_acc_thresh);

  printf ("\n");
      }

      if ( loss == 0 ) {
  if (PRB_flag) printf("PRB   %8.5lf\n", calc_break_even_point());
  if (APR_flag) printf("APR   %8.5lf\n", calc_apr(0));
  if (ROC_flag) printf("ROC   %8.5lf\n", calc_roca(0));
  if (R50_flag) printf("R50   %8.5lf\n", calc_rocn(50));
  if (RKL_flag) printf("RKL    %d\n",    (int) calc_rank_last());
  if (T01_flag) printf("TOP1  %8.5lf\n", calc_top1());
  if (T10_flag) printf("TOP10 %8.5lf\n", calc_topn(10));
  if (TOP_flag) printf("NTOP%d  %8.5lf\n", ((int) (topn+0.01)), calc_top_sum(topn)/(topn+eps));
  if (SLQ_flag) printf("SLQ   %8.5lf Bin_Width %9.6lf\n", calc_slq_score(), slacq_bin_width);
      }
      else {
  if (PRB_flag) printf("PRB   %8.5lf\n", 1.0 - calc_break_even_point());
  if (APR_flag) printf("APR   %8.5lf\n", 1.0 - calc_apr(0));
  if (ROC_flag) printf("ROC   %8.5lf\n", 1.0 - calc_roca(0));
  if (R50_flag) printf("R50   %8.5lf\n", 1.0 - calc_rocn(50));
  if (RKL_flag) printf ("RKL    %8.5lf\n", ((calc_rank_last()-1) / (((double) (no_item-1)) + eps)));
  if (T01_flag) printf ("TOP1  %8.5lf\n", 1.0 - calc_top1());
  if (T10_flag) printf ("TOP10 %8.5lf\n", 1.0 - calc_topn(10));
  if (TOP_flag) printf ("NTOP%d  %8.5lf\n", (int) (topn+0.01), 1.0 - (calc_top_sum(topn)/(topn+eps)));
  if (SLQ_flag) printf ("SLQ   %8.5lf Bin_Width %9.6lf\n", 1.0-calc_slq_score(), slacq_bin_width);
      }

      if (CXE_flag) printf("CXE   %8.5lf\n", calc_mcxe());
      if (NRM_flag) printf("NRM   %8.5lf\n", calc_norm());
      if (RMS_flag) printf("RMS   %8.5lf\n", calc_rmse());
      if (CAL_flag) printf("CA1   %8.5lf 19_0.05_bins\n", calc_cal1());
      if (CAL_flag) printf("CA2   %8.5lf Bin_Size %d\n",  calc_cal2(cal_bin_size), cal_bin_size);
      if (costs)    printf("MIN_COST  %8.2lf Threshold %9.6lf\n", min_cost, min_cost_thresh);
    }
  else {
    mean_top1 = 0.0;
    mean_apr  = 0.0;
    mean_roc  = 0.0;
    mean_rkl  = 0.0;
    mean_rms  = 0.0;
    total_items = no_item;
    no_blocks = 0;
    more_blocks = 1;
    while (more_blocks) {
      cur_block = nnn;
      no_item = 0;
      for (bitem=0; bitem<total_items; bitem++) {
        if (block[bitem] != nnn) {
          if (cur_block == nnn)
            cur_block = block[bitem];
          if (block[bitem] == cur_block) {
            true[no_item] = btrue[bitem];
            pred[no_item] = bpred[bitem];
            block[bitem] = nnn;
            no_item++;
          }
        }
      }
      if (cur_block == nnn)
        more_blocks = 0;
      else {
        double d;
        no_blocks++;
        start_computations();
        if (T01_flag) {
          mean_top1 += d = calc_top1();
          stddev_top1 += d*d;
        }
        if (APR_flag) {
          mean_apr  += d = calc_apr(0);
          stddev_apr += d*d;
        }
        if (RKL_flag) {
          mean_rkl  += d = calc_rank_last();
          stddev_rkl+= d*d;
        }
        if (RMS_flag) {
          mean_rms  += d = calc_rmse();
          stddev_rms += d*d;
        }
        if (ROC_flag) {
          mean_roc  += d = calc_roca(0);
          stddev_roc += d*d;
        }
      }
    }
    if (APR_flag) printf("MEAN_BLOCK_APR    %8.5lf\tDEV %le\n", mean_apr/no_blocks, (stddev_apr - mean_apr*mean_apr / no_blocks) / no_blocks);
    if (RKL_flag) printf("MEAN_BLOCK_RKL    %8.5lf\tDEV %le\n", mean_rkl/no_blocks, (stddev_rkl - mean_rkl*mean_rkl / no_blocks) / no_blocks);
    if (RMS_flag) printf("MEAN_BLOCK_RMS    %8.5lf\tDEV %le\n", mean_rms/no_blocks, (stddev_rms - mean_rms*mean_rms / no_blocks) / no_blocks);
    if (ROC_flag) printf("MEAN_BLOCK_ROC    %8.5lf\tDEV %le\n", mean_roc/no_blocks, (stddev_roc - mean_roc*mean_roc / no_blocks) / no_blocks);
    if (T01_flag) printf("MEAN_BLOCK_TOP1   %8.5lf\tDEV %le\n", mean_top1/no_blocks, (stddev_top1 - mean_top1*mean_top1 / no_blocks) / no_blocks);
  }
  return 0;
}

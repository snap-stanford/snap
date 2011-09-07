#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define CATEGORICAL 1
//#define CONTINUOUS 12
#define CONTINUOUS 3
#define SAMPLES 100
#define MIN_POS 10
//#define PRUNE

double training_fraction;

typedef struct example {
  int targ;
  int features[CATEGORICAL + CONTINUOUS];
  int evals;
  double sum;
} example_t;

typedef struct split {
  int split_val;
  int split_pos;
  double ent;
  double chi;
} split_t;

//100000445910000 100000032993442 0 1 465857 217052 101201 47272 3690 1 1 1 1 30 180

example_t ** test_data;
example_t ** train_data;
example_t * examples;
int * already_split;
int N;
int data_size;
int F;

inline double sq(double d){
  return d*d;
}
inline double max(double a, double b){
  return a > b ? a : b;
}

/*compare examples by feature F*/
int compare_examples(const void * a, const void * b){
  example_t * e1 = *(example_t **)a;
  example_t * e2 = *(example_t **)b;
  return e1->features[F] - e2->features[F];
}

void add_example (char * buf) {
  int i, j, v = 0;
  if(N == data_size){
    data_size *= 1.5;
    printf("reallocing to %d\n",data_size);
    examples = realloc(examples, sizeof(example_t) * data_size);
  }
  for (i = j = 0; buf[i]; i++) {
    if (buf[i] <= ' ') {
      if (v != 0x80000000) {
        if(j == 0){
          examples[N].evals = 0;
          examples[N].sum = 0;
          examples[N].targ = !!v;
        } else if(j <= CATEGORICAL + CONTINUOUS) {
          examples[N].features[j - 1] = v;
        } else {
          fprintf(stderr,"too many features: %s\n",buf);
          exit(1);
        }
      }
      j++;
      v = 0x80000000;
    } else {
      if(v == 0x80000000)v = 0;
      v = v * 10 + buf[i] - '0';
    }
  }
  N++;
}

double entropy(unsigned int a, unsigned int b){
  if(a == 0 || a == b)return 0;
  double p = (double)a / (a+b);
  return - (a * log(p)) - (b-a) * log(1-p);
}

void leaf(int start, int end, int p, int t){
  int i;
  double d = (double)p/t;
  for(i = start; i < end; i++) {
    test_data[i]->evals++;
    test_data[i]->sum += d;
  }
}

double chi(int a, int b, int c, int d){
  int ab = a+b;
  int cd = c+d;
  int ac = a+c;
  int bd = b+d;
  int n = a+b+c+d;
  double ret = n * sq(max(0,abs(a*d - b*c) - n / 2.0)) / ((double)ab * cd * ac * bd);
  //printf("%d %d %d %d %lf\n",a,b,c,d,ret);
  /*
  if(ret < 0){
    printf("%d %d %d %d %lf\n",a,b,c,d,ret);
  }
  */
  return ret;
}

double categorical(int start, int end, int positives) {
  int i, j, v, pos;
  double ent = 0;
  v = train_data[start]->features[F];
  pos = train_data[start]->targ;
  j = start;
  for (i = start + 1; i<end; i++) {
    if (train_data[i]->features[F] != v) {
      //printf("%d %d %d %d %lf\n",F,pos,i-j,v,(double)pos/(i-j));
      ent += entropy(pos,i-j);
      pos = 0;
      v = train_data[i]->features[F];
      j = i;
    }
    pos += train_data[i]->targ;
  }
  //printf("%d %d %d %d %lf\n",F,pos,i-j,v,(double)pos/(i-j));
  return ent / (end - start);
}
split_t continuous(int start, int end, int positives) {
  int i, v, pos, bpos = -1, btot;
  double ent = 0;
  split_t ret;
  ret.ent = entropy(positives,end-start);
  v = train_data[start]->features[F];
  pos = train_data[start]->targ;
  for (i = start + 1; i<end; i++) {
    if (train_data[i]->features[F] != v) {
      ent = entropy(pos,i - start) + entropy(positives - pos,end - i);
      if(ent < ret.ent){
        ret.ent = ent;
        ret.split_val = train_data[i]->features[F];
        ret.split_pos = i;
        bpos = pos;
        btot = i-start;
      }
      v = train_data[i]->features[F];
    }
    pos += train_data[i]->targ;
  }
#ifdef PRUNE
  if(bpos != -1){
    ret.chi = chi(bpos, btot, positives - bpos, end-start-btot);
  } else {
    ret.chi = 0;
  }
#endif
  //printf("chi = %lf, split_pos = %d\n", ret.chi, ret.split_pos);
  ret.ent = ret.ent / (end - start);
  return ret;
}

void train(int start, int end, int test_start, int test_end, int dep){
  int i, j, k;
  double best_split_ent;
  double d, ent;
  double chiv;
  int split_val, split_pos;
  int split_feat = -1;
  int positives = 0;
  int pts;
  split_t sp;
  for(i = start; i<end; i++){
    positives += train_data[i]->targ;
  }
  for(i = 0; i<dep; i++)printf("  ");
  printf("train %d %d @ %d: %d/%d %lf -- test %d %d -- %d\n",start,end,dep,positives,end-start,(double)positives/(end-start),test_start,test_end,train_data[start]->features[0]);
  ent = best_split_ent = entropy(positives, end-start) / (end - start);
  if(positives <= MIN_POS || positives == end - start) {
    leaf(test_start, test_end, positives, end-start);
    return;
  }
  for(i = 0; i<CATEGORICAL + CONTINUOUS; i++) {
    F = i;
    qsort(&train_data[start], end - start, sizeof(example_t*), compare_examples);
    if (i < CATEGORICAL) {
      if(already_split[i]) {//not going to get anything
        d = ent;
      } else {
        d = categorical(start, end, positives);
      }
    } else {
      sp = continuous(start, end, positives);
      if(sp.chi > 3) {
        d = sp.ent;
      }else{
        d = ent;
      }
    }
    //printf("%d %lf %lf %le\n",i,d,ent,ent-d);
    if (d < best_split_ent) {
      best_split_ent = d;
      split_feat = i;
      if (i >= CATEGORICAL) {
        chiv = sp.chi;
        split_val = sp.split_val;
        split_pos = sp.split_pos;
      }
    }
  }
  if (split_feat == -1) {
    leaf(test_start, test_end, positives, end-start);
    return;
  }
  F = split_feat;
  qsort(&train_data[start], end - start, sizeof(example_t*), compare_examples);
  qsort(&test_data[test_start], test_end - test_start, sizeof(example_t*), compare_examples);
  for(i = 0; i<dep; i++)printf("  ");
  printf("at dep = %d, best split = %d, gain = %le, chi = %lf\n",dep,split_feat,ent-best_split_ent, chiv);
  if(split_feat < CATEGORICAL) {
    already_split[split_feat] = 1;
    j = start;
    k = train_data[start]->features[split_feat];
    pts = test_start;
    for(i = start+1; i<end; i++){
      if(train_data[i]->features[split_feat] != k) {
        while(test_start < test_end && test_data[test_start]->features[split_feat] <= k) {
          test_start++;
        }
        train(j,i,pts, test_start,dep+1);
        pts = test_start;
        j = i;
        k = train_data[i]->features[split_feat];
      }
    }
    //we're not checking for unknown categories here, but oh well
    train(j,i, test_start, test_end, dep+1);
    already_split[split_feat] = 0;
  } else {
    //printf("best split = %d, gain = %le, at %d -- %d (%d/%d)\n",split_feat,ent-best_split_ent, split_val, split_pos, split_pos - start, end-start);
    int lo = test_start, hi = test_end, m;
    while(lo + 1 < hi){
      m = (lo+hi)/2;
      if(test_data[m]->features[split_feat] < split_val){
        lo = m;
      } else {
        hi = m;
      }
    }
    //printf("%d %d %d %d\n",hi, split_val, test_data[hi-1]->features[split_feat], test_data[hi]->features[split_feat]);
    train(start, split_pos, test_start, hi, dep+1);
    train(split_pos, end , hi, test_end, dep+1);
  }
}

int main(int argc, char ** argv){
  FILE * data_file = fopen(argv[1],"r");
  char buf[1000];
  int i, c;
  setvbuf(stdout, NULL, _IONBF, 0);
  data_size = 1000000;
  examples = malloc(sizeof(example_t) * data_size);
  sscanf(argv[3],"%lf",&training_fraction);
  while(fgets(buf,999,data_file)){
    add_example(buf);
  }
  printf("N = %d\n",N);
  train_data = malloc(sizeof(example_t*) * N);
  test_data = malloc(sizeof(example_t*) * N);
  for(c = 0; c < SAMPLES; c++){
    int train_cnt = 0, test_cnt = 0;
    for(i = 0; i<N; i++){
      if((double)(rand() & 0x3fffffff) / 0x40000000 < training_fraction) {
        train_data[train_cnt++] = &examples[i];
      } else {
        test_data[test_cnt++] = &examples[i];
      }
    }
    already_split = malloc(sizeof(int) * (CATEGORICAL + CONTINUOUS));
    memset(already_split,0,sizeof(int) * (CATEGORICAL + CONTINUOUS));
    train(0,train_cnt, 0, test_cnt, 0);
    FILE * output = fopen(argv[2],"w");
    for(i = 0; i<N; i++){
      if(examples[i].evals){
        fprintf(output, "%d %lf %d\n",examples[i].targ, examples[i].sum / examples[i].evals, examples[i].evals);
      }
    }
    fclose(output);
  }
  return 0;
}



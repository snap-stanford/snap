# Speeding Up BigClam Implementation on SNAP


C.H. Bryan Liu, Benjamin Paul Chamberlain

We perform a detailed analysis of the C++ implementation of the Cluster Affiliation Model for Big Networks (BigClam) on the Stanford Network Analysis Project (SNAP). BigClam is a popular graph mining algorithm that is capable of finding overlapping communities in networks containing millions of nodes. Our analysis shows a key stage of the algorithm - determining if a node belongs to a community - dominates the runtime of the implementation, yet the computation is not parallelized. We show that by parallelizing computations across multiple threads using OpenMP we can speed up the algorithm by 5.3 times when solving large networks for communities, while preserving the integrity of the program and the result.


## Setup

We provide a script to load the SNAP network datasets:

```
./load_data.sh
```

The script requires ~600MB of network traffic and ~1700MB of disk space. 

To get the total number of community affiliations in each network,
which is used to calculate r in Table 2, run the following:

```
wc -w ./data/*.all.cmty.txt
```


## Running the code

Note: While the code is tested to be compilable and running on MacOSX and Linux machines. The effect of parallel computing can only be seen on a Linux machine on our side. We therefore **recommend you to run the following commands on a Linux machine to verify our claims**. 

First compile the code (you need to have `make` and `g++` installed):

```
make all
```

The command builds three executables: 

1. `./bigclam` (with CA stage parallelised)
2. `./bigclam_old` (_without_ CA stage parallelised), and
3. `./cmtycompre` (the utility for community equality comparison)


To compare the runtime difference between the unparallelized and 
parallelized versions, run the following - it is ordered by increasing runtime required:

```
# DBLP collaboration network
./bigclam -o:./data/dblp_ -i:./data/com-dblp.ungraph.txt -c:13477
./bigclam_old -o:./data/dblp_old_ -i:./data/com-dblp.ungraph.txt -c:13477

# Amazon product co-purchase network
./bigclam -o:./data/amazon_ -i:./data/com-amazon.ungraph.txt -c:75149
./bigclam_old -o:./data/amazon_old_ -i:./data/com-amazon.ungraph.txt -c:75149

# Youtube social network
./bigclam -o:./data/youtube_ -i:./data/com-youtube.ungraph.txt -c:8385
./bigclam_old -o:./data/youtube_old_ -i:./data/com-youtube.ungraph.txt -c:8385

# LiveJournal social network (very long runtime!)
./bigclam -o:./data/lj_ -i:./data/com-lj.ungraph.txt -c:287512
./bigclam_old -o:./data/lj_old_ -i:./data/com-lj.ungraph.txt -c:287512
```

The seemingly hard-coded number of communities is the number of ground truth communities (see http://snap.stanford.edu/data).

To show the parallelized version produced the same community memberships
(which are not necessarily in the same order), run:

```
./cmtycompare -i1:./data/dblp_cmtyvv.txt -i2:./data/dblp_old_cmtyvv.txt
./cmtycompare -i1:./data/amazon_cmtyvv.txt -i2:./data/amazon_old_cmtyvv.txt
./cmtycompare -i1:./data/youtube_cmtyvv.txt -i2:./data/youtube_old_cmtyvv.txt
./cmtycompare -i1:./data/lj_cmtyvv.txt -i2:./data/lj_old_cmtyvv.txt
```

You can remove the object files and executables by _either_ doing the shallow clean:

```
make clean
```

... or a deep clean, which also remove the object files and executables in the main SNAP library:

```
make deepclean
```


## Using Our Experiment Data

You can find the data generated in our experiments in the `experiment_data` directory. 

The file `parallelize_runtime.csv` contains data generated in the experiment described in Section 5.1 of the paper (showing parallelizing the CA stage speeds up the implementation), and the file `parallelize_speedup_limit_test.csv` contains data generated in the experiment described in Section 5.3 of the paper (exploring the limit in speed up with parallel computing).

We also have a number of R scripts / Jupyter Notebook generating the figures and table featured in the paper. We will shortly update this file with a handy dependency installation script, though the dependencies are more or less the standard ones (R: ggplot2 and reshape2, Python: Numpy, Pandas, Matplotlib, Scipy, and Jupyter).

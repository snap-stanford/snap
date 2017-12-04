# Speeding Up BigClam Implementation on SNAP

C.H. Bryan Liu, Benjamin Paul Chamberlain
----------------------------------------------------------

We perform a detailed analysis of the C++ implementation of the Cluster Affiliation Model for Big Networks (BigClam) on the Stanford Network Analysis Project (SNAP). BigClam is a popular graph mining algorithm that is capable of finding overlapping communities in networks containing millions of nodes. Our analysis shows a key stage of the algorithm - determining if a node belongs to a community - dominates the runtime of the implementation, yet the computation is not parallelized. We show that by parallelizing computations across multiple threads using OpenMP we can speed up the algorithm by 5.3 times when solving large networks for communities, while preserving the integrity of the program and the result.

## Setup

We provide a script to load the SNAP network datasets:

```
./load_data.sh
```

To get the total number of community affiliations in each network,
which is used to calculate r in Table 2, run the following:

```
wc -w ./data/*.all.cmty.txt
```

## Running the code

Note: While the implementation is tested to be running on both MacOSX and Linux, the effect of parallelization is currently only seen on a Linux machine. We therefore *recommend you to compile and run the program on a Linux machine to verify our claims*.

First compile the code:

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

The seemingly hard-coded number of communities are the number of ground truth communities (see http://snap.stanford.edu/data). You can also ignore the compilation warnings.


To show the parallelized version produced the same community memberships
(which are not necessarily in the same order), run:

```
./cmtycompare -i1:dblp_cmtyvv.txt -i2:dblp_old_cmtyvv.txt
./cmtycompare -i1:amazon_cmtyvv.txt -i2:amazon_old_cmtyvv.txt
./cmtycompare -i1:youtube_cmtyvv.txt -i2:youtube_old_cmtyvv.txt
./cmtycompare -i1:youtbe_cmtyvv.txt -i2:youtube_old_cmtyvv.txt
```



# Speeding Up BigClam Implementation on SNAP


C. H. Bryan Liu, Benjamin Paul Chamberlain

We perform a detailed analysis of the C++ implementation of the Cluster Affiliation Model for Big Networks (BigClam) on the Stanford Network Analysis Project (SNAP). BigClam is a popular graph mining algorithm that is capable of finding overlapping communities in networks containing millions of nodes. Our analysis shows a key stage of the algorithm - determining if a node belongs to a community - dominates the runtime of the implementation, yet the computation is not parallelized. We show that by parallelizing computations across multiple threads using OpenMP we can speed up the algorithm by 5.3 times when solving large networks for communities, while preserving the integrity of the program and the result.

The results are reported in our [pre-print hosted on arXiv](https://arxiv.org/pdf/1712.01209v1.pdf). The figure/table numbers referred in this README corresponds to **version 1** of the pre-print:
```
@misc{liu2018speeding,
      title={Speeding Up BigClam Implementation on SNAP}, 
      author={C. H. Bryan Liu and Benjamin Paul Chamberlain},
      year={2018},
      eprint={1712.01209},
      archivePrefix={arXiv},
      primaryClass={cs.SI}
}
```

Part of the pre-print is also presented in [2018 Imperial College Computing Student Workshop (ICCSW 2018)](https://drops.dagstuhl.de/opus/volltexte/2019/10182/pdf/OASIcs-ICCSW-2018-1.pdf):
```
@InProceedings{liu_et_al:OASIcs:2019:10182,
  author =	{C. H. Bryan Liu and Benjamin Paul Chamberlain},
  title =	{{Speeding Up BigClam Implementation on SNAP}},
  booktitle =	{2018 Imperial College Computing Student Workshop (ICCSW 2018)},
  pages =	{1:1--1:13},
  series =	{OpenAccess Series in Informatics (OASIcs)},
  ISBN =	{978-3-95977-097-2},
  ISSN =	{2190-6807},
  year =	{2019},
  volume =	{66},
  editor =	{Edoardo Pirovano and Eva Graversen},
  publisher =	{Schloss Dagstuhl--Leibniz-Zentrum fuer Informatik},
  address =	{Dagstuhl, Germany},
  URL =		{http://drops.dagstuhl.de/opus/volltexte/2019/10182},
  URN =		{urn:nbn:de:0030-drops-101829},
  doi =		{10.4230/OASIcs.ICCSW.2018.1},
  annote =	{Keywords: BigClam, Community Detection, Parallelization, Networks}
}
```


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


## Running the C++ code

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

# LiveJournal social network (very long runtime - at least a day!)
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
To ensure the `cmtycompare` utility is working properly, you can manually manipulate (add, delete, or swap the node IDs between communities) the community associations in the files and check the utility does indeed notifying you that the communities are different with some diagnostics.

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

# Speeding Up BigClam Implementation on SNAP


C. H. Bryan Liu, Benjamin Paul Chamberlain

We perform a detailed analysis of the C++ implementation of the Cluster Affiliation Model for Big Networks (BigClam) on the Stanford Network Analysis Project (SNAP). BigClam is a popular graph mining algorithm that is capable of finding overlapping communities in networks containing millions of nodes. Our analysis shows a key stage of the algorithm - determining if a node belongs to a community - dominates the runtime of the implementation, yet the computation is not parallelized. We show that by parallelizing computations across multiple threads using OpenMP we can speed up the algorithm by 5.3 times when solving large networks for communities, while preserving the integrity of the program and the result.

The results are reported in our [pre-print hosted on arXiv](https://arxiv.org/pdf/1712.01209v1.pdf). The figure/table numbers referred in this README corresponds to **version 1** of the pre-print. Part of the pre-print is also presented in [2018 Imperial College Computing Student Workshop (ICCSW 2018)](https://drops.dagstuhl.de/opus/volltexte/2019/10182/pdf/OASIcs-ICCSW-2018-1.pdf).

If you find the results useful, we will be grateful if you can cite the workshop paper:
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

**Note:** The code is tested to run on Linux and MacOSX systems. Although if you have a MacOSX and uses the default `clang` that comes with Xcode, OpenMP is explicitly disabled while we build the executable (as it is unsupported), and thus you will not see any wall clock time gain due to parallelisation for the entire BigClam implementation. 
We therefore **recommend you to run the following commands on a Linux machine to verify our claims**. 

To use a `clang` compiler that supports OpenMP, see [this Q&A thread](https://stackoverflow.com/questions/43555410/enable-openmp-support-in-clang-in-mac-os-x-sierra-mojave). After following what's suggested in the thread, you probably also need to symlink your `g++` to the new `clang++`, and/or change `Makefile.config` to point to the new `clang++` and add relevant compiler flags.

First compile the code (you need to have `make` and `g++` installed), there will be some complier warnings which can be ignored:
.
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


## Using Our Experiment Data in R & Python

You can find the code for and data generated in our experiments in the `experiment_data` directory:

* The file `parallelize_runtime.csv` contains data generated in the experiment described in Section 5.1 of the pre-print / Section 4.1 of the workshop paper (showing parallelizing the CA stage speeds up the implementation);
* The file `parallelize_speedup_limit_test.csv` contains data generated in the experiment described in Section 5.3 of the pre-print (exploring the limit in speed up with parallel computing);
* The file `figure_3_proportion_time_spent.R` contains the R script that generates Figure 3 in the pre-print / Figure 2 in the workshop paper (which has a different legend placement);
* The file `figure_6_box_plot.R` contains the R script that generates Figure 6 in the pre-print / Figure 4 in the workshop paper;
* The file `figure_7_extra_speedup_limit.ipynb` contains the Jupyter notebook (Python) that generates Figure 7 in the pre-print; and
* The file `table_3.R` contains the R script that produces Table 3 in the pre-print and the workshop paper and its associated t-test results.

### Running the R Scripts

The R scripts should only require a R environment to be installed---each of the scripts will automatically install the dependencies (`ggplot2` and `reshape`) if they are not already installed.

### Running the Jupyter notebook

To run the notebook you would need `numpy`, `pandas`, `matplotlib`, `scipy`, `jupyter`, and their dependencies.

We included a `Pipfile` and `Pipfile.lock` that specifies the dependencies above if you are running `pipenv`. To use the files:

1. Ensure you have `gcc`, `make`, and `pip` installed---if you can run the C++ code above the first two should already be there.

2. Install `pyenv`:
  * For Linux (together with other required libraries):
    ```bash
    sudo apt-get install -y make build-essential libssl-dev zlib1g-dev libbz2-dev \
    libreadline-dev libsqlite3-dev wget curl llvm libncurses5-dev libncursesw5-dev \
    xz-utils tk-dev libffi-dev liblzma-dev python-openssl git
    wget -L https://github.com/pyenv/pyenv-installer/raw/master/bin/pyenv-installer | bash
     
    chmod u+x pyenv-installer
    ./pyenv-installer
    ```
  
  * For Mac OS X:
    ```bash
    brew install pyenv
    ```

3. Configure the system PATHs for `pyenv`:
  ```bash
  export PATH="$HOME/.pyenv/bin:$PATH"
  eval "$(pyenv init -)"
  eval "$(pyenv virtualenv-init -)"
  ```

4. Install the right Python version for our project:
  ```bash
  pyenv install 3.7.3
  ```

5. Install `pipenv` using `pip` or `pip3`:
  ```bash
  pip install -U pipenv
  ```

6. Change directory to the same level as this README (i.e. `./ICL-bigclam_speedup/`) and sync the environment:
  ```bash
  # Exact cd target depends on where you downloaded the repo to
  cd ./ICL-bigclam_speedup
  
  # Switch to Python 3.7.3 for pyenv
  pyenv local 3.7.3
  pipenv update --dev
  ```

7. Run `jupyter notebook` on the environment you have just set up:
  ```bash
  pipenv run jupyter notebook
  ```


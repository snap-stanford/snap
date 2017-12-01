#!/bin/bash

# Load the graph files
curl https://snap.stanford.edu/data/bigdata/communities/com-amazon.ungraph.txt.gz > ./data/com-amazon.ungraph.txt.gz
gunzip ./data/com-amazon.ungraph.txt.gz --force

curl https://snap.stanford.edu/data/bigdata/communities/com-dblp.ungraph.txt.gz > ./data/com-dblp.ungraph.txt.gz
gunzip ./data/com-dblp.ungraph.txt.gz --force

curl https://snap.stanford.edu/data/bigdata/communities/com-lj.ungraph.txt.gz > ./data/com-lj.ungraph.txt.gz
gunzip ./data/com-lj.ungraph.txt.gz --force

curl https://snap.stanford.edu/data/bigdata/communities/com-youtube.ungraph.txt.gz > ./data/com-youtube.ungraph.txt.gz
gunzip ./data/com-youtube.ungraph.txt.gz --force

# Load the community files
curl https://snap.stanford.edu/data/bigdata/communities/com-amazon.all.dedup.cmty.txt.gz > ./data/com-amazon.all.cmty.txt.gz
gunzip ./data/com-amazon.all.cmty.txt.gz --force

curl https://snap.stanford.edu/data/bigdata/communities/com-dblp.all.cmty.txt.gz > ./data/com-dblp.all.cmty.txt.gz
gunzip ./data/com-dblp.all.cmty.txt.gz --force

curl https://snap.stanford.edu/data/bigdata/communities/com-lj.all.cmty.txt.gz > ./data/com-lj.all.cmty.txt.gz
gunzip ./data/com-lj.all.cmty.txt.gz --force

curl https://snap.stanford.edu/data/bigdata/communities/com-youtube.all.cmty.txt.gz > ./data/com-youtube.all.cmty.txt.gz
gunzip ./data/com-youtube.all.cmty.txt.gz --force



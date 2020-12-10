#!/bin/bash

# Load the graph files
echo "Loading Amazon product co-purchasing network graph..."
curl -k https://snap.stanford.edu/data/bigdata/communities/com-amazon.ungraph.txt.gz > ./data/com-amazon.ungraph.txt.gz
gunzip ./data/com-amazon.ungraph.txt.gz --force

echo "Loading DBLP collaboration network graph..."
curl -k https://snap.stanford.edu/data/bigdata/communities/com-dblp.ungraph.txt.gz > ./data/com-dblp.ungraph.txt.gz
gunzip ./data/com-dblp.ungraph.txt.gz --force

echo "Loading LiveJournal social network graph..."
curl -k https://snap.stanford.edu/data/bigdata/communities/com-lj.ungraph.txt.gz > ./data/com-lj.ungraph.txt.gz
gunzip ./data/com-lj.ungraph.txt.gz --force

echo "Loading Youtube social network graph..."
curl -k https://snap.stanford.edu/data/bigdata/communities/com-youtube.ungraph.txt.gz > ./data/com-youtube.ungraph.txt.gz
gunzip ./data/com-youtube.ungraph.txt.gz --force

# Load the community files
echo "Loading Amazon product co-purchasing network ground-truth communities..."
curl -k https://snap.stanford.edu/data/bigdata/communities/com-amazon.all.dedup.cmty.txt.gz > ./data/com-amazon.all.cmty.txt.gz
gunzip ./data/com-amazon.all.cmty.txt.gz --force

echo "Loading DBLP collaboration network ground-truth communities..."
curl -k https://snap.stanford.edu/data/bigdata/communities/com-dblp.all.cmty.txt.gz > ./data/com-dblp.all.cmty.txt.gz
gunzip ./data/com-dblp.all.cmty.txt.gz --force

echo "Loading LiveJournal social network ground-truth communities..."
curl -k https://snap.stanford.edu/data/bigdata/communities/com-lj.all.cmty.txt.gz > ./data/com-lj.all.cmty.txt.gz
gunzip ./data/com-lj.all.cmty.txt.gz --force

echo "Loading Youtube social network ground-truth communities..."
curl -k https://snap.stanford.edu/data/bigdata/communities/com-youtube.all.cmty.txt.gz > ./data/com-youtube.all.cmty.txt.gz
gunzip ./data/com-youtube.all.cmty.txt.gz --force

echo "Loading Friendster social network ground-truth communities..."
curl -k https://snap.stanford.edu/data/bigdata/communities/com-friendster.all.cmty.txt.gz > ./data/com-friendster.all.cmty.txt.gz
gunzip ./data/com-friendster.all.cmty.txt.gz --force

echo "Loading Orkut social network ground-truth communities..."
curl -k https://snap.stanford.edu/data/bigdata/communities/com-orkut.all.cmty.txt.gz > ./data/com-orkut.all.cmty.txt.gz
gunzip ./data/com-orkut.all.cmty.txt.gz --force

echo "Loading complete. They can be found in ./data/"


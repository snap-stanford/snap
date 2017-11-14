#!/bin/bash

curl https://snap.stanford.edu/data/bigdata/communities/com-amazon.ungraph.txt.gz > ./data/com-amazon.ungraph.txt.gz
gunzip ./data/com-amazon.ungraph.txt.gz --force

curl https://snap.stanford.edu/data/bigdata/communities/com-dblp.ungraph.txt.gz > ./data/com-dblp.ungraph.txt.gz
gunzip ./data/com-dblp.ungraph.txt.gz --force



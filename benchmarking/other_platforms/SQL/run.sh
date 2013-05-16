#!/bin/bash
sizes=( 10 30 100 300 1000 3000 10000 )
for i in "${sizes[@]}"
do
  echo "Building graphs with input size $i (thousands of rows)"
  sed "s/SIZEPARAM/$i/" query_test.sql | mysql -u mraison -p mraison
done


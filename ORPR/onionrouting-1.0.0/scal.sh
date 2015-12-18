#!/bin/bash
NB_NODES="100 200 300 400 500 600 700 800 900 1000"
#NB_NODES="129 257 513 1025"
WIT="2 3 5 7 10"
#WIT="2"
for w in ${WIT}
do
  for n in ${NB_NODES}
  do
    #echo "NB_NODES = " $n >> scal_test$w.txt
    ./do_exp.sh $w 0 $n
    ./compute_stat.sh $n >> scal_test$w.txt
    ./compute_logSize.sh $n >> scal_test$w.txt
    echo "======" >> scal_test$w.txt
  done
done



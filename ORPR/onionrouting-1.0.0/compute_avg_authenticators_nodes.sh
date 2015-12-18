#!/bin/bash
NB_NODES="100 200 300 400 500 600 700 800 900 1000"
#NB_NODES="129 257 513 1025"
WIT="3"
R=40
#WIT="2"
for w in ${WIT}
do
  for n in ${NB_NODES}
  do
    #echo "NB_NODES = " $n >> scal_test$w.txt
    ./do_exp.sh $n $w 0 $R
    ./avg_authenticators.sh $n >> authenticators$w-$R.txt
  done
done



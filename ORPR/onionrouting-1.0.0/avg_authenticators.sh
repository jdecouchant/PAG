#!/bin/bash
NB_NODES=$1 #nb nodes
#WIT=$2   #nb monitors
AVG_AUTH=0
for i in `seq 0 $(($NB_NODES-1))`
do
   #echo "i = " $i
   a=$(($i/254))
   b=$((1+$i%254))
   #echo "a = " $a
   #echo "b = " $b
   grep "nbCons" sim-10.0.$a.$b.log > tmpAuth.txt
   nbAuth=$(tail -1 tmpAuth.txt | awk '{print $7}')
   AVG_AUTH=$(($AVG_AUTH + $nbAuth))
done
AUTH=$(($AVG_AUTH/$NB_NODES))
echo "Avg authenticators = " $AUTH


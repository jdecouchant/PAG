#!/bin/bash
WIT_SET="2 3 4 5"
NB_NODES=50
NB_RELAYS=5
for wit in ${WIT_SET}
do
    ./do_exp.sh $NB_NODES $wit 0 $NB_RELAYS
    ./compute_stat.sh $NB_NODES $wit  >> res50n-traf-log.txt
    ./compute_logSize.sh $NB_NODES >> res50n-traf-log.txt 
done


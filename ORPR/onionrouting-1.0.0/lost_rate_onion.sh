#!/bin/bash
NB_NODES=$1  # nombre de noeuds
nbSENT=0
nbRCVD=0
for i in `seq 0 $(($NB_NODES-1))`
do
   a=$(($i/254))
   b=$((1+$i%254))
   grep "nbSent" sim-10.0.$a.$b.log > tmpS.txt
   grep "nbRcvd" sim-10.0.$a.$b.log > tmpR.txt

   nbS=$(tail -1 tmpS.txt | awk '{print $4}') 
   nbR=$(tail -1 tmpR.txt | awk '{print $4}')
   if [ -z $nbS ]
   then
       nbS=0
   fi
   
   if [ -z $nbR ]
   then
       nbR=0
   fi	
   nbSENT=$(($nbSENT + $nbS))   
   nbRCVD=$(($nbRCVD + $nbR))
done;
echo "======================="
echo "Number of nodes = " $NB_NODES
echo "sent_onions = " $nbSENT
echo "received_onions = " $nbRCVD
rm tmp*

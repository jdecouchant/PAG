#!/bin/bash
NB_NODES=$1  # nombre de noeuds
ONION_SIZE=10010  # en bytes
ELAPSED_TIME=99 # en secondes
AVG_Traffic=0
AVG_Rcvd=0
AVG_PAYLOAD=0

for i in `seq 0 $(($NB_NODES-1))`
do
   a=$(($i/254))
   b=$((1+$i%254))
   grep "nbSent" sim-10.0.$a.$b.log > tmpS.txt
   grep "nbFwd" sim-10.0.$a.$b.log > tmpF.txt
   grep "nbRcvd" sim-10.0.$a.$b.log > tmpR.txt
   
   nbS=$(tail -1 tmpS.txt | awk '{print $4}')
   nbF=$(tail -1 tmpF.txt | awk '{print $4}')
   nbR=$(tail -1 tmpR.txt | awk '{print $4}')
   #if [ -z "$nbF" ]
   #then
   #    echo "node$i"
   #fi
   THROUGHPUT_PER_NODE=$((($nbR*$ONION_SIZE*8)/($ELAPSED_TIME*1000))) #kbps
   TRAFFIC_PER_NODE=$((($nbF*$ONION_SIZE*8)/($ELAPSED_TIME*1000)))

   AVG_Rcvd=$(($AVG_Rcvd + $THROUGHPUT_PER_NODE))
   AVG_Traffic=$(($AVG_Traffic + $TRAFFIC_PER_NODE))
done;
echo "======================="
echo "Number of nodes = " $NB_NODES
TRAFFIC_SENT=$(($AVG_Traffic/$NB_NODES)) # en kbps
THROUGHPUT=$(($AVG_Rcvd/($NB_NODES))) # en kbps
echo "Traffic_sent = " $TRAFFIC_SENT "kbps"
echo "Throughput = " $THROUGHPUT "kb/s"
rm tmp*

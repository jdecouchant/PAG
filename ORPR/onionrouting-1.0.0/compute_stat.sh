#!/bin/bash

if [ $# -lt 3 ]; then
    echo "Usage: $0 nbNodes nbWit elapsedTime"
    exit 0
fi


NB_NODES=$1  # nombre de noeuds
#NB_RATIONALS=$2 # nombre de noeuds rationnels
WIT=$2
ELAPSED_TIME=$3 # total time - warmup time
CHUNK_SIZE=10000  # en bytes
#ELAPSED_TIME=199 # en seconds
CBR=300 # constant bit rate en kbps
AVG_F=0
AVG_R=0
AVG_B=0
AVG_COMMIT=0
AVG_CHAL=0
AVG_RESP=0
AVG_CR=0
AVG_PAYLOAD=0
AVG_OTHER=0
AVG_CONS=0
AVG_TRANS=0

echo "" > accOnionRouting.txt

for i in `seq 0 $(($NB_NODES-1))`
do
   a=$(($i/254))
   b=$((1+$i%254))

   echo "$a $b" 
   grep "nbSent" sim-10.0.$a.$b.log > tmpS.txt
   grep "nbFwd" sim-10.0.$a.$b.log > tmpF.txt
   grep "nbRcvd" sim-10.0.$a.$b.log > tmpR.txt
   grep "nbR" sim-10.0.$a.$b.log > tmp.txt
   grep "TIME" sim-10.0.$a.$b.log > time.txt
   grep "nbBytesSent" sim-10.0.$a.$b.log > tmpB.txt
   grep "commitBytes" sim-10.0.$a.$b.log > tmpCommit.txt
   grep "chalBytes" sim-10.0.$a.$b.log > tmpChal.txt
   grep "respBytes" sim-10.0.$a.$b.log > tmpResp.txt
   grep "otherBytes" sim-10.0.$a.$b.log > tmpOther.txt
   grep "payloadBytes" sim-10.0.$a.$b.log > tmpPayload.txt
   grep "consistencyBytes" sim-10.0.$a.$b.log > tmpConsistency.txt
   grep "transmitted_logs" sim-10.0.$a.$b.log > tmpTransmit.txt   
   
   
   nbS=$(tail -1 tmpS.txt | awk '{print $4}')
   nbF=$(tail -1 tmpF.txt | awk '{print $4}')
   nbR=$(tail -1 tmpR.txt | awk '{print $4}')
   nb=$(tail -1 tmp.txt | awk '{print $4}')
   nbB=$(tail -1 tmpB.txt | awk '{print $4}')
   nbCoB=$(tail -1 tmpCommit.txt | awk '{print $4}')
   nbchB=$(tail -1 tmpChal.txt | awk '{print $4}')
   nbreB=$(tail -1 tmpResp.txt | awk '{print $4}')
   #nbOB=$(tail -1 tmpOther.txt | awk '{print $4}')
   #echo "node"$i 
   nbPB=$(tail -1 tmpPayload.txt | awk '{print $4}')
   nbConsB=$(tail -1 tmpConsistency.txt | awk '{print $4}')
   nbTransB=$(tail -1 tmpTransmit.txt | awk '{print $4}')
  
   #THROUGHPUT_PER_NODE=$((($nbR*$CHUNK_SIZE*8)/($ELAPSED_TIME*1000))) #kbps
   TRAFFIC_PER_NODE=$(($nbB*8/($ELAPSED_TIME*1000)))
   #echo "node"$i  $TRAFFIC_PER_NODE 
   #TRAFFIC_PER_NODE=$(($nbB*8/($ELAPSED_TIME*1000)))
   #TRAFFIC_PER_NODE_CoB=$(($nbCoB*8/($ELAPSED_TIME*1000)))
   #TRAFFIC_PER_NODE_chB=$(($nbchB*8/($ELAPSED_TIME*1000)))
   #TRAFFIC_PER_NODE_reB=$(($nbreB*8/($ELAPSED_TIME*1000)))
   TRAFFIC_PER_NODE_cr=$((($nbchB+$nbreB)*8/($ELAPSED_TIME*1000)))
   #TRAFFIC_PER_NODE_OB=$(($nbOB*8/($ELAPSED_TIME*1000)))
   TRAFFIC_PER_NODE_PB=$(($nbPB*8/($ELAPSED_TIME*1000)))
   TRAFFIC_PER_NODE_Cons=$(($nbConsB*8/($ELAPSED_TIME*1000)))
   TRAFFIC_PER_NODE_Trans=$(($nbTransB*8/($ELAPSED_TIME*1000)))

  ##CONTENT_RCVD_PER_NODE=$(($THROUGHPUT_PER_NODE*100/$CBR))
  #AVG_F=$(($AVG_F + $nbF*$CHUNK_SIZE*8)) #nombre de bits
  ##AVG_R=$(($AVG_R + $nbR*$CHUNK_SIZE*8)) #nombre de bits
  #AVG_R=$(($AVG_R + $THROUGHPUT_PER_NODE))
  AVG_B=$(($AVG_B + $TRAFFIC_PER_NODE))
  echo $TRAFFIC_PER_NODE >> accOnionRouting.txt

# #AVG_COMMIT=$((AVG_COMMIT + $nbCB))
# AVG_COMMIT=$((AVG_COMMIT + $TRAFFIC_PER_NODE_CoB))
#
# #AVG_CHAL=$((AVG_CHAL + $nbchB))
# AVG_CHAL=$(($AVG_CHAL + $TRAFFIC_PER_NODE_chB))
# AVG_RESP=$(($AVG_RESP + $TRAFFIC_PER_NODE_reB))
  AVG_CR=$(($AVG_CR + $TRAFFIC_PER_NODE_cr))
# #AVG_PAYLOAD=$((AVG_PAYLOAD + $nbPB))
  AVG_PAYLOAD=$(($AVG_PAYLOAD + $TRAFFIC_PER_NODE_PB))
#
  #AVG_OTHER=$((AVG_OTHER + $TRAFFIC_PER_NODE_OB))
# #AVG_CONS=$((AVG_CONS + $nbConsB))
  AVG_CONS=$(($AVG_CONS + $TRAFFIC_PER_NODE_Cons))
  AVG_TRANS=$(($AVG_TRANS + $TRAFFIC_PER_NODE_Trans)) 
   #echo "node"$i $THROUGHPUT_PER_NODE "kbps" $CONTENT_RCVD_PER_NODE "%" 
   #echo "node"$i $TRAFFIC_PER_NODE "kbps" $THROUGHPUT_PER_NODE "kb/s" $nbS 
   #echo "node"$i $THROUGHPUT_PER_NODE  
   #echo "node"$i  $nbS $nbF $nbR $nb
done;
echo "======================="
echo "Number of nodes = " $NB_NODES
echo "Number of witnesses = " $WIT
#echo "Number of rational nodes = " $NB_RATIONALS
#FORWARD=$(($AVG_F/($NB_NODES*$ELAPSED_TIME*1000))) # en kbps
PAYLOAD=$(($AVG_PAYLOAD/$NB_NODES)) # en kbps
#TRAFFIC_COMMIT=$(($AVG_COMMIT/$NB_NODES))
#TRAFFIC_CHAL=$(($AVG_CHAL/$NB_NODES))
#TRAFFIC_RESP=$(($AVG_RESP/$NB_NODES))
TRAFFIC_CR=$(($AVG_CR/$NB_NODES))
TRAFFIC_CONS=$(($AVG_CONS/$NB_NODES))
TRAFFIC_TRANS=$(($AVG_TRANS/$NB_NODES))
#TRAFFIC_OTHER=$(($AVG_OTHER/$NB_NODES))
TRAFFIC_SENT=$(($AVG_B/$NB_NODES)) # en kbps
#THROUGHPUT=$(($AVG_R/($NB_NODES))) # en kbps
##CONTENT_RCVD=$(($DEBIT*100/$CBR))
#echo "Number of nodes = " $1
echo "Payload = " $PAYLOAD "kbps"
#echo "Forward = " $FORWARD "kbps"
#echo "Commitment = " $TRAFFIC_COMMIT "kbps"
##echo "challenge_traffic = " $TRAFFIC_CHAL "bps"
#echo "Auditing = " $(($TRAFFIC_CHAL+$TRAFFIC_RESP)) "kbps"
echo "Consistency = " $TRAFFIC_CONS "kbps"
echo "Auditing = " $TRAFFIC_CR  "kbps"
DELTA=$(($TRAFFIC_CONS + $TRAFFIC_CR - $TRAFFIC_TRANS))
echo "Log_traffic = " $TRAFFIC_TRANS "kbps"
#echo "other_traffic = " $TRAFFIC_OTHER "kbps"
echo "Global_traffic = " $TRAFFIC_SENT "kbps"
echo "delta = " $DELTA "kbps"
echo "PR_traffic" $(($TRAFFIC_SENT - $PAYLOAD)) "kbps"
echo "FR_traffic" $(($TRAFFIC_SENT - $PAYLOAD + $DELTA)) "kbps"

##echo "Payload = " $PAYLOAD "kbps"
#echo "Throughput = " $THROUGHPUT "kb/s"
#echo $1 $THROUGHPUT
#echo "DEBIT = " $DEBIT "kbps"
#echo "Content_rcvd = " $CONTENT_RCVD "%" 
rm tmp*

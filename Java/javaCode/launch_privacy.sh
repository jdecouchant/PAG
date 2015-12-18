#!/bin/bash

RTE=10
FANOUT_CLI=3
FANOUT_SVR=3
UPDATE_LEN=938
SESSION_DURATION=300

NBNODES=40

mkdir -p results-cofree/
DIR="./results-cofree/$RTE.$FANOUT_CLI.$FANOUT_SVR.$SESSION_DURATION.$NBNODES"
mkdir -p $DIR

rm $DIR/*

# WARNING: Must be removed for g5K
echo "Creating config files"
echo "localhost" > ./nodes.txt
PORT=54000
./add_port_to_nodes.py $NBNODES ./nodes.txt ./nodes_and_ports.txt $PORT

echo "Killing the processes that run on targeted ports"
for i in $(seq $PORT $(($PORT+$NBNODES))) 
do
   fuser -k -n tcp $i
done
#sleep 30

echo "Running experiment"
# launch X instances of CoFree (one is the server, others are peers) 
for id in $(seq 0 $NBNODES)
do
   #xterm -e  java -jar ./jar/DeployColluders.jar $id $RTE $FANOUT_CLI $FANOUT_SVR $PERIOD $EPOCH $SCENARIO $SESSION_DURATION ./scripts/nodes_and_ports.txt &
   #java -jar ./jar/DeployColluders.jar $id $RTE $FANOUT_CLI $FANOUT_SVR $PERIOD $EPOCH $SCENARIO $SESSION_DURATION $PROBA_CHURN ./scripts/nodes_and_ports.txt > /dev/null &
   java -jar ./jar/DeployPrivacy.jar $id $RTE $FANOUT_CLI $FANOUT_SVR $UPDATE_LEN $SESSION_DURATION ./nodes_and_ports.txt > /dev/null &
done

echo "Sleeping during experiment"
x=0
while [ $x -le $SESSION_DURATION ]
do
   echo "$x / $SESSION_DURATION s..."
   sleep 10
   x=$(( $x + 10 ))
done

echo "Sleeping a little more"
sleep 10

echo "Saving the results in $DIR"
mv *nbDeleted* $DIR/
mv *SentUpdates* $DIR/
mv *logSize* $DIR/
mv *Bandwidth* $DIR/


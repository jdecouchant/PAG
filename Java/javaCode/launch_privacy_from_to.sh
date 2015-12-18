#!/bin/bash

if [ $# -ne 7 ]; then
   echo "Usage: ID RTE FANOUT_CLI FANOUT_SVR UPDATE_LEN SESSION_DURATION NBNODES_PER_HOST" 
   echo "1:$1, 2:$2, 3:$3, 4:$4, 5:$5, 6:$6, 7:$7"   
   exit
fi

ID=$1
RTE=$2
FANOUT_CLI=$3
FANOUT_SVR=$4
UPDATE_LEN=$5
SESSION_DURATION=$6
NBNODES=$7

ROOT_DIRECTORY=$(dirname $0)

for i in $(seq 1 $NBNODES)
do
   #java -jar "${ROOT_DIRECTORY}"/jar/DeployColluders.jar $ID $RTE $FANOUT_CLI $FANOUT_SVR $PERIOD $EPOCH $SCENARIO $SESSION_DURATION "${ROOT_DIRECTORY}"/scripts/nodes_and_ports.txt > /dev/null &
 
   java -jar "${ROOT_DIRECTORY}"/jar/DeployPrivacy.jar $ID $RTE $FANOUT_CLI $FANOUT_SVR $UPDATE_LEN $SESSION_DURATION "${ROOT_DIRECTORY}"/nodes_and_ports.txt &> ${ROOT_DIRECTORY}/node_$(hostname)_${ID}.log &
   #java -jar "${ROOT_DIRECTORY}"/jar/DeployColluders.jar $ID $RTE $FANOUT_CLI $FANOUT_SVR $PERIOD $EPOCH $SCENARIO $SESSION_DURATION "${ROOT_DIRECTORY}"/scripts/nodes_and_ports.txt &
   ID=$(($ID + 1))
done

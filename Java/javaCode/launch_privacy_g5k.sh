#!/bin/bash

source util.sh

RTE=10
FANOUT_CLI=3
FANOUT_SVR=3
UPDATE_LEN=938
SESSION_DURATION=60

NBNODES_PER_HOST=10
NBHOST=$(cat ${ROOT_DIRECTORY}/nodes.txt | wc -l)
NBNODES=$(( $NBNODES_PER_HOST * $NBHOST ))

DIR="./$RTE.$FANOUT_CLI.$FANOUT_SVR.$SESSION_DURATION.$NBNODES"
mkdir -p $DIR

rm $DIR/*

#oarprint host > ~/nodes.txt

PORT=54000
./add_port_to_nodes.py $NBNODES_PER_HOST ${ROOT_DIRECTORY}/nodes.txt ${ROOT_DIRECTORY}/nodes_and_ports.txt $PORT

echo "Killing the processes that run on targeted ports on all machines"
while read ligne; do
   for i in $(seq $PORT $(($PORT+$NBNODES_PER_HOST))) #Too large but whatever...
   do
      ssh -n ${LOGIN}@$ligne "fuser -k -n tcp $i"
   done
done < ${ROOT_DIRECTORY}/nodes.txt

./clean_results.sh

echo "Starting all processes"
ID=0
while read ligne; do
echo $ligne
# launch X instances of CoFree (one is the server, others are peers) 
   ssh -n ${LOGIN}@$ligne "${ROOT_DIRECTORY}/launch_privacy_from_to.sh $ID $RTE $FANOUT_CLI $FANOUT_SVR $UPDATE_LEN $SESSION_DURATION $NBNODES_PER_HOST"
   ID=$(( $ID + $NBNODES_PER_HOST ))
done < ${ROOT_DIRECTORY}/nodes.txt

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
mv ~/0_nbrSentUpdates.txt $DIR/
mv ~/*nbDeleted* $DIR/
mv ~/*logSize* $DIR/
mv ~/*Bandwidth* $DIR/
mv ~/*stats* $DIR/


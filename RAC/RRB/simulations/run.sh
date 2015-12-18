#!/bin/bash

#CONFIGS="one two three four five six seven eight nine ten eleven twelve thirteen fourteen fifteen sixteen seventeen eighteen nineteen twenty twenty_one twenty_two twenty_three twenty_four twenty_five twenty_six twenty_seven twenty_eight"
#CONFIGS="fifteen sixteen seventeen eighteen nineteen twenty twenty_one twenty_two twenty_three twenty_four twenty_five twenty_six twenty_seven twenty_eight"
#CONFIGS="ten twelve fourteen sixteen eighteen twenty twenty_two twenty_four twenty_six twenty_eight"
CONFIGS="one two three four five"
#CONFIGS="twenty_eight"
INI_FILES="RRBproto-0.ini RRBproto-1.ini RRBproto-2.ini"
#INI_FILES="RRBproto-1.ini RRBproto-2.ini"
if [ -z $INET_PATH ]
then
	echo "Please set \$INET_PATH and make it point to your inet root directory"
	exit 1
fi
if [ -z $SRC_PATH ]
then
	echo "Please set \$SRC_PATH"
	exit 1
fi

#SRC_PATH=/Users/amadoudiarra/Desktop/Phd/Dev/RR-AC/Simulation/RRB/src
make -C $SRC_PATH msgheaders
make -C $SRC_PATH

for FILE in ${INI_FILES}
do
  for CONFIG_INI in ${CONFIGS} 
  do

  ../src/RRB -u Cmdenv -l $INET_PATH/src/inet -n $INET_PATH/src:./:../src  -f ${FILE} -c ${CONFIG_INI} 
  
  done

done
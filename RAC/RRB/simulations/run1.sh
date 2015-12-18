#!/bin/bash

CONFIGS="one two three four five six seven eight nine ten eleven"

SRC_PATH="~/sim/amadou/RRB/src"

if [ -z $INET_PATH ]
then
	echo "Please set \$INET_PATH and make it point to your inet root directory"
	exit 1
fi
if [ -z $SRC_PATH ]
then
	echo "Please set \$SRC_PATH "
	exit 1
fi

make -C $SRC_PATH msgheaders
make -C $SRC_PATH

  for CONFIG_INI in ${CONFIGS} 
  do

  ../src/RRB -u Cmdenv -l $INET_PATH/src/inet -n $INET_PATH/src:./:../src  -f RRBproto1.ini -c ${CONFIG_INI}

  done

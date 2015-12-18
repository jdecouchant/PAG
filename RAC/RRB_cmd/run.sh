#!/bin/bash
if [ -z $INET_PATH ]
then
	echo "Please set \$INET_PATH and make it point to your inet root directory"
	exit 1
fi

LIBINET_SO="$INET_PATH/src/libinet.so"
SRC_PATH="src/"

if [ -z $MODE ]
then
	if [ -f "$LIBINET_SO" ]
	then
		LINKNAME=`readlink $LIBINET_SO`
		if [ "`echo "$LINKNAME" | grep -c "release"`" == "1" ]
		then
			export MODE=release
		else
			export MODE=debug
		fi
	else
		export MODE=debug
	fi
fi
echo "MODE = $MODE"

make -C $SRC_PATH msgheaders
make -C $SRC_PATH

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$INET_PATH/src
if [ -z $1 ]; then
	CONFIG_INI="General"
else
	CONFIG_INI="$1"
fi
./src/RRB -u Cmdenv -l $INET_PATH/src/inet -n $INET_PATH/src:./:./src -f RRBproto.ini -c $CONFIG_INI
#cat OutputSimu.txt | grep "\[Clax\]"  | sed 's/\[Clax\]//g'

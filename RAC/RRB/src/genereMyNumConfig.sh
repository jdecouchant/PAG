#!/bin/bash

max="$1"
for i in $(seq 0 $max)
do
	#echo "**.client[$i].tcpApp[0].myNum = $i"
	echo "router[$i].pppg++<--> RC <--> router[$i+1].pppg++;"
done

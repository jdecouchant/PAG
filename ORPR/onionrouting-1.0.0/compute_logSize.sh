#!/bin/bash
DIR=peerreview-onionrouting-$1-rational
n=0
total=0
data=0
logsize_sender=0
data_sender=0
for i in $DIR/node*
do
  if [ -d "$i"/ ]
  then
     index_size=$(du $i/local.index | awk '{print $1}')
     data_size=$(du -k $i/local.data | awk '{print $1}')
     data=$(($data+$data_size))
     #echo $index_size  $data_size
     log_size=$(($index_size+$data_size))
     total=$(($total+$log_size))
     if [ $n -eq 0 ]
     then
        data_sender=$data
        logsize_sender=$log_size
     fi
     n=$(($n+1))
     #echo node$n  $log_size
     #echo "log_size = " $log_size
  fi
done
#echo "n = " $n
#echo "logsize_sender = " $logsize_sender
#echo "data_sender = " $data_sender
size=$(($total/$n))
size1=$((($data-$data_sender)/($n-1)))
echo "logSize = " $size "kbps"
#echo "dataSize = " $size1

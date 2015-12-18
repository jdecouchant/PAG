#!/bin/bash
#rm tmp*
PERIODS="10000000 20000000 30000000 40000000 50000000 60000000 70000000 80000000 90000000 100000000"
#PERIODS="10000000"
for p in ${PERIODS}
do
   AVG_NBRCVD=0
   n=0
   for i in `seq 1 100`
   do
     grep nbRcvd sim-10.0.0.$i.log > tmp.txt
     start=$(head -1 tmp.txt | awk '{print $1}')
     end=$(($start+$p))
     awk '{ if($1 <= '"$end"') print NR }' tmp.txt >> tmp1.txt
     val=$(tail -1 tmp1.txt)
     AVG_NBRCVD=$(($val + $AVG_NBRCVD))
     n=$(($n+1))
     rm tmp*
   done
  RCVD=$(($AVG_NBRCVD/100))
  T=$(($p/1000000))
  #echo $T  $RCVD >> latence.txt
  echo $RCVD  
done

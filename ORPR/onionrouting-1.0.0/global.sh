#!/bin/bash
WIT_SIZE="3 4 5"
NUM_RATIONALS="10 20 30 40 50"
for w in ${WIT_SIZE}
do
   echo $w "wit" >> resultat.txt
   for i in ${NUM_RATIONALS}
   do
     ./do_exp.sh $w $i 100
      echo $i"% of rationals"
     ./compute_stat.sh 100 >> resultat.txt 
   done
   echo "====" >> resultat.txt
done


#!/bin/bash
./do_exp.sh $1 $2
grep multicastMisbehavior multicast-100-rational.param > essai.txt
grep rational=true multicast-100-rational.param > essai2.txt

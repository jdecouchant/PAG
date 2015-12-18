#!/bin/sh

cd ~/sim/Fprivacycrypto/src
make clean
make

cd ~/sim/Fprivacycrypto/simulations

mkdir -p updateLen

#../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_1updates -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

#../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_3updates -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

#../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_5updates -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

#../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_10updates -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;

#../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_20updates -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

#../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_30updates -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

#../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_40updates -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_50updates -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_60updates -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_70updates -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_80updates -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_90updates -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_100updates -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;

mv *.dat updateLen


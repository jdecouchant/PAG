#!/bin/sh

cd ~/sim/Fprivacycrypto/src
make clean
make

cd ~/sim/Fprivacycrypto/simulations

mkdir -p audioStreams

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_audio64 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_audio128 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_audio500 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;

mv *.dat audioStreams

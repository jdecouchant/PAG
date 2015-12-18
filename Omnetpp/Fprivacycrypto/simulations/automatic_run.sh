#!/bin/sh

cd ~/sim/Fprivacycrypto/src
make clean
make

cd ~/sim/Fprivacycrypto/simulations

mkdir -p 1session
mkdir -p 2sessions
mkdir -p 3sessions
mkdir -p 4sessions
mkdir -p videoQuality
mkdir -p scalability

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f3_w3_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini

exit

# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f4_w4_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f5_w5_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f6_w6_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f7_w7_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# 
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f3_w3_80 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f4_w4_80 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f5_w5_80 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f6_w6_80 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f7_w7_80 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# 
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f3_w3_120 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f4_w4_120 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f5_w5_120 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f6_w6_120 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f7_w7_120 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# 
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f3_w3_160 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f4_w4_160 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f5_w5_160 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f6_w6_160 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f7_w7_160 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;
# 

# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f3_w3_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f4_w3_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f5_w3_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f6_w3_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;
# 
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f7_w3_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f3_w5_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f4_w5_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f5_w5_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;
# 
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f6_w5_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f7_w5_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f3_w4_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f4_w4_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;
# 
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f5_w4_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f6_w4_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_f7_w4_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;

# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _250nodes_f2_w3_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
# 
# ../src/Fprivacycrypto -r 0 -u Cmdenv -c _250nodes_f2_w5_40 -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;

#mv *UPDNB.40.dat 1session/
# mv *UPDNB.80.dat 2sessions/
# mv *UPDNB.120.dat 3sessions/
# mv *UPDNB.160.dat 4sessions/

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_144p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_240p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_360p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_480p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_720p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w3_1080p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;


#mv *.dat videoQuality

exit

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w5_144p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &


 
 ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w5_240p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;
 





../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w5_360p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &



../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w5_480p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;

mv *.dat videoQuality



../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w5_720p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &


 
../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w5_1080p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;

mv *.dat videoQuality


../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w4_144p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w6_144p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w4_240p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
 
 ../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w6_240p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;
 
mv *.dat videoQuality


../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w4_360p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w6_360p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w4_480p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w6_480p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;

mv *.dat videoQuality

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w4_720p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w6_720p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w4_1080p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &
 
../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w6_1080p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;

mv *.dat videoQuality

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w7_144p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w7_240p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w7_360p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w7_480p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w7_720p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini > /dev/null &

../src/Fprivacycrypto -r 0 -u Cmdenv -c _1000nodes_w7_1080p -n .:../src:../../inet/examples:../../inet/src -l ../../inet/src/inet CBR_advanced.ini ;

mv *.dat videoQuality

#!/bin/bash

rm -f multicast.param
witness1="XXXWITNESS0XXX"
witness2="XXXWITNESS1XXX"

echo "Creating directories and configuration for multicast experiment..."

mkdir -p ce

for i in `seq -w 1 100`; do 
  dirname=peerreview/node$i
  ident=`grep Subject\: ca/node0$i\_cert.pem |grep -o CN=.*\/|grep -o [0-9a-f]\\\{40\\\}|tr [:lower:] [:upper:]`
  cp ca/node0$i\_cert.pem ce/$ident-cert.pem
done  

for i in `seq -w 1 100`; do 
  dirname=peerreview/node0$i
  mkdir -p $dirname
  cp ca/cacert.pem $dirname
  cp ca/node0$i\_cert.pem $dirname/nodecert.pem
  openssl rsa -passin pass:monkey -in ca/node0$i\_key.pem -out $dirname/nodekey.pem 2>/dev/null

  peername=peerreview/node0$i/peers
  mkdir -p $peername
  cp ce/* $peername
  
  storagename=peerreview/node0$i/storage
  mkdir -p $storagename
  
  ident=`grep Subject\: $dirname/nodecert.pem |grep -o CN=.*\/|grep -o [0-9a-f]\\\{40\\\}|tr [:lower:] [:upper:]`
  ip=`echo $i|bc`
  
  echo [$ident/10.0.0.$ip] >>multicast.param
  echo directory=peerreview/node0$i >>multicast.param
  echo trace=node0$i.trace >>multicast.param
  echo storageDir=peerreview/node0$i/storage >>multicast.param
  echo witnesses=2 >>multicast.param
  echo witness1=$witness1 >>multicast.param
  echo witness2=$witness2 >>multicast.param
  echo "" >>multicast.param
  witness1=$witness2
  witness2=$ident/10.0.0.$ip
done

cat multicast.param |sed s/XXXWITNESS0XXX/`echo $witness1 | sed s/\\\//\\\\\\\\\\\//`/ >mc2
cat mc2 |sed s/XXXWITNESS1XXX/`echo $witness2 | sed s/\\\//\\\\\\\\\\\//`/ >multicast.param
rm -f mc2
rm -rf ce

echo "Done."

#!/bin/bash
#Usage: ./do_exp.sh nbNodes nbWit nbRat
#You can declare the DEBUG variable with value VALGRIND (resp. GDB)
#to run an experiment with valgrind (resp. gdb)

if [ $# -lt 3 ]; then
   echo "Usage: $0 nbNodes nbWit nbRat nbRelais"
   echo "Global variables: DEBUG=VALGRIND or DEBUG=GDB to use valgrind or gdb"
   exit 0
fi

NB_NODES=$1
NB_WITNESS=$2
NB_RATIONALS=$3
NB_RELAIS=$4

./clean.sh
./cleanup.sh

#make clean
#make
# Generation de certificat pour les noeuds
#../transports-1.0.2-OR/makeca -t rsa -b 1024 $NB_NODES
#./makecerts.sh
# Creation du fichier de configuration pour l'expérience

./makeexp rational $NB_NODES $NB_WITNESS $NB_RATIONALS $NB_RELAIS
./makedirs.sh

# Lancer l'expérience
#PROG="./onionrouting-direct-sim"
PROG="./onionrouting-sim"
ARGS="onionrouting-$NB_NODES-rational.param"
${PROG} ${ARGS}
#if [ -z $DEBUG ]; then
#   ${PROG} ${ARGS} 2>&1
#elif [ "$DEBUG" = "GDB" ]; then
#   echo "run ${ARGS}" > /tmp/batch.gdb
#   echo "bt" >> /tmp/batch.gdb
#   echo "quit" >> /tmp/batch.gdb
#   gdb -x /tmp/batch.gdb ${PROG} 2>&1
#elif [ "$DEBUG" = "VALGRIND" ];then
#   valgrind --tool=memcheck --leak-check=full --show-reachable=yes --log-file=valgrind_log_${NB_NODES}nodes.log ${PROG} ${ARGS} 2>&1
#else
#   echo "Unknown $DEBUG"
#fi

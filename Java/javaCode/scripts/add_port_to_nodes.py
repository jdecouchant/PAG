#!/usr/bin/python
import sys

nb_args = len(sys.argv)
args = sys.argv

nb_clients_per_host = int(sys.argv[1])
fin = open(sys.argv[2], 'r')
fout = open(sys.argv[3], 'w')

for line in fin:
    line = line.strip('\n')
    port = 62000

    for i in range(0,nb_clients_per_host+1):
       fout.write(line + '\n')
       fout.write(str(port) + '\n')
       port += 1
    
fin.close()
fout.close()

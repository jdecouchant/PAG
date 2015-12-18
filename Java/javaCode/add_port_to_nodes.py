#!/usr/bin/python
import sys

nb_args = len(sys.argv)
args = sys.argv

if len(sys.argv) != 5:
   print "Usage: ", sys.argv[0], " nbClientsPerHost InFile OutFile startPortNbr" 
   sys.exit(1)
   
# The first node is also affected the source of the stream 

nb_clients_per_host = int(sys.argv[1])
fin = open(sys.argv[2], 'r')
fout = open(sys.argv[3], 'w')

first = True

for line in fin:
   line = line.strip('\n')
   port = int(sys.argv[4])
      
   for i in range(0,nb_clients_per_host):
      fout.write(line + '\n')
      fout.write(str(port) + '\n')
      port += 1
                        
      if first:
         first = False
         fout.write(line + '\n')
         fout.write(str(port) + '\n')
         port += 1

fin.close()
fout.close()

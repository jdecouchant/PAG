## Process this file with automake to produce Makefile.in

EXTRA_DIST=  	   depcomp scripts/makedirs-esm.sh README
AUTOMAKE_OPTIONS = foreign subdir-objects 1.5
                  
bin_PROGRAMS = onionrouting-sim onionrouting-inet onionrouting-direct-sim onionrouting-direct-inet makeexp

onionrouting_sim_SOURCES = src/main-sim.cc src/onionrouting.cc src/onionrouting-sender.cc include/onionrouting.h src/Cryptographer.cc include/onionrouting.h include/Cryptographer.h
onionrouting_sim_LDADD = -lpeerreview -lparameters -lsimtransport -lx509 -lcrypto

onionrouting_inet_SOURCES = src/main-inet.cc src/onionrouting.cc src/onionrouting-sender.cc src/Cryptographer.cc include/onionrouting.h include/Cryptographer.h
onionrouting_inet_LDADD = -lpeerreview -lparameters -linettransport -lx509 -lcrypto

onionrouting_direct_sim_SOURCES = src/main-direct-sim.cc src/onionrouting.cc src/onionrouting-sender.cc src/Cryptographer.cc include/onionrouting.h include/Cryptographer.h
onionrouting_direct_sim_LDADD = -lparameters -lsimtransport -lx509 -lcrypto

onionrouting_direct_inet_SOURCES = src/main-direct-inet.cc src/onionrouting.cc src/onionrouting-sender.cc src/Cryptographer.cc include/onionrouting.h include/Cryptographer.h
onionrouting_direct_inet_LDADD = -lparameters -linettransport -lx509 -lcrypto
                  
makeexp_SOURCES = src/makeexp.cc
                  
DISTCLEANFILES= config.log config.status 
MAINTAINERCLEANFILES= aclocal.m4 depcomp ltmain.sh mkinstalldirs stamp-h.in stamp-h1 install-sh missing include/config.h.in configure Makefile.in compile $(PACKAGE)-$(VERSION).tar.gz


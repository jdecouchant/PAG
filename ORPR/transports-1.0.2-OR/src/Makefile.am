## Process this file with automake to produce Makefile.in

if MAKE_LIBESIGNXP
ESIGNLIB = libesignxp.la
ESIGNPROGS = esign-keygen esign-signcert
ESIGNLDADD = -lgmp
endif

lib_LTLIBRARIES         = libsimtransport.la libinettransport.la libselfsign.la libnfstransport.la \
                          $(ESIGNLIB) libx509.la libdummyxp.la
libsimtransport_la_SOURCES = net/simulator.cc
libinettransport_la_SOURCES      = net/inet.cc
libnfstransport_la_SOURCES   = net/nfs_udp.cc
libselfsign_la_SOURCES  = id/simpleid.cc id/simpletransport.cc id/selfsign.cc
libesignxp_la_SOURCES   = id/simpleid.cc id/simpletransport.cc id/esignxp.cc
libx509_la_SOURCES      = id/simpleid.cc id/simpletransport.cc id/x509.cc
libdummyxp_la_SOURCES   = id/simpleid.cc id/simpletransport.cc id/x509.cc

bin_PROGRAMS = $(ESIGNPROGS) readcert

esign_keygen_SOURCES = tools/esign-keygen.cc

esign_keygen_LDADD = -lesign -lgmp

esign_signcert_SOURCES = tools/esign-signcert.cc

esign_signcert_LDADD = -lesign -lgmp

readcert_SOURCES = tools/readcert.cc

readcert_LDADD = $(ESIGNLDADD)

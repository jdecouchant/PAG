## Process this file with automake to produce Makefile.in

if MAKE_LIBESIGNXP
ESIGNLDADD = -lgmp
endif

SUBDIRS = src include
DISTCLEANFILES= config.log config.status
MAINTAINERCLEANFILES= config.guess config.sub aclocal.m4 depcomp config.guess config.sub ltmain.sh mkinstalldirs stamp-h.in stamp-h1 install-sh missing config.h.in configure Makefile.in compile $(PACKAGE)-$(VERSION).tar.gz
ACLOCAL_AMFLAGS = -I m4

makeca_SOURCES = src/tools/makeca.cc

makeca_LDADD = $(ESIGNLDADD)

bin_PROGRAMS = makeca

maintainer-clean-local:
	rm -rf m4

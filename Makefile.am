MCC_HOME=$(HOME)/ompss/mcxx-nonblock-mpi
NANOS_HOME=$(HOME)/ompss/nanox-nonblock-mpi

pwd=/home/bsc15/bsc15685/tests/mpi-nonblocking

ACLOCAL_AMFLAGS=-I m4

AM_CPPFLAGS=@ompssinc@ @mpiinc@ -I@top_srcdir@/src $(CPPFLAGS)
AM_CXXFLAGS=$(CXXFLAGS)
AM_LDFLAGS=@ompsslib@ @mpilib@ $(LDFLAGS)

pkginclude_HEADERS=
lib_LTLIBRARIES=libmpiompss-interop.la
libmpiompss_interop_la_SOURCES=src/mpioverload.cc
libmpiompss_interop_la_LIBADD=@ompsslibs@ @mpilibs@


MCC_HOME=$(HOME)/ompss/mcxx-nonblock-mpi
NANOS_HOME=$(HOME)/ompss/nanox-nonblock-mpi

ACLOCAL_AMFLAGS=-I m4

AM_CPPFLAGS=@ompssinc@ -I@top_srcdir@/src/common -include "config.h" $(CPPFLAGS)
AM_CXXFLAGS=$(CXXFLAGS)

pkginclude_HEADERS=

lib_LTLIBRARIES=libmpiompss-interop.la
#libmpiompss-interop-f.la

libmpiompss_interop_la_SOURCES=\
src/c/barrier.cc \
src/c/bcast.cc \
src/c/gatherv.cc \
src/c/recv.cc \
src/c/reduce.cc \
src/c/send.cc \
src/c/sendrecv.cc \
src/c/sendrecvreplace.cc \
src/c/waitall.cc \
src/c/wait.cc

libmpiompss_interop_la_LIBADD=@extraelibs@ @ompsslibs@

#libmpiompss_interop_f_la_SOURCES=src/mpioverload_fortran.cc
#libmpiompss_interop_f_la_LIBADD=libmpiompss-interop.la @extraelibs@ @ompsslibs@


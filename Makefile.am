MCC_HOME=$(HOME)/ompss/mcxx-nonblock-mpi
NANOS_HOME=$(HOME)/ompss/nanox-nonblock-mpi

ACLOCAL_AMFLAGS=-I m4

AM_CPPFLAGS=@ompssinc@ -I@top_srcdir@/src/common -include "config.h" $(CPPFLAGS)
AM_CXXFLAGS=$(CXXFLAGS)

pkginclude_HEADERS=

lib_LTLIBRARIES=libmpiompss-interop.la libmpiompss-interop-fort.la

libmpiompss_interop_la_SOURCES=\
src/c/barrier.cc \
src/c/bcast.cc \
src/c/gatherv.cc \
src/c/recv.cc \
src/c/reduce.cc \
src/c/send.cc \
src/c/sendrecv.cc \
src/c/waitall.cc \
src/c/wait.cc \
src/c/sendrecvreplace.cc

libmpiompss_interop_fort_la_SOURCES=\
src/fortran/barrier.cc \
src/fortran/bcast.cc \
src/fortran/gatherv.cc \
src/fortran/recv.cc \
src/fortran/reduce.cc \
src/fortran/send.cc \
src/fortran/sendrecv.cc \
src/fortran/waitall.cc \
src/fortran/wait.cc \
src/fortran/sendrecvreplace.cc

libmpiompss_interop_la_LIBADD=@extraelibs@ @ompsslibs@

libmpiompss_interop_fort_la_LIBADD=@extraelibs@ @ompsslibs@


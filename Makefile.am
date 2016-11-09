ACLOCAL_AMFLAGS=-I m4

AM_CPPFLAGS=\
 @ompssinc@ \
 -I@top_srcdir@/src \
 -I@top_srcdir@/src/common \
 -include "config.h" \
 $(CPPFLAGS)

if NANOS6
AM_CPPFLAGS+=\
 -I@top_srcdir@/src/nanos6
endif
if NANOX
AM_CPPFLAGS+=\
 -I@top_srcdir@/src/nanox
endif

AM_CXXFLAGS=$(CXXFLAGS)
AM_LDFLAGS=@extraelib@ @ompsslib@ -ldl $(LDFLAGS)
LIBS=@extraelibs@ @ompsslibs@

pkginclude_HEADERS= # This library does not provide any additional header

c_api_sources= \
 src/c/allreduce.cc \
 src/c/alltoall.cc \
 src/c/alltoallv.cc \
 src/c/barrier.cc \
 src/c/bcast.cc \
 src/c/gatherv.cc \
 src/c/recv.cc \
 src/c/reduce.cc \
 src/c/send.cc \
 src/c/sendrecv.cc \
 src/c/ssend.cc \
 src/c/waitall.cc \
 src/c/wait.cc \
 src/c/sendrecvreplace.cc

fortran_api_sources= \
 src/fortran/allreduce.cc \
 src/fortran/alltoall.cc \
 src/fortran/alltoallv.cc \
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

if NANOS6
c_api_sources+= \
 src/c/init-finalize.cc
fortran_api_sources+= \
 src/fortran/init-finalize.cc
endif

lib_LTLIBRARIES=libmpiompss-interop.la
libmpiompss_interop_la_SOURCES=$(c_api_sources) $(fortran_api_sources)


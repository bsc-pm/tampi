MCC_HOME=$(HOME)/ompss/mcxx-nonblock-mpi
NANOS_HOME=$(HOME)/ompss/nanox-nonblock-mpi

pwd=/home/bsc15/bsc15685/tests/mpi-nonblocking

ACLOCAL_AMFLAGS=-I m4

AM_CPPFLAGS=@ompssinc@ @mpiinc@ -I@top_srcdir@/src $(CPPFLAGS)
AM_CXXFLAGS=$(CXXFLAGS)

if USE_EXTRAE
AM_LDFLAGS= $(LDFLAGS)
else
AM_LDFLAGS= $(LDFLAGS) @mpilib@ @mpilibs@ 
endif

pkginclude_HEADERS=
lib_LTLIBRARIES=libmpiompss-interop.la libmpiompss-interop-f.la

libmpiompss_interop_la_SOURCES=src/mpioverload.cc
libmpiompss_interop_la_LIBADD=@extraelibs@ @ompsslibs@

libmpiompss_interop_f_la_SOURCES=src/mpioverload_fortran.cc
libmpiompss_interop_f_la_LIBADD=libmpiompss-interop.la @extraelibs@ @ompsslibs@


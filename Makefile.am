ACLOCAL_AMFLAGS=-I m4

AM_CPPFLAGS=\
 @ompssinc@ \
 @argobotsinc@ \
 -I@top_srcdir@/src \
 -I@top_srcdir@/src/common \
 -include "config.h" \
 $(CPPFLAGS)

AM_CXXFLAGS=$(CXXFLAGS)
AM_LDFLAGS=@extraelib@ @ompsslib@ @argobotslib@ $(LDFLAGS)
LIBS=@extraelibs@ @ompsslibs@ @argobotslibs@

pkginclude_HEADERS= # This library does not provide any additional header

c_api_sources= \
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

# Additional debug mode files
if debug_mode_enabled
c_api_sources += \
 src/c/init-finalize.cc
endif

fortran_api_sources= \
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

lib_LTLIBRARIES=libmpiompss-interop.la libmpiompss-interop-fort.la
libmpiompss_interop_la_SOURCES=$(c_api_sources)
libmpiompss_interop_fort_la_SOURCES=$(fortran_api_sources)


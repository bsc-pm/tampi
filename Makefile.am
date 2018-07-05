ACLOCAL_AMFLAGS=-I m4

EXTRA_DIST=\
 src/common/*.h\
 src/common/mpi/*.h

AM_CPPFLAGS=\
 @extraeinc@ \
 @ompssinc@ \
 -I@top_srcdir@/src \
 -I@top_srcdir@/src/common \
 -include "config.h" \
 $(CPPFLAGS)

if debug_mode_enabled
AM_CXXFLAGS=-O0 -g3 -Wall -Wextra $(CXXFLAGS)
else
AM_CXXFLAGS=-O3 -fno-rtti -fno-exceptions -fvisibility-inlines-hidden -Wall -Wextra $(CXXFLAGS)
endif

AM_LDFLAGS=@extraelib@ @ompsslib@ -ldl $(LDFLAGS)
LIBS=@extraelibs@ @ompsslibs@

if DEFINES_I_MPI_CXX
export I_MPI_CXX=@I_MPI_CXX@
endif
if DEFINES_OMPI_CXX
export OMPI_CXX=@OMPI_CXX@
endif
if DEFINES_MPICH_CXX
export MPICH_CXX=@MPICH_CXX@
endif

pkginclude_HEADERS= # This library does not provide any additional header

c_api_sources= \
 src/c/allgather.cc \
 src/c/allgatherv.cc \
 src/c/allreduce.cc \
 src/c/alltoall.cc \
 src/c/alltoallv.cc \
 src/c/alltoallw.cc \
 src/c/barrier.cc \
 src/c/bcast.cc \
 src/c/gather.cc \
 src/c/gatherv.cc \
 src/c/recv.cc \
 src/c/reduce.cc \
 src/c/reducescatter.cc \
 src/c/reducescatterblock.cc \
 src/c/scatter.cc \
 src/c/scatterv.cc \
 src/c/send.cc \
 src/c/sendrecv.cc \
 src/c/ssend.cc \
 src/c/waitall.cc \
 src/c/wait.cc \
 src/c/sendrecvreplace.cc \
 src/c/scan.cc \
 src/c/exscan.cc \
 src/c/init-finalize.cc \
 src/c/utils.cc

fortran_api_sources= \
 src/fortran/allgather.cc \
 src/fortran/allgatherv.cc \
 src/fortran/allreduce.cc \
 src/fortran/alltoall.cc \
 src/fortran/alltoallv.cc \
 src/fortran/alltoallw.cc \
 src/fortran/barrier.cc \
 src/fortran/bcast.cc \
 src/fortran/gather.cc \
 src/fortran/gatherv.cc \
 src/fortran/recv.cc \
 src/fortran/reduce.cc \
 src/fortran/reducescatter.cc \
 src/fortran/reducescatterblock.cc \
 src/fortran/scatter.cc \
 src/fortran/scatterv.cc \
 src/fortran/send.cc \
 src/fortran/sendrecv.cc \
 src/fortran/ssend.cc \
 src/fortran/waitall.cc \
 src/fortran/wait.cc \
 src/fortran/sendrecvreplace.cc \
 src/fortran/scan.cc \
 src/fortran/exscan.cc \
 src/fortran/init-finalize.cc \
 src/fortran/utils.cc

common_sources = \
 src/common/environment.cc \
 src/common/process_request.h

lib_LTLIBRARIES = libmpiompss-interop.la libmpiompss-c-interop.la libmpiompss-fortran-interop.la
libmpiompss_interop_la_SOURCES=$(common_sources) $(c_api_sources) $(fortran_api_sources)
libmpiompss_c_interop_la_SOURCES = $(common_sources) $(c_api_sources)
libmpiompss_fortran_interop_la_SOURCES = $(common_sources) $(fortran_api_sources)


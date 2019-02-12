ACLOCAL_AMFLAGS=-I m4

EXTRA_DIST=\
 src/common/*.h\
 src/include/*.h

AM_CPPFLAGS=\
 -I@top_srcdir@/src \
 -I@top_srcdir@/src/common \
 -include "config.h" \
 $(CPPFLAGS)

if debug_mode_enabled
AM_CXXFLAGS=-O0 -g3 -Wall -Wextra $(CXXFLAGS)
else
AM_CXXFLAGS=-O3 -fno-rtti -fno-exceptions -fvisibility-inlines-hidden -Wall -Wextra $(CXXFLAGS)
endif

AM_LDFLAGS=-ldl $(LDFLAGS)
LIBS=

if DEFINES_I_MPI_CXX
export I_MPI_CXX=@I_MPI_CXX@
endif
if DEFINES_OMPI_CXX
export OMPI_CXX=@OMPI_CXX@
endif
if DEFINES_MPICH_CXX
export MPICH_CXX=@MPICH_CXX@
endif

include_HEADERS= src/include/TAMPI.h src/include/TAMPIf.h
pkginclude_HEADERS= # This library does not provide any additional header

c_api_sources= \
 src/c/Allgather.cpp \
 src/c/Allgatherv.cpp \
 src/c/Allreduce.cpp \
 src/c/Alltoall.cpp \
 src/c/Alltoallv.cpp \
 src/c/Alltoallw.cpp \
 src/c/Barrier.cpp \
 src/c/Bcast.cpp \
 src/c/Bsend.cpp \
 src/c/Exscan.cpp \
 src/c/Gather.cpp \
 src/c/Gatherv.cpp \
 src/c/InitFinalize.cpp \
 src/c/Iwait.cpp \
 src/c/Iwaitall.cpp \
 src/c/Recv.cpp \
 src/c/Reduce.cpp \
 src/c/Reducescatter.cpp \
 src/c/Reducescatterblock.cpp \
 src/c/Rsend.cpp \
 src/c/Scan.cpp \
 src/c/Scatter.cpp \
 src/c/Scatterv.cpp \
 src/c/Send.cpp \
 src/c/Sendrecv.cpp \
 src/c/Sendrecvreplace.cpp \
 src/c/Ssend.cpp \
 src/c/Utils.cpp \
 src/c/Wait.cpp \
 src/c/Waitall.cpp

fortran_api_sources= \
 src/fortran/Allgather.cpp \
 src/fortran/Allgatherv.cpp \
 src/fortran/Allreduce.cpp \
 src/fortran/Alltoall.cpp \
 src/fortran/Alltoallv.cpp \
 src/fortran/Alltoallw.cpp \
 src/fortran/Barrier.cpp \
 src/fortran/Bcast.cpp \
 src/fortran/Bsend.cpp \
 src/fortran/Exscan.cpp \
 src/fortran/Gather.cpp \
 src/fortran/Gatherv.cpp \
 src/fortran/InitFinalize.cpp \
 src/fortran/Iwait.cpp \
 src/fortran/Iwaitall.cpp \
 src/fortran/Recv.cpp \
 src/fortran/Reduce.cpp \
 src/fortran/Reducescatter.cpp \
 src/fortran/Reducescatterblock.cpp \
 src/fortran/Rsend.cpp \
 src/fortran/Scan.cpp \
 src/fortran/Scatter.cpp \
 src/fortran/Scatterv.cpp \
 src/fortran/Send.cpp \
 src/fortran/Sendrecv.cpp \
 src/fortran/Sendrecvreplace.cpp \
 src/fortran/Ssend.cpp \
 src/fortran/Utils.cpp \
 src/fortran/Wait.cpp \
 src/fortran/Waitall.cpp

common_sources = \
 src/common/Environment.cpp

lib_LTLIBRARIES = libtampi.la libtampi-c.la libtampi-fortran.la
libtampi_la_SOURCES=$(common_sources) $(c_api_sources) $(fortran_api_sources)
libtampi_c_la_SOURCES = $(common_sources) $(c_api_sources)
libtampi_fortran_la_SOURCES = $(common_sources) $(fortran_api_sources)


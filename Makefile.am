#	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
#
#	Copyright (C) 2015-2021 Barcelona Supercomputing Center (BSC)

ACLOCAL_AMFLAGS=-I m4

AM_CPPFLAGS=\
 $(BOOST_CPPFLAGS) -DBOOST_ENABLE_ASSERT_DEBUG_HANDLER \
 -I$(top_srcdir)/src \
 -I$(top_srcdir)/src/common \
 -include "config.h" \
 $(PTHREAD_CFLAGS) \
 $(tampi_CPPFLAGS)

AM_CXXFLAGS=$(MPI_CXXFLAGS) $(tampi_CXXFLAGS)

AM_LDFLAGS=$(BOOST_LDFLAGS) $(PTHREAD_CFLAGS) $(PTHREAD_LIBS) -ldl $(MPI_CXXLDFLAGS)
LIBS=

include_HEADERS= \
 src/include/TAMPI.h \
 src/include/TAMPI_Decl.h \
 src/include/TAMPI_Wrappers.h \
 src/include/TAMPIf.h
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
 src/common/Environment.cpp \
 src/common/TaskingModel.cpp

noinst_HEADERS = \
 src/common/Definitions.hpp \
 src/common/Environment.hpp \
 src/common/Instrument.hpp \
 src/common/RequestManager.hpp \
 src/common/Symbol.hpp \
 src/common/TaskContext.hpp \
 src/common/TaskingModelAPI.hpp \
 src/common/TaskingModel.hpp \
 src/common/Ticket.hpp \
 src/common/TicketManager.hpp \
 src/common/TicketManagerInternals.hpp \
 src/common/util/ArrayView.hpp \
 src/common/util/Clock.hpp \
 src/common/util/EnvironmentVariable.hpp \
 src/common/util/ErrorHandler.hpp \
 src/common/util/LockFreeQueue.hpp \
 src/common/util/SpinLock.hpp \
 src/common/util/Utils.hpp

lib_LTLIBRARIES = libtampi.la libtampi-c.la libtampi-fortran.la

libtampi_la_CPPFLAGS=$(AM_CPPFLAGS)
libtampi_la_SOURCES=$(common_sources) $(c_api_sources) $(fortran_api_sources)

libtampi_c_la_CPPFLAGS=$(AM_CPPFLAGS) -DDISABLE_FORTRAN_LANG
libtampi_c_la_SOURCES = $(common_sources) $(c_api_sources)

libtampi_fortran_la_CPPFLAGS=$(AM_CPPFLAGS) -DDISABLE_C_LANG
libtampi_fortran_la_SOURCES = $(common_sources) $(fortran_api_sources)

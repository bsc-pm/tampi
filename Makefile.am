#	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
#
#	Copyright (C) 2015-2025 Barcelona Supercomputing Center (BSC)

ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = \
 $(BOOST_CPPFLAGS) -DBOOST_ENABLE_ASSERT_DEBUG_HANDLER \
 -I$(top_srcdir)/src \
 -I$(top_srcdir)/src/common \
 -I$(top_srcdir)/src/include \
 -include "config.h" \
 $(ovni_CPPFLAGS) \
 $(tampi_CPPFLAGS) \
 $(asan_CPPFLAGS)

AM_CXXFLAGS = $(MPI_CXXFLAGS) $(tampi_CXXFLAGS) $(asan_CXXFLAGS)

AM_LDFLAGS = $(ovni_LIBS) $(asan_LDFLAGS) -ldl $(MPI_CXXLDFLAGS)
LIBS =

include_HEADERS = \
 src/include/TAMPI.h \
 src/include/TAMPI_Decl.h \
 src/include/TAMPI_Wrappers.h \
 src/include/TAMPIf.h

pkginclude_HEADERS = # This library does not provide any additional header

c_api_sources = \
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
 src/c/Wait.cpp \
 src/c/Recv.cpp \
 src/c/Reduce.cpp \
 src/c/Reducescatter.cpp \
 src/c/Reducescatterblock.cpp \
 src/c/Rsend.cpp \
 src/c/Scan.cpp \
 src/c/Scatter.cpp \
 src/c/Scatterv.cpp \
 src/c/Send.cpp \
 src/c/Ssend.cpp \
 src/c/Utils.cpp

common_sources = \
 src/common/Allocator.cpp \
 src/common/Environment.cpp \
 src/common/Operation.cpp \
 src/common/TaskingModel.cpp \
 src/common/instrument/Instrument.cpp \
 src/common/polling/PollingPeriodCtrl.cpp

noinst_HEADERS = \
 src/common/Allocator.hpp \
 src/common/ALPI.hpp \
 src/common/Declarations.hpp \
 src/common/CompletionManager.hpp \
 src/common/Environment.hpp \
 src/common/Interface.hpp \
 src/common/Operation.hpp \
 src/common/OperationManager.hpp \
 src/common/Symbol.hpp \
 src/common/TaskContext.hpp \
 src/common/TaskingModel.hpp \
 src/common/Ticket.hpp \
 src/common/TicketManager.hpp \
 src/common/TicketManagerCapacityCtrl.hpp \
 src/common/TicketManagerInternals.hpp \
 src/common/instrument/Instrument.hpp \
 src/common/instrument/OvniInstrument.hpp \
 src/common/polling/Polling.hpp \
 src/common/polling/PollingPeriodCtrl.hpp \
 src/common/polling/SlowStartPollingPeriodCtrl.hpp \
 src/common/util/ArrayView.hpp \
 src/common/util/BoostLockFreeQueue.hpp \
 src/common/util/Clock.hpp \
 src/common/util/EnvironmentVariable.hpp \
 src/common/util/ErrorHandler.hpp \
 src/common/util/FixedSizeStack.hpp \
 src/common/util/MultiLockFreeQueue.hpp \
 src/common/util/SpinLock.hpp \
 src/common/util/SpinWait.hpp \
 src/common/util/StringSupport.hpp \
 src/common/util/Utils.hpp

lib_LTLIBRARIES = libtampi.la libtampi-c.la libtampi-fortran.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = tampi.pc

libtampi_la_CPPFLAGS = $(AM_CPPFLAGS)
libtampi_la_SOURCES = $(common_sources) $(c_api_sources)

libtampi_c_la_CPPFLAGS = $(AM_CPPFLAGS) -DDISABLE_FORTRAN_LANG
libtampi_c_la_SOURCES = $(common_sources) $(c_api_sources)

libtampi_fortran_la_CPPFLAGS = $(AM_CPPFLAGS) -DDISABLE_C_LANG
libtampi_fortran_la_SOURCES = $(common_sources)

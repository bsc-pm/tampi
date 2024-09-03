/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2024 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "Declarations.hpp"
#include "Environment.hpp"
#include "OperationManager.hpp"
#include "Symbol.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {

int MPI_Allgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
		void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	if (Environment::isBlockingEnabledForCurrentThread()) {
		OperationManager<C, CollOperation>::process(ALLGATHER, BLK, comm, sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype);
		return MPI_SUCCESS;
	} else {
		static Symbol<Prototypes<C>::mpi_allgather_t> symbol(__func__);
		return symbol(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
	}
}

int TAMPI_Iallgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
		void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
	if (Environment::isNonBlockingEnabled()) {
		OperationManager<C, CollOperation>::process(ALLGATHER, NONBLK, comm, sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype);
		return MPI_SUCCESS;
	} else {
		ErrorHandler::fail(__func__, " not enabled");
		return MPI_ERR_UNSUPPORTED_OPERATION;
	}
}

} // extern C

#pragma GCC visibility pop

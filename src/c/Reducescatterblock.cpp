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

int MPI_Reduce_scatter_block(const void *sendbuf, void *recvbuf, int recvcount, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	if (Environment::isBlockingEnabledForCurrentThread()) {
		OperationManager<C, CollOperation>::process(REDUCESCATTERBLOCK, BLK, comm, sendbuf, 0, datatype, recvbuf, recvcount, MPI_DATATYPE_NULL, op);
		return MPI_SUCCESS;
	} else {
		static Symbol<Prototypes<C>::mpi_reduce_scatter_block_t> symbol(__func__);
		return symbol(sendbuf, recvbuf, recvcount, datatype, op, comm);
	}
}

int TAMPI_Ireduce_scatter_block(const void *sendbuf, void *recvbuf, int recvcount, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	if (Environment::isNonBlockingEnabled()) {
		OperationManager<C, CollOperation>::process(REDUCESCATTERBLOCK, NONBLK, comm, sendbuf, 0, datatype, recvbuf, recvcount, MPI_DATATYPE_NULL, op);
		return MPI_SUCCESS;
	} else {
		ErrorHandler::fail(__func__, " not enabled");
		return MPI_ERR_UNSUPPORTED_OPERATION;
	}
}

} // extern C

#pragma GCC visibility pop

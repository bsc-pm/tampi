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

int	MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
	if (Environment::isBlockingEnabledForCurrentThread()) {
		OperationManager<C, CollOperation>::process(BCAST, BLK, comm, nullptr, 0, MPI_DATATYPE_NULL, buffer, count, datatype, MPI_OP_NULL, root);
		return MPI_SUCCESS;
	} else {
		static Symbol<Prototypes<C>::mpi_bcast_t> symbol(__func__);
		return symbol(buffer, count, datatype, root, comm);
	}
}

int TAMPI_Ibcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
	if (Environment::isNonBlockingEnabled()) {
		OperationManager<C, CollOperation>::process(BCAST, NONBLK, comm, nullptr, 0, MPI_DATATYPE_NULL, buffer, count, datatype, MPI_OP_NULL, root);
		return MPI_SUCCESS;
	} else {
		ErrorHandler::fail(__func__, " not enabled");
		return MPI_ERR_UNSUPPORTED_OPERATION;
	}
}

} // extern C

#pragma GCC visibility pop

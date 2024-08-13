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

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	if (Environment::isBlockingEnabledForCurrentThread()) {
		OperationManager<C, Operation>::process(RECV, BLK, buf, count, datatype, source, tag, comm, status);
		return MPI_SUCCESS;
	} else {
		static Symbol<Prototypes<C>::mpi_recv_t> symbol(__func__);
		return symbol(buf, count, datatype, source, tag, comm, status);
	}
}

int TAMPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	if (Environment::isNonBlockingEnabled()) {
		OperationManager<C, Operation>::process(RECV, NONBLK, buf, count, datatype, source, tag, comm, status);
		return MPI_SUCCESS;
	} else {
		ErrorHandler::fail(__func__, " not enabled");
		return MPI_ERR_UNSUPPORTED_OPERATION;
	}
}

} // extern C

#pragma GCC visibility pop

/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2024 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "Declarations.hpp"
#include "Environment.hpp"
#include "OperationManager.hpp"
#include "Symbol.hpp"
#include "instrument/Instrument.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {

int MPI_Allreduce(MPI3CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	if (Environment::isBlockingEnabledForCurrentThread()) {
		Instrument::Guard<LibraryInterface> instrGuard;
		CollOperation<C> operation(ALLREDUCE, comm, sendbuf, count, datatype, recvbuf, count, datatype, op);
		OperationManager<C>::processOperation(operation, true);
		return MPI_SUCCESS;
	} else {
		static Symbol<MPI_Allreduce_t> symbol(__func__);
		return symbol(sendbuf, recvbuf, count, datatype, op, comm);
	}
}

int TAMPI_Iallreduce(MPI3CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
	if (Environment::isNonBlockingEnabled()) {
		Instrument::Guard<LibraryInterface> instrGuard;
		CollOperation<C> operation(ALLREDUCE, comm, sendbuf, count, datatype, recvbuf, count, datatype, op);
		OperationManager<C>::processOperation(operation, false);
		return MPI_SUCCESS;
	} else {
		ErrorHandler::fail(__func__, " not enabled");
		return MPI_ERR_UNSUPPORTED_OPERATION;
	}
}

} // extern C

#pragma GCC visibility pop

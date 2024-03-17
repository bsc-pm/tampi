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

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	if (Environment::isBlockingEnabledForCurrentThread()) {
		Instrument::Guard<LibraryInterface> instrGuard;
		Operation<C> operation(RECV, buf, count, datatype, source, tag, comm);
		OperationManager<C>::processOperation(operation, true, status);
		return MPI_SUCCESS;
	} else {
		static Symbol<Prototypes<C>::mpi_recv_t> symbol(__func__);
		return symbol(buf, count, datatype, source, tag, comm, status);
	}
}

int TAMPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	if (Environment::isNonBlockingEnabled()) {
		Instrument::Guard<LibraryInterface> instrGuard;
		Operation<C> operation(RECV, buf, count, datatype, source, tag, comm);
		OperationManager<C>::processOperation(operation, false, status);
		return MPI_SUCCESS;
	} else {
		ErrorHandler::fail(__func__, " not enabled");
		return MPI_ERR_UNSUPPORTED_OPERATION;
	}
}

} // extern C

#pragma GCC visibility pop

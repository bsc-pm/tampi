/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "Declarations.hpp"
#include "Environment.hpp"
#include "Interface.hpp"
#include "OperationManager.hpp"
#include "Symbol.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {

int MPI_Gatherv(MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype,
		void *recvbuf, MPI3CONST int recvcounts[], MPI3CONST int displs[],
		MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	if (Environment::isBlockingEnabledForCurrentThread()) {
		Instrument::Guard<LibraryInterface> instrGuard;
		CollOperation<C> operation(GATHERV, comm, sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, 0, root);
		OperationManager<C>::processOperation(operation, true);
		return MPI_SUCCESS;
	} else {
		static Symbol<MPI_Gatherv_t> symbol(__func__);
		return symbol(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);
	}
}

int TAMPI_Igatherv(MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype,
		void *recvbuf, MPI3CONST int recvcounts[], MPI3CONST int displs[],
		MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	if (Environment::isNonBlockingEnabled()) {
		Instrument::Guard<LibraryInterface> instrGuard;
		CollOperation<C> operation(GATHERV, comm, sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, 0, root);
		OperationManager<C>::processOperation(operation, false);
		return MPI_SUCCESS;
	} else {
		ErrorHandler::fail(__func__, " not enabled");
		return MPI_ERR_UNSUPPORTED_OPERATION;
	}
}

} // extern C

#pragma GCC visibility pop

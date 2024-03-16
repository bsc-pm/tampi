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

int MPI_Allgatherv(MPI3CONST void* sendbuf, int sendcount, MPI_Datatype sendtype,
		void* recvbuf, MPI3CONST int recvcounts[], MPI3CONST int displs[],
		MPI_Datatype recvtype, MPI_Comm comm)
{
	if (Environment::isBlockingEnabledForCurrentThread()) {
		Instrument::Guard<LibraryInterface> instrGuard;
		CollOperation<C> operation(ALLGATHERV, comm, sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype);
		OperationManager<C>::processOperation(operation, true);
		return MPI_SUCCESS;
	} else {
		static Symbol<Prototypes<C>::mpi_allgatherv_t> symbol(__func__);
		return symbol(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
	}
}

int TAMPI_Iallgatherv(MPI3CONST void* sendbuf, int sendcount, MPI_Datatype sendtype,
		void* recvbuf, MPI3CONST int recvcounts[], MPI3CONST int displs[],
		MPI_Datatype recvtype, MPI_Comm comm)
{
	if (Environment::isNonBlockingEnabled()) {
		Instrument::Guard<LibraryInterface> instrGuard;
		CollOperation<C> operation(ALLGATHERV, comm, sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype);
		OperationManager<C>::processOperation(operation, false);
		return MPI_SUCCESS;
	} else {
		ErrorHandler::fail(__func__, " not enabled");
		return MPI_ERR_UNSUPPORTED_OPERATION;
	}
}

} // extern C

#pragma GCC visibility pop

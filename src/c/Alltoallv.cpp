/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "Declarations.hpp"
#include "Environment.hpp"
#include "OperationManager.hpp"
#include "Symbol.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {

int MPI_Alltoallv(MPI3CONST void *sendbuf, MPI3CONST int sendcounts[], MPI3CONST int sdispls[], MPI_Datatype sendtype,
		void *recvbuf, MPI3CONST int recvcounts[], MPI3CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm)
{
	if (Environment::isBlockingEnabledForCurrentThread()) {
		Instrument::Guard<LibraryInterface> instrGuard;
		CollOperation<C> operation(ALLTOALLV, comm, sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype);
		OperationManager<C>::processOperation(operation, true);
		return MPI_SUCCESS;
	} else {
		static Symbol<MPI_Alltoallv_t> symbol(__func__);
		return symbol(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm);
	}
}

int TAMPI_Ialltoallv(MPI3CONST void *sendbuf, MPI3CONST int sendcounts[], MPI3CONST int sdispls[], MPI_Datatype sendtype,
		void *recvbuf, MPI3CONST int recvcounts[], MPI3CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm)
{
	if (Environment::isNonBlockingEnabled()) {
		Instrument::Guard<LibraryInterface> instrGuard;
		CollOperation<C> operation(ALLTOALLV, comm, sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype);
		OperationManager<C>::processOperation(operation, false);
		return MPI_SUCCESS;
	} else {
		ErrorHandler::fail(__func__, " not enabled");
		return MPI_ERR_UNSUPPORTED_OPERATION;
	}
}

} // extern C

#pragma GCC visibility pop

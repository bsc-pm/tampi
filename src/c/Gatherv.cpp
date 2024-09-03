/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2024 Barcelona Supercomputing Center (BSC)
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

int MPI_Gatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
		void *recvbuf, const int recvcounts[], const int displs[],
		MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	if (Environment::isBlockingEnabledForCurrentThread()) {
		OperationManager<C, CollOperation>::process(GATHERV, BLK, comm, sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, MPI_OP_NULL, root);
		return MPI_SUCCESS;
	} else {
		static Symbol<Prototypes<C>::mpi_gatherv_t> symbol(__func__);
		return symbol(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);
	}
}

int TAMPI_Igatherv(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
		void *recvbuf, const int recvcounts[], const int displs[],
		MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	if (Environment::isNonBlockingEnabled()) {
		OperationManager<C, CollOperation>::process(GATHERV, NONBLK, comm, sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, MPI_OP_NULL, root);
		return MPI_SUCCESS;
	} else {
		ErrorHandler::fail(__func__, " not enabled");
		return MPI_ERR_UNSUPPORTED_OPERATION;
	}
}

} // extern C

#pragma GCC visibility pop

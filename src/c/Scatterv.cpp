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

int MPI_Scatterv(const void* sendbuf, const int sendcounts[], const int displs[], MPI_Datatype sendtype,
		void* recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	if (Environment::isBlockingEnabledForCurrentThread()) {
		OperationManager<C, CollOperation>::process(SCATTERV, BLK, comm, sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, MPI_OP_NULL, root);
		return MPI_SUCCESS;
	} else {
		static Symbol<Prototypes<C>::mpi_scatterv_t> symbol(__func__);
		return symbol(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm);
	}
}

int TAMPI_Iscatterv(const void* sendbuf, const int sendcounts[], const int displs[], MPI_Datatype sendtype,
		void* recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	if (Environment::isNonBlockingEnabled()) {
		OperationManager<C, CollOperation>::process(SCATTERV, NONBLK, comm, sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, MPI_OP_NULL, root);
		return MPI_SUCCESS;
	} else {
		ErrorHandler::fail(__func__, " not enabled");
		return MPI_ERR_UNSUPPORTED_OPERATION;
	}
}

} // extern C

#pragma GCC visibility pop

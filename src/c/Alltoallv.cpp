/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "Declarations.hpp"
#include "Environment.hpp"
#include "Interface.hpp"
#include "RequestManager.hpp"
#include "Symbol.hpp"
#include "instrument/Instrument.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {

int MPI_Alltoallv(MPI3CONST void *sendbuf, MPI3CONST int sendcounts[], MPI3CONST int sdispls[], MPI_Datatype sendtype,
		void *recvbuf, MPI3CONST int recvcounts[], MPI3CONST int rdispls[], MPI_Datatype recvtype, MPI_Comm comm)
{
	int err = MPI_SUCCESS;
	if (Environment::isBlockingEnabledForCurrentThread()) {
		Instrument::Guard<LibraryInterface> instrGuard;
		Instrument::enter<IssueNonBlockingOp>();

		static Symbol<MPI_Ialltoallv_t> symbol("MPI_Ialltoallv");

		MPI_Request request;
		err = symbol(sendbuf, sendcounts, sdispls, sendtype,
				recvbuf, recvcounts, rdispls, recvtype, comm, &request);

		Instrument::exit<IssueNonBlockingOp>();

		if (err == MPI_SUCCESS)
			RequestManager<C>::processRequest(request);
	} else {
		static Symbol<MPI_Alltoallv_t> symbol(__func__);
		err = symbol(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm);
	}
	return err;
}

} // extern C

#pragma GCC visibility pop

/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#include <dlfcn.h>
#include <mpi.h>

#include "Environment.hpp"
#include "Interface.hpp"
#include "RequestManager.hpp"
#include "Symbol.hpp"
#include "instrument/Instrument.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {

int MPI_Gatherv(MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype,
		void *recvbuf, MPI3CONST int recvcounts[], MPI3CONST int displs[],
		MPI_Datatype recvtype, int root, MPI_Comm comm)
{
	int err = MPI_SUCCESS;
	if (Environment::isBlockingEnabledForCurrentThread()) {
		Instrument::Guard<LibraryInterface> instrGuard;
		Instrument::enter<IssueNonBlockingOp>();

		MPI_Request request;
		err = MPI_Igatherv(sendbuf, sendcount, sendtype,
				recvbuf, recvcounts, displs, recvtype,
				root, comm, &request);

		Instrument::exit<IssueNonBlockingOp>();

		if (err == MPI_SUCCESS)
			RequestManager<C>::processRequest(request);
	} else {
		static MPI_Gatherv_t *symbol = (MPI_Gatherv_t *) Symbol::load(__func__);
		err = (*symbol)(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm);
	}
	return err;
}

} // extern C

#pragma GCC visibility pop

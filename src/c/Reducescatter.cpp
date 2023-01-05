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
	int MPI_Reduce_scatter(MPI3CONST void *sendbuf, void *recvbuf, MPI3CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
	{
		int err = MPI_SUCCESS;
		if (Environment::isBlockingEnabledForCurrentThread()) {
			Instrument::Guard<LibraryInterface> instrGuard;
			Instrument::enter<IssueNonBlockingOp>();

			MPI_Request request;
			err = MPI_Ireduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm, &request);

			Instrument::exit<IssueNonBlockingOp>();

			if (err == MPI_SUCCESS)
				RequestManager<C>::processRequest(request);
		} else {
			static MPI_Reduce_scatter_t *symbol = (MPI_Reduce_scatter_t *) Symbol::load(__func__);
			err = (*symbol)(sendbuf, recvbuf, recvcounts, datatype, op, comm);
		}
		return err;
	}
} // extern C

#pragma GCC visibility pop

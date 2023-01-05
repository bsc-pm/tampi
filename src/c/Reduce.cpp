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
	int MPI_Reduce(MPI3CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
	{
		int err = MPI_SUCCESS;
		if (Environment::isBlockingEnabledForCurrentThread()) {
			Instrument::Guard<LibraryInterface> instrGuard;
			Instrument::enter<IssueNonBlockingOp>();

			MPI_Request request;
			err = MPI_Ireduce(sendbuf, recvbuf, count, datatype, op, root, comm, &request);

			Instrument::exit<IssueNonBlockingOp>();

			if (err == MPI_SUCCESS)
				RequestManager<C>::processRequest(request);
		} else {
			static MPI_Reduce_t *symbol = (MPI_Reduce_t *) Symbol::load(__func__);
			err = (*symbol)(sendbuf, recvbuf, count, datatype, op, root, comm);
		}
		return err;
	}
} // extern C

#pragma GCC visibility pop

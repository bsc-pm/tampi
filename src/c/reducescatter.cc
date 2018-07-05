/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <dlfcn.h>
#include <mpi.h>

#if MPI_VERSION >=3

#include "definitions.h"
#include "environment.h"
#include "process_request.h"
#include "symbols.h"

extern "C" {
	int MPI_Reduce_scatter(const void *sendbuf, void *recvbuf, const int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
	{
		int err = MPI_SUCCESS;
		if (C::Environment::isEnabled()) {
			MPI_Request request;
			err = MPI_Ireduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm, &request);
			C::processRequest(request);
		} else {
			static C::MPI_Reduce_scatter_t *symbol = (C::MPI_Reduce_scatter_t *) Symbol::loadNextSymbol(__func__);
			err = (*symbol)(sendbuf, recvbuf, recvcounts, datatype, op, comm);
		}
		return err;
	}
}

#endif // MPI_VERSION

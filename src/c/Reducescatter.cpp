/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2020 Barcelona Supercomputing Center (BSC)
*/

#include <dlfcn.h>
#include <mpi.h>

#include "Definitions.hpp"
#include "Environment.hpp"
#include "RequestManager.hpp"
#include "Symbol.hpp"

extern "C" {
	int MPI_Reduce_scatter(MPI3CONST void *sendbuf, void *recvbuf, MPI3CONST int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
	{
		int err = MPI_SUCCESS;
		if (Environment::isBlockingEnabled()) {
			MPI_Request request;
			err = MPI_Ireduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm, &request);
			if (err == MPI_SUCCESS)
				RequestManager<C>::processRequest(request);
		} else {
			static MPI_Reduce_scatter_t *symbol = (MPI_Reduce_scatter_t *) Symbol::load(__func__);
			err = (*symbol)(sendbuf, recvbuf, recvcounts, datatype, op, comm);
		}
		return err;
	}
}


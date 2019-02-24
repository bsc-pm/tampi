/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <dlfcn.h>
#include <mpi.h>

#if MPI_VERSION >=3

#include "Definitions.hpp"
#include "Environment.hpp"
#include "RequestManager.hpp"
#include "Symbols.hpp"

extern "C" {
	int MPI_Reduce_scatter(const void *sendbuf, void *recvbuf, const int recvcounts[], MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
	{
		int err = MPI_SUCCESS;
		if (Environment<C>::isBlockingEnabled()) {
			MPI_Request request;
			err = MPI_Ireduce_scatter(sendbuf, recvbuf, recvcounts, datatype, op, comm, &request);
			if (err == MPI_SUCCESS)
				RequestManager<C>::processRequest(request);
		} else {
			static MPI_Reduce_scatter_t *symbol = (MPI_Reduce_scatter_t *) Symbol::loadNextSymbol(__func__);
			err = (*symbol)(sendbuf, recvbuf, recvcounts, datatype, op, comm);
		}
		return err;
	}
}

#endif // MPI_VERSION

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
	int MPI_Alltoallw(const void *sendbuf, const int sendcounts[], const int sdispls[], const MPI_Datatype sendtypes[],
			void *recvbuf, const int recvcounts[], const int rdispls[], const MPI_Datatype recvtypes[], MPI_Comm comm)
	{
		int err = MPI_SUCCESS;
		if (C::Environment::isEnabled()) {
			MPI_Request request;
			err = MPI_Ialltoallw(sendbuf, sendcounts, sdispls, sendtypes,
					recvbuf, recvcounts, rdispls, recvtypes,
					comm, &request);
			C::processRequest(request);
		} else {
			static C::MPI_Alltoallw_t *symbol = (C::MPI_Alltoallw_t *) Symbol::loadNextSymbol(__func__);
			err = (*symbol)(sendbuf, sendcounts, sdispls, sendtypes, recvbuf, recvcounts, rdispls, recvtypes, comm);
		}
		return err;
	}
}

#endif // MPI_VERSION

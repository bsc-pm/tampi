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
	int MPI_Alltoallv(
			const void *sendbuf, const int sendcounts[], const int sdispls[], MPI_Datatype sendtype,
			void *recvbuf, const int recvcounts[], const int rdispls[], MPI_Datatype recvtype,
			MPI_Comm comm)
	{
		int err = MPI_SUCCESS;
		if (C::Environment::isEnabled()) {
			MPI_Request request;
			err = MPI_Ialltoallv(sendbuf, sendcounts, sdispls, sendtype,
					recvbuf, recvcounts, rdispls, recvtype, comm, &request);
			
			C::processRequest(request);
		} else {
			static C::MPI_Alltoallv_t *symbol = (C::MPI_Alltoallv_t *) Symbol::loadNextSymbol(__func__);
			err = (*symbol)(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, comm);
		}
		return err;
	}
}

#endif // MPI_VERSION

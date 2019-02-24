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
	int MPI_Alltoallw(const void *sendbuf, const int sendcounts[], const int sdispls[], const MPI_Datatype sendtypes[],
			void *recvbuf, const int recvcounts[], const int rdispls[], const MPI_Datatype recvtypes[], MPI_Comm comm)
	{
		int err = MPI_SUCCESS;
		if (Environment<C>::isBlockingEnabled()) {
			MPI_Request request;
			err = MPI_Ialltoallw(sendbuf, sendcounts, sdispls, sendtypes,
					recvbuf, recvcounts, rdispls, recvtypes,
					comm, &request);
			if (err == MPI_SUCCESS)
				RequestManager<C>::processRequest(request);
		} else {
			static MPI_Alltoallw_t *symbol = (MPI_Alltoallw_t *) Symbol::loadNextSymbol(__func__);
			err = (*symbol)(sendbuf, sendcounts, sdispls, sendtypes, recvbuf, recvcounts, rdispls, recvtypes, comm);
		}
		return err;
	}
}

#endif // MPI_VERSION

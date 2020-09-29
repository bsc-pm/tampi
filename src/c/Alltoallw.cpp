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

#pragma GCC visibility push(default)

extern "C" {
	int MPI_Alltoallw(MPI3CONST void *sendbuf, MPI3CONST int sendcounts[], MPI3CONST int sdispls[], MPI3CONST MPI_Datatype sendtypes[],
			void *recvbuf, MPI3CONST int recvcounts[], MPI3CONST int rdispls[], MPI3CONST MPI_Datatype recvtypes[], MPI_Comm comm)
	{
		int err = MPI_SUCCESS;
		if (Environment::isBlockingEnabled()) {
			MPI_Request request;
			err = MPI_Ialltoallw(sendbuf, sendcounts, sdispls, sendtypes,
					recvbuf, recvcounts, rdispls, recvtypes,
					comm, &request);
			if (err == MPI_SUCCESS)
				RequestManager<C>::processRequest(request);
		} else {
			static MPI_Alltoallw_t *symbol = (MPI_Alltoallw_t *) Symbol::load(__func__);
			err = (*symbol)(sendbuf, sendcounts, sdispls, sendtypes, recvbuf, recvcounts, rdispls, recvtypes, comm);
		}
		return err;
	}
} // extern C

#pragma GCC visibility pop

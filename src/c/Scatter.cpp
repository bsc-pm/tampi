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

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {
	int MPI_Scatter(MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype,
			void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
	{
		int err = MPI_SUCCESS;
		if (Environment::isBlockingEnabledForCurrentThread()) {
			MPI_Request request;
			err = MPI_Iscatter(sendbuf, sendcount, sendtype,
					recvbuf, recvcount, recvtype, root,
					comm, &request);
			if (err == MPI_SUCCESS)
				RequestManager<C>::processRequest(request);
		} else {
			static MPI_Scatter_t *symbol = (MPI_Scatter_t *) Symbol::load(__func__);
			err = (*symbol)(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
		}
		return err;
	}
} // extern C

#pragma GCC visibility pop

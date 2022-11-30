/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2022 Barcelona Supercomputing Center (BSC)
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
	int MPI_Exscan(MPI3CONST void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
	{
		int err = MPI_SUCCESS;
		if (Environment::isBlockingEnabled()) {
			MPI_Request request;
			err = MPI_Iexscan(sendbuf, recvbuf, count, datatype, op, comm, &request);
			if (err == MPI_SUCCESS)
				RequestManager<C>::processRequest(request);
		} else {
			static MPI_Exscan_t *symbol = (MPI_Exscan_t *) Symbol::load(__func__);
			err = (*symbol)(sendbuf, recvbuf, count, datatype, op, comm);
		}
		return err;
	}
} // extern C

#pragma GCC visibility pop

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
	int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
	{
		int err = MPI_SUCCESS;
		if (Environment::isBlockingEnabled()) {
			MPI_Request request;
			err = MPI_Irecv(buf, count, datatype, source, tag, comm, &request);
			if (err == MPI_SUCCESS)
				RequestManager<C>::processRequest(request, status);
		} else {
			static MPI_Recv_t *symbol = (MPI_Recv_t *) Symbol::load(__func__);
			err = (*symbol)(buf, count, datatype, source, tag, comm, status);
		}

		return err;
	}
} // extern C

#pragma GCC visibility pop

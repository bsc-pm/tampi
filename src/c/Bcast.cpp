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
	int	MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
	{
		int err = MPI_SUCCESS;
		if (Environment<C>::isBlockingEnabled()) {
			MPI_Request request;
			err = MPI_Ibcast(buffer, count, datatype, root, comm, &request);
			if (err == MPI_SUCCESS)
				RequestManager<C>::processRequest(request);
		} else {
			static MPI_Bcast_t *symbol = (MPI_Bcast_t *) Symbol::load(__func__);
			err = (*symbol)(buffer, count, datatype, root, comm);
		}
		return err;
	}
} // extern C


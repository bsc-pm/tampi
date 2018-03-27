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
	int	MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
	{
		int err = MPI_SUCCESS;
		if (C::Environment::isEnabled()) {
			MPI_Request request;
			err = MPI_Ibcast(buffer, count, datatype, root, comm, &request);
			C::processRequest(request);
		} else {
			static C::MPI_Bcast_t *symbol = (C::MPI_Bcast_t *) Symbol::loadNextSymbol(__func__);
			err = (*symbol)(buffer, count, datatype, root, comm);
		}
		return err;
	}
} // extern C

#endif // MPI_VERSION


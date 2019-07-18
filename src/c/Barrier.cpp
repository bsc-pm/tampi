/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2019 Barcelona Supercomputing Center (BSC)
*/

#include <dlfcn.h>
#include <mpi.h>

#include "Definitions.hpp"
#include "Environment.hpp"
#include "RequestManager.hpp"
#include "Symbol.hpp"

extern "C" {
	int MPI_Barrier(MPI_Comm comm)
	{
		int err = MPI_SUCCESS;
		if (Environment<C>::isBlockingEnabled()) {
			MPI_Request request;
			err = MPI_Ibarrier(comm, &request);
			if (err == MPI_SUCCESS)
				RequestManager<C>::processRequest(request);
		} else {
			static MPI_Barrier_t *symbol = (MPI_Barrier_t *) Symbol::loadNextSymbol(__func__);
			err = (*symbol)(comm);
		}
		return err;
	}
} // extern C

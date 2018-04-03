/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <dlfcn.h>
#include <mpi.h>

#include "definitions.h"
#include "environment.h"
#include "process_request.h"
#include "symbols.h"

extern "C" {
	int MPI_Waitall(int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[])
	{
		int err = MPI_SUCCESS;
		if (C::Environment::isEnabled()) {
			C::processRequests({array_of_requests, count}, array_of_statuses);
		} else {
			static C::MPI_Waitall_t *symbol = (C::MPI_Waitall_t *) Symbol::loadNextSymbol(__func__);
			err = (*symbol)(count, array_of_requests, array_of_statuses);
		}
		return err;
	}
} // extern C


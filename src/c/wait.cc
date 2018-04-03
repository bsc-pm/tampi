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
	int MPI_Wait(MPI_Request *request, MPI_Status *status)
	{
		int err = MPI_SUCCESS;
		if (C::Environment::isEnabled()) {
			assert(request != NULL);
			C::processRequest(*request, status);
		} else {
			static C::MPI_Wait_t *symbol = (C::MPI_Wait_t *) Symbol::loadNextSymbol(__func__);
			err = (*symbol)(request, status);
		}
		return err;
	}
} // extern C


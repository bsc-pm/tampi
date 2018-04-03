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
	int MPI_Ssend(MPI3CONST void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
	{
		int err = MPI_SUCCESS;
		if (C::Environment::isEnabled()) {
			MPI_Request request;
			err = MPI_Issend(buf, count, datatype, dest, tag, comm, &request);
			C::processRequest(request);
		} else {
			static C::MPI_Ssend_t *symbol = (C::MPI_Ssend_t *) Symbol::loadNextSymbol(__func__);
			err = (*symbol)(buf, count, datatype, dest, tag, comm);
		}
		return err;
	}
} // extern C


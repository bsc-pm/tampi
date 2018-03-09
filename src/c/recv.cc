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
	int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
	{
		int err = MPI_SUCCESS;
		if (C::Environment::isEnabled()) {
			MPI_Request request;
			err = MPI_Irecv(buf, count, datatype, source, tag, comm, &request);
			C::processRequest(request, status);
		} else {
			static C::MPI_Recv_t *symbol = (C::MPI_Recv_t *) Symbol::loadNextSymbol(__func__);
			err = (*symbol)(buf, count, datatype, source, tag, comm, status);
		}
		
		return err;
	}
} // extern C


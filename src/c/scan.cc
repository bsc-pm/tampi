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
	int MPI_Scan(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
	{
		int err = MPI_SUCCESS;
		if (C::Environment::isEnabled()) {
			MPI_Request request;
			err = MPI_Iscan(sendbuf, recvbuf, count, datatype, op, comm, &request);
			C::processRequest(request);
		} else {
			static C::MPI_Scan_t *symbol = (C::MPI_Scan_t *) Symbol::loadNextSymbol(__func__);
			err = (*symbol)(sendbuf, recvbuf, count, datatype, op, comm);
		}
		return err;
	}
}

#endif // MPI_VERSION

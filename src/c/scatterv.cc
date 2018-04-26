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
	int MPI_Scatterv(
			const void* sendbuf, const int sendcounts[], const int displs[], MPI_Datatype sendtype,
			void* recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
	{
		int err = MPI_SUCCESS;
		if (C::Environment::isEnabled()) {
			MPI_Request request;
			err = MPI_Iscatterv(sendbuf, sendcounts, displs, sendtype,
					recvbuf, recvcount, recvtype, root, comm, &request);
			C::processRequest(request);
		} else {
			static C::MPI_Scatterv_t *symbol = (C::MPI_Scatterv_t *) Symbol::loadNextSymbol(__func__);
			err = (*symbol)(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm);
		}
		return err;
	}
} // extern C

#endif // MPI_VERSION


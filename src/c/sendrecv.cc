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
	int MPI_Sendrecv(
			MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag,
			void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag,
			MPI_Comm comm, MPI_Status *status)
	{
		int err = MPI_SUCCESS;
		if (C::Environment::isEnabled()) {
			MPI_Request requests[2];
			err = MPI_Irecv(recvbuf, recvcount, recvtype, source, recvtag, comm, &requests[0]);
			err = MPI_Isend(sendbuf, sendcount, sendtype, dest, sendtag, comm, &requests[1]);
			
			if (status != MPI_STATUS_IGNORE) {
				MPI_Status statuses[2];
				C::processRequests({requests, 2}, statuses);
				*status = statuses[0];
			} else {
				C::processRequests({requests, 2});
			}
		} else {
			static C::MPI_Sendrecv_t *symbol = (C::MPI_Sendrecv_t *) Symbol::loadNextSymbol(__func__);
			err = (*symbol)(sendbuf, sendcount, sendtype, dest, sendtag,
					recvbuf, recvcount, recvtype, source, recvtag,
					comm, status);
		}
		return err;
	}
} // extern C


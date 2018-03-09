/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "definitions.h"
#include "environment.h"
#include "process_request.h"
#include "symbols.h"

extern "C" {
	void mpi_irecv_(
			void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *src, MPI_Fint *tag,
			MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err);
	
	void mpi_isend_(
			MPI3CONST void *sendbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest,
			MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err );
	
	void mpi_sendrecv_(
			MPI3CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, MPI_Fint *dest, MPI_Fint *sendtag,
			void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *source, MPI_Fint *recvtag,
			MPI_Fint *comm, MPI_Fint *status, MPI_Fint *err)
	{
		if (Fortran::Environment::isEnabled()) {
			MPI_Fint requests[2];
			mpi_irecv_(recvbuf, recvcount, recvtype, source, recvtag, comm, &requests[0], err);
			mpi_isend_(sendbuf, sendcount, sendtype, dest, sendtag, comm, &requests[1], err);
			
			if (status != MPI_F_STATUS_IGNORE) {
				MPI_Fint statuses[2];
				Fortran::processRequests({requests, 2}, statuses);
				*status = statuses[0];
			} else {
				Fortran::processRequests({requests, 2});
			}
		} else {
			static Fortran::mpi_sendrecv_t *symbol = (Fortran::mpi_sendrecv_t *) Symbol::loadNextSymbol(__func__);
			(*symbol)(sendbuf, sendcount, sendtype, dest, sendtag,
					recvbuf, recvcount, recvtype, source, recvtag,
					comm, status, err);
		}
	}
} // extern C


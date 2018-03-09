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
	void mpi_irecv_(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag,
			MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err);

	void mpi_recv_(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *err)
	{
		if (Fortran::Environment::isEnabled()) {
			MPI_Fint request;
			mpi_irecv_(buf, count, datatype, source, tag, comm, &request, err);
			Fortran::processRequest(request, status);
		} else {
			static Fortran::mpi_recv_t *symbol = (Fortran::mpi_recv_t *) Symbol::loadNextSymbol(__func__);
			(*symbol)(buf, count, datatype, source, tag, comm, status, err);
		}
		
	}
} // extern C


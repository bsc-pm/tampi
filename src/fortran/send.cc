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
	void mpi_isend_(MPI3CONST void *buf, MPI_Fint *count, MPI_Fint *datatype,
			MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm,
			MPI_Fint *request, MPI_Fint *err);
	
	void mpi_send_(MPI3CONST void *buf, MPI_Fint *count, MPI_Fint *datatype,
			MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *err)
	{
		if (Fortran::Environment::isEnabled()) {
			MPI_Fint request;
			mpi_isend_(buf, count, datatype, dest, tag, comm, &request, err);
			Fortran::processRequest(request);
		} else {
			static Fortran::mpi_send_t *symbol = (Fortran::mpi_send_t *) Symbol::loadNextSymbol(__func__);
			(*symbol)(buf, count, datatype, dest, tag, comm, err);
		}
	}
} // extern C


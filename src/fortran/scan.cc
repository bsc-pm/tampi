/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h> // defines MPI_VERSION

#if MPI_VERSION >=3

#include "definitions.h"
#include "environment.h"
#include "process_request.h"
#include "symbols.h"

extern "C" {
	void mpi_iscan_(const void *sendbuf, void *recvbuf,
			MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op,
			MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err);

	void mpi_scan_(const void *sendbuf, void *recvbuf,
			MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op,
			MPI_Fint *comm, MPI_Fint *err)
	{
		if (Fortran::Environment::isEnabled()) {
			MPI_Fint request;
			mpi_iscan_(sendbuf, recvbuf, count, datatype, op, comm, &request, err);
			Fortran::processRequest(request);
		} else {
			static Fortran::mpi_scan_t *symbol = (Fortran::mpi_scan_t *) Symbol::loadNextSymbol(__func__);
			(*symbol)(sendbuf, recvbuf, count, datatype, op, comm, err);
		}
	}
} // extern C

#endif // MPI_VERSION


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
	void mpi_ibarrier_(MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err);
	
	void mpi_barrier_(MPI_Fint *comm, MPI_Fint *err)
	{
		if (Fortran::Environment::isEnabled()) {
			MPI_Fint request;
			mpi_ibarrier_(comm, &request, err);
			Fortran::processRequest(request);
		} else {
			static Fortran::mpi_barrier_t *symbol = (Fortran::mpi_barrier_t *) Symbol::loadNextSymbol(__func__);
			(*symbol)(comm, err);
		}
	}
} // extern C

#endif // MPI_VERSION


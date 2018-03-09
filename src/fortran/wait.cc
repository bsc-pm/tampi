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
	void mpi_wait_(MPI_Fint *request, MPI_Fint *status, MPI_Fint *err)
	{
		if (Fortran::Environment::isEnabled()) {
			Fortran::processRequest(*request, status);
		} else {
			static Fortran::mpi_wait_t *symbol = (Fortran::mpi_wait_t *) Symbol::loadNextSymbol(__func__);
			(*symbol)(request, status, err);
		}
	}
} // extern C


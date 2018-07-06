/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "include/TAMPI.h"

#include "definitions.h"
#include "environment.h"
#include "process_request.h"
#include "symbols.h"

extern "C" {
	void mpi_wait_(MPI_Fint *request, MPI_Fint *status, MPI_Fint *err);
	
	void tampi_iwait_(MPI_Fint *request, MPI_Fint *status, MPI_Fint *err)
	{
		if (Fortran::Environment::isEnabled() && !Fortran::Environment::inSerialContext()) {
			Fortran::processRequest(*request, status, /* Do not block */ false);
			*err = MPI_SUCCESS;
		} else {
			mpi_wait_(request, status, err);
		}
	}
} // extern C


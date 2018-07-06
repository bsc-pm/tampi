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
	void mpi_waitall_(MPI_Fint *count, MPI_Fint array_of_requests[], MPI_Fint *array_of_statuses, MPI_Fint *err);
	
	void tampi_iwaitall_(MPI_Fint *count, MPI_Fint array_of_requests[], MPI_Fint *array_of_statuses, MPI_Fint *err)
	{
		if (Fortran::Environment::isEnabled() && !Fortran::Environment::inSerialContext()) {
			Fortran::processRequests({array_of_requests, *count}, array_of_statuses, /* Do not block */ false);
			*err = MPI_SUCCESS;
		} else {
			mpi_waitall_(count, array_of_requests, array_of_statuses, err);
		}
	}
} // extern C


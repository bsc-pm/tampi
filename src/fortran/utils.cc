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
	void mpi_query_thread_(MPI_Fint *provided, MPI_Fint *err)
	{
		if (Fortran::Environment::isEnabled()) {
			*provided = MPI_TASK_MULTIPLE;
			*err = MPI_SUCCESS;
		} else {
			static Fortran::mpi_query_thread_t *symbol = (Fortran::mpi_query_thread_t *) Symbol::loadNextSymbol(__func__);
			(*symbol)(provided, err);
		}
	}
} // extern C


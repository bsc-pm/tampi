/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <mpi.h>
#include <dlfcn.h>
#include <cassert>

#include "include/TAMPI.h"

#include "definitions.h"
#include "environment.h"
#include "symbols.h"

extern "C" {

void mpi_init_(MPI_Fint *err)
{
	static Fortran::mpi_init_t *symbol = (Fortran::mpi_init_t *) Symbol::loadNextSymbol(__func__);
	
	// Call to MPI_Init
	(*symbol)(err);
	
	// Disable the interoperability
	Fortran::Environment::disable();
}

void mpi_init_thread_(MPI_Fint *required, MPI_Fint *provided, MPI_Fint *err)
{
	static Fortran::mpi_init_thread_t *symbol = (Fortran::mpi_init_thread_t *) Symbol::loadNextSymbol(__func__);
	
	/* NOTE: This should be modified when MPI_TASK_MULTIPLE
 	 * mode is supported by MPI */
	MPI_Fint actualRequired = *required;
	if (actualRequired == MPI_TASK_MULTIPLE) {
		actualRequired = MPI_THREAD_MULTIPLE;
	}
	
	// Call to MPI_Init_thread
	(*symbol)(&actualRequired, provided, err);
	if (*err != MPI_SUCCESS) return;
	
	if (*required == MPI_TASK_MULTIPLE && *provided == MPI_THREAD_MULTIPLE) {
		MPI_F_REQUEST_NULL = MPI_Request_c2f(MPI_REQUEST_NULL);
		
		Fortran::Environment::enable();
		Fortran::Environment::initialize();
		*provided = MPI_TASK_MULTIPLE;
	} else {
		Fortran::Environment::disable();
	}
}

void mpi_finalize_(MPI_Fint *err)
{
	static Fortran::mpi_finalize_t *symbol = (Fortran::mpi_finalize_t *) Symbol::loadNextSymbol(__func__);
	
	// Call MPI_Finalize
	(*symbol)(err);
	
	// Finalize the interoperability
	if (Fortran::Environment::isEnabled()) {
		Fortran::Environment::finalize();
	}
}

} // extern C

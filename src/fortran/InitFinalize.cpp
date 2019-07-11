/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2019 Barcelona Supercomputing Center (BSC)
*/

#include <config.h>

#include <mpi.h>
#include <dlfcn.h>
#include <cassert>

#include "include/TAMPI_Decl.h"

#include "Definitions.hpp"
#include "Environment.hpp"
#include "Symbol.hpp"

extern "C" {

void mpi_init_(MPI_Fint *err)
{
	static mpi_init_t *symbol = (mpi_init_t *) Symbol::loadNextSymbol(__func__);
	
	// Disable both TAMPI modes
	Environment<Fortran>::initialize(false, false);
	
	// Call to MPI_Init
	(*symbol)(err);
}

void mpi_init_thread_(MPI_Fint *required, MPI_Fint *provided, MPI_Fint *err)
{
	static mpi_init_thread_t *symbol = (mpi_init_thread_t *) Symbol::loadNextSymbol(__func__);
	
	// Assuming that MPI does not provide MPI_TASK_MULTIPLE
	MPI_Fint irequired = *required;
	if (irequired == MPI_TASK_MULTIPLE) {
		irequired = MPI_THREAD_MULTIPLE;
	}
	
	// Call to MPI_Init_thread
	(*symbol)(&irequired, provided, err);
	if (*err != MPI_SUCCESS) return;
	
	bool enableBlockingMode = false;
	bool enableNonBlockingMode = false;
	if (*provided == MPI_THREAD_MULTIPLE) {
#if HAVE_BLOCKING_MODE
		if (*required == MPI_TASK_MULTIPLE) {
			enableBlockingMode = true;
		}
#endif
#if HAVE_NONBLOCKING_MODE
		enableNonBlockingMode = true;
#endif
	}
	
	Environment<Fortran>::initialize(enableBlockingMode, enableNonBlockingMode);
	if (enableBlockingMode) {
		*provided = MPI_TASK_MULTIPLE;
	}
}

void mpi_finalize_(MPI_Fint *err)
{
	static mpi_finalize_t *symbol = (mpi_finalize_t *) Symbol::loadNextSymbol(__func__);
	
	// Call MPI_Finalize
	(*symbol)(err);
	if (*err != MPI_SUCCESS) return;
	
	// Finalize the interoperability
	Environment<Fortran>::finalize();
}

} // extern C

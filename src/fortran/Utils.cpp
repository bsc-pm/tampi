/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "include/TAMPI.h"

#include "Definitions.hpp"
#include "Environment.hpp"
#include "Symbols.hpp"

extern "C" {
	void mpi_query_thread_(MPI_Fint *provided, MPI_Fint *err)
	{
		if (Environment<Fortran>::isBlockingEnabled()) {
			*provided = MPI_TASK_MULTIPLE;
			*err = MPI_SUCCESS;
		} else {
			static mpi_query_thread_t *symbol = (mpi_query_thread_t *) Symbol::loadNextSymbol(__func__);
			(*symbol)(provided, err);
		}
	}
} // extern C


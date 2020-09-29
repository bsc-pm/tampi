/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2020 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "include/TAMPI_Decl.h"

#include "Definitions.hpp"
#include "Environment.hpp"
#include "Symbol.hpp"

#pragma GCC visibility push(default)

extern "C" {
	void mpi_query_thread_(MPI_Fint *provided, MPI_Fint *err)
	{
		if (Environment::isBlockingEnabled()) {
			*provided = MPI_TASK_MULTIPLE;
			*err = MPI_SUCCESS;
		} else {
			static mpi_query_thread_t *symbol = (mpi_query_thread_t *) Symbol::load(__func__);
			(*symbol)(provided, err);
		}
	}

	void tampi_blocking_enabled(MPI_Fint *flag, MPI_Fint *err)
	{
		assert(flag != NULL);
		*flag = Environment::isBlockingEnabled() ? 1 : 0;
		*err = MPI_SUCCESS;
	}

	void tampi_nonblocking_enabled(MPI_Fint *flag, MPI_Fint *err)
	{
		assert(flag != NULL);
		*flag = Environment::isNonBlockingEnabled() ? 1 : 0;
		*err = MPI_SUCCESS;
	}
} // extern C

#pragma GCC visibility pop

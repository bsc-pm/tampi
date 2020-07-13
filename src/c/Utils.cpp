/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2020 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "include/TAMPI_Decl.h"

#include "Definitions.hpp"
#include "Environment.hpp"
#include "Symbol.hpp"

extern "C" {
	int MPI_Query_thread(int *provided)
	{
		int err = MPI_SUCCESS;
		if (Environment::isBlockingEnabled()) {
			assert(provided != NULL);
			*provided = MPI_TASK_MULTIPLE;
		} else {
			static MPI_Query_thread_t *symbol = (MPI_Query_thread_t *) Symbol::load(__func__);
			err = (*symbol)(provided);
		}
		return err;
	}

	int TAMPI_Blocking_enabled(int *flag)
	{
		assert(flag != NULL);
		*flag = Environment::isBlockingEnabled() ? 1 : 0;
		return MPI_SUCCESS;
	}

	int TAMPI_Nonblocking_enabled(int *flag)
	{
		assert(flag != NULL);
		*flag = Environment::isNonBlockingEnabled() ? 1 : 0;
		return MPI_SUCCESS;
	}
} // extern C


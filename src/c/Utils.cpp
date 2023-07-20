/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "TAMPI_Decl.h"

#include "Environment.hpp"
#include "Interface.hpp"
#include "Symbol.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {
	int MPI_Query_thread(int *provided)
	{
		assert(provided != nullptr);

		int err = MPI_SUCCESS;
		if (Environment::isBlockingEnabled()) {
			*provided = MPI_TASK_MULTIPLE;
		} else {
			static MPI_Query_thread_t *symbol = (MPI_Query_thread_t *) Symbol::load(__func__);
			err = (*symbol)(provided);
		}
		return err;
	}

	int TAMPI_Blocking_enabled(int *flag)
	{
		assert(flag != nullptr);

		*flag = Environment::isBlockingEnabled() ? 1 : 0;
		return MPI_SUCCESS;
	}

	int TAMPI_Nonblocking_enabled(int *flag)
	{
		assert(flag != nullptr);

		*flag = Environment::isNonBlockingEnabled() ? 1 : 0;
		return MPI_SUCCESS;
	}

	int TAMPI_Property_get(int property, int *value)
	{
		assert(value != nullptr);

		// There is no valid property yet
		return MPI_ERR_ARG;
	}

	int TAMPI_Property_set(int property, int value)
	{
		// There is no valid property yet
		return MPI_ERR_ARG;
	}
} // extern C

#pragma GCC visibility pop

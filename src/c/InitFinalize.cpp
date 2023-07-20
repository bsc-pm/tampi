/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#include <config.h>

#include <cassert>
#include <dlfcn.h>
#include <mpi.h>
#include <stdarg.h>

#include "TAMPI_Decl.h"

#include "Environment.hpp"
#include "Interface.hpp"
#include "Symbol.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {
	int MPI_Init(int *argc, char ***argv)
	{
		static MPI_Init_t *symbol = (MPI_Init_t *) Symbol::load(__func__);

		// Call MPI_Init
		int err = (*symbol)(argc, argv);
		if (err != MPI_SUCCESS)
			return err;

		int provided;
		int required = MPI_THREAD_SINGLE;

		// Prepare the MPI environment
		Environment::preinitialize(required);

		// Initialize the library; no mode is enabled
		Environment::initialize(required, &provided);

		return MPI_SUCCESS;
	}

	int MPI_Init_thread(int *argc, char ***argv, int required, int *provided)
	{
		static MPI_Init_thread_t *symbol = (MPI_Init_thread_t *) Symbol::load(__func__);

		// Assuming that MPI does not provide MPI_TASK_MULTIPLE
		int irequired = required;
		if (required == MPI_TASK_MULTIPLE)
			irequired = MPI_THREAD_MULTIPLE;

		// Call MPI_Init_thread
		int err = (*symbol)(argc, argv, irequired, provided);
		if (err != MPI_SUCCESS)
			return err;

		// Prepare the MPI enviornment
		Environment::preinitialize(*provided);

		// Initialize the library
		Environment::initialize(required, provided);

		return MPI_SUCCESS;
	}

	int MPI_Finalize(void)
	{
		static MPI_Finalize_t *symbol = (MPI_Finalize_t *) Symbol::load(__func__);

		// Call MPI_Finalize
		int err = (*symbol)();
		if (err != MPI_SUCCESS)
			return err;

		// Finalize the library
		Environment::finalize();

		return MPI_SUCCESS;
	}
} // extern C

#pragma GCC visibility pop

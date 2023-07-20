/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#include <config.h>

#include <mpi.h>
#include <dlfcn.h>
#include <cassert>

#include "TAMPI_Decl.h"

#include "Environment.hpp"
#include "Interface.hpp"
#include "Symbol.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {
	void mpi_init_(MPI_Fint *err)
	{
		static mpi_init_t *symbol = (mpi_init_t *) Symbol::load(__func__);

		// Call to MPI_Init
		(*symbol)(err);
		if (*err != MPI_SUCCESS)
			return;

		int provided;
		int required = MPI_THREAD_SINGLE;

		// Prepare the MPI environment
		Environment::preinitialize(required);

		// Initialize the library; no mode is enabled
		Environment::initialize(required, &provided);
	}

	void mpi_init_thread_(MPI_Fint *required, MPI_Fint *provided, MPI_Fint *err)
	{
		static mpi_init_thread_t *symbol = (mpi_init_thread_t *) Symbol::load(__func__);

		// Assuming that MPI does not provide MPI_TASK_MULTIPLE
		MPI_Fint irequired = *required;
		if (irequired == MPI_TASK_MULTIPLE) {
			irequired = MPI_THREAD_MULTIPLE;
		}

		// Call to MPI_Init_thread
		(*symbol)(&irequired, provided, err);
		if (*err != MPI_SUCCESS)
			return;

		// Prepare the MPI enviornment
		Environment::preinitialize(*provided);

		// Initialize the library
		Environment::initialize(*required, provided);
	}

	void mpi_finalize_(MPI_Fint *err)
	{
		static mpi_finalize_t *symbol = (mpi_finalize_t *) Symbol::load(__func__);

		// Call MPI_Finalize
		(*symbol)(err);
		if (*err != MPI_SUCCESS)
			return;

		// Finalize the library
		Environment::finalize();
	}
} // extern C

#pragma GCC visibility pop

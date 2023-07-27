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

		// Retrieve the default thread level
		int provided;
		mpi_query_thread_(&provided, err);
		if (*err != MPI_SUCCESS)
			return;

		// Prepare the MPI environment
		Environment::preinitialize(provided);

		// Attempt to auto initialize the library; no mode is enabled
		Environment::initialize(provided, &provided, /* auto */ true);
	}

	void mpi_init_thread_(MPI_Fint *required, MPI_Fint *provided, MPI_Fint *err)
	{
		static mpi_init_thread_t *symbol = (mpi_init_thread_t *) Symbol::load(__func__);

		// When TAMPI is in explicit initialization mode, the MPI_Init_thread acts
		// as the standard call and does not support MPI_TASK_MULTIPLE. In such
		// case, MPI_Init_thread must be called with MPI_THREAD_MULTIPLE, and then,
		// TAMPI_Init can be called with MPI_TASK_MULTIPLE
		if (!Environment::isAutoInitializeEnabled() && *required == MPI_TASK_MULTIPLE)
			ErrorHandler::fail("The MPI_TASK_MULTIPLE must be passed to TAMPI_Init");

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

		// Attempt to auto initialize the library
		Environment::initialize(*required, provided, /* auto */ true);
	}

	void mpi_finalize_(MPI_Fint *err)
	{
		static mpi_finalize_t *symbol = (mpi_finalize_t *) Symbol::load(__func__);

		// Call MPI_Finalize
		(*symbol)(err);
		if (*err != MPI_SUCCESS)
			return;

		// Attempt to auto finalize the library
		Environment::finalize(/* auto */ true);
	}

	void tampi_init_(MPI_Fint *required, MPI_Fint *provided, MPI_Fint *err)
	{
		*err = MPI_SUCCESS;

		// Explicitly initialize the library
		Environment::initialize(*required, provided, /* auto */ false);
	}

	void tampi_finalize_(MPI_Fint *err)
	{
		*err = MPI_SUCCESS;

		// Explicitly finalize the library
		Environment::finalize(/* auto */ false);
	}
} // extern C

#pragma GCC visibility pop

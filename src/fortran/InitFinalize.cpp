/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2022 Barcelona Supercomputing Center (BSC)
*/

#include <config.h>

#include <mpi.h>
#include <dlfcn.h>
#include <cassert>

#include "include/TAMPI_Decl.h"

#include "Environment.hpp"
#include "Interface.hpp"
#include "Symbol.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {
	void mpi_init_(MPI_Fint *err)
	{
		static mpi_init_t *symbol = (mpi_init_t *) Symbol::load(__func__);

		// Disable both TAMPI modes
		Environment::initialize(false, false);

		// Call to MPI_Init
		(*symbol)(err);
		if (*err != MPI_SUCCESS) return;
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
		if (*err != MPI_SUCCESS) return;

		bool enableBlockingMode = false;
		bool enableNonBlockingMode = false;
		if (*provided == MPI_THREAD_MULTIPLE) {
#ifndef DISABLE_BLOCKING_MODE
			if (*required == MPI_TASK_MULTIPLE) {
				enableBlockingMode = true;
			}
#endif
#ifndef DISABLE_NONBLOCKING_MODE
			enableNonBlockingMode = true;
#endif
		}

		Environment::initialize(enableBlockingMode, enableNonBlockingMode);
		if (enableBlockingMode) {
			*provided = MPI_TASK_MULTIPLE;
		}
	}

	void mpi_finalize_(MPI_Fint *err)
	{
		static mpi_finalize_t *symbol = (mpi_finalize_t *) Symbol::load(__func__);

		// Call MPI_Finalize
		(*symbol)(err);
		if (*err != MPI_SUCCESS) return;

		// Finalize the interoperability
		Environment::finalize();
	}
} // extern C

#pragma GCC visibility pop

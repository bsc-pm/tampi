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

	// Retrieve the default thread level
	int provided;
	err = MPI_Query_thread(&provided);
	if (err != MPI_SUCCESS)
		return err;

	// Prepare the MPI environment
	Environment::preinitialize(provided);

	// Attempt to auto initialize the library; no mode is enabled
	Environment::initialize(provided, &provided, /* auto */ true);

	return MPI_SUCCESS;
}

int MPI_Init_thread(int *argc, char ***argv, int required, int *provided)
{
	static MPI_Init_thread_t *symbol = (MPI_Init_thread_t *) Symbol::load(__func__);

	// When TAMPI is in explicit initialization mode, the MPI_Init_thread acts
	// as the standard call and does not support MPI_TASK_MULTIPLE. In such
	// case, MPI_Init_thread must be called with MPI_THREAD_MULTIPLE, and then,
	// TAMPI_Init can be called with MPI_TASK_MULTIPLE
	if (!Environment::isAutoInitializeEnabled() && required == MPI_TASK_MULTIPLE)
		ErrorHandler::fail("The MPI_TASK_MULTIPLE must be passed to TAMPI_Init");

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

	// Attempt to auto initialize the library
	Environment::initialize(required, provided, /* auto */ true);

	return MPI_SUCCESS;
}

int MPI_Finalize(void)
{
	static MPI_Finalize_t *symbol = (MPI_Finalize_t *) Symbol::load(__func__);

	// Call MPI_Finalize
	int err = (*symbol)();
	if (err != MPI_SUCCESS)
		return err;

	// Attempt to finalize the library
	Environment::finalize(/* auto */ true);

	return MPI_SUCCESS;
}

int TAMPI_Init(int required, int *provided)
{
	// Explicitly initialize the library
	Environment::initialize(required, provided, /* auto */ false);

	return MPI_SUCCESS;
}

int TAMPI_Finalize(void)
{
	// Explicitly finalize the library
	Environment::finalize(/* auto */ false);

	return MPI_SUCCESS;
}

} // extern C

#pragma GCC visibility pop

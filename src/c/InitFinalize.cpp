/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2024 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "TAMPI_Decl.h"

#include "Declarations.hpp"
#include "Environment.hpp"
#include "Interface.hpp"
#include "Symbol.hpp"
#include "util/ErrorHandler.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {

int MPI_Init(int *argc, char ***argv)
{
	static Symbol<Prototypes<C>::mpi_init_t> symbol(__func__);

	// Call MPI_Init
	int err = symbol(argc, argv);
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
	static Symbol<Prototypes<C>::mpi_init_thread_t> symbol(__func__);

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
	int err = symbol(argc, argv, irequired, provided);
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
	static Symbol<Prototypes<C>::mpi_finalize_t> symbol(__func__);

	// Call MPI_Finalize
	int err = symbol();
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

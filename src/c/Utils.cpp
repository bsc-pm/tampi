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

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {

int MPI_Query_thread(int *provided)
{
	assert(provided != nullptr);

	int blocking = 0;
	int err = Environment::getProperty(TAMPI_PROPERTY_BLOCKING_MODE, &blocking);
	if (err)
		return MPI_ERR_ARG;

	if (blocking) {
		*provided = MPI_TASK_MULTIPLE;
	} else {
		static Symbol<Prototypes<C>::mpi_query_thread_t> symbol(__func__);
		return symbol(provided);
	}
	return MPI_SUCCESS;
}

int TAMPI_Blocking_enabled(int *flag)
{
	assert(flag != nullptr);

	int err = Environment::getProperty(TAMPI_PROPERTY_BLOCKING_MODE, flag);
	if (err)
		return MPI_ERR_ARG;

	return MPI_SUCCESS;
}

int TAMPI_Nonblocking_enabled(int *flag)
{
	assert(flag != nullptr);

	int err = Environment::getProperty(TAMPI_PROPERTY_NONBLOCKING_MODE, flag);
	if (err)
		return MPI_ERR_ARG;

	return MPI_SUCCESS;
}

int TAMPI_Property_get(int property, int *value)
{
	assert(value != nullptr);

	int err = Environment::getProperty(property, value);
	if (err)
		return MPI_ERR_ARG;

	return MPI_SUCCESS;
}

int TAMPI_Property_set(int property, int value)
{
	int err = Environment::setProperty(property, value);
	if (err)
		return MPI_ERR_ARG;

	return MPI_SUCCESS;
}

} // extern C

#pragma GCC visibility pop

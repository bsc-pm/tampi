/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
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

void mpi_query_thread_(MPI_Fint *provided, MPI_Fint *err)
{
	assert(provided != nullptr);
	assert(err != nullptr);

	*err = MPI_SUCCESS;

	int blocking = 0;
	int ierr = Environment::getProperty(TAMPI_PROPERTY_BLOCKING_MODE, &blocking);
	if (ierr) {
		*err = MPI_ERR_ARG;
		return;
	}

	if (blocking) {
		*provided = MPI_TASK_MULTIPLE;
	} else {
		static mpi_query_thread_t *symbol = (mpi_query_thread_t *) Symbol::load(__func__);
		(*symbol)(provided, err);
	}
}

void tampi_blocking_enabled_(MPI_Fint *flag, MPI_Fint *err)
{
	assert(flag != nullptr);
	assert(err != nullptr);

	*err = MPI_SUCCESS;

	int ierr = Environment::getProperty(TAMPI_PROPERTY_BLOCKING_MODE, flag);
	if (ierr)
		 *err = MPI_ERR_ARG;
}

void tampi_nonblocking_enabled_(MPI_Fint *flag, MPI_Fint *err)
{
	assert(flag != nullptr);
	assert(err != nullptr);

	*err = MPI_SUCCESS;

	int ierr = Environment::getProperty(TAMPI_PROPERTY_NONBLOCKING_MODE, flag);
	if (ierr)
		 *err = MPI_ERR_ARG;
}

void tampi_property_get_(MPI_Fint *property, MPI_Fint *value, MPI_Fint *err)
{
	assert(property != nullptr);
	assert(value != nullptr);
	assert(err != nullptr);

	*err = MPI_SUCCESS;

	int ierr = Environment::getProperty(*property, value);
	if (ierr)
		 *err = MPI_ERR_ARG;
}

void tampi_property_set_(MPI_Fint *property, MPI_Fint *value, MPI_Fint *err)
{
	assert(property != nullptr);
	assert(value != nullptr);
	assert(err != nullptr);

	*err = MPI_SUCCESS;

	int ierr = Environment::setProperty(*property, *value);
	if (ierr)
		 *err = MPI_ERR_ARG;
}

} // extern C

#pragma GCC visibility pop

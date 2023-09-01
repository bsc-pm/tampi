/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "TAMPI_Decl.h"

#include "Environment.hpp"
#include "Interface.hpp"
#include "RequestManager.hpp"
#include "Symbol.hpp"
#include "instrument/Instrument.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {

void mpi_wait_(MPI_Fint *request, MPI_Fint *status, MPI_Fint *err)
{
	if (Environment::isBlockingEnabledForCurrentThread()) {
		Instrument::Guard<LibraryInterface> instrGuard;
		RequestManager<Fortran>::processRequest(*request, status);
		*err = MPI_SUCCESS;
	} else {
		static mpi_wait_t *symbol = (mpi_wait_t *) Symbol::load(__func__);
		(*symbol)(request, status, err);
	}
}

void tampi_wait_internal_(MPI_Fint *request, MPI_Fint *status, MPI_Fint *err)
{
	*err = MPI_SUCCESS;

	if (!Environment::isNonBlockingEnabled())
		mpi_wait_(request, status, err);
}

} // extern C

#pragma GCC visibility pop

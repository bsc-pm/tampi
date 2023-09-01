/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "Declarations.hpp"
#include "Environment.hpp"
#include "Interface.hpp"
#include "RequestManager.hpp"
#include "Symbol.hpp"
#include "instrument/Instrument.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {

int MPI_Wait(MPI_Request *request, MPI_Status *status)
{
	int err = MPI_SUCCESS;
	if (Environment::isBlockingEnabledForCurrentThread()) {
		assert(request != NULL);
		Instrument::Guard<LibraryInterface> instrGuard;
		RequestManager<C>::processRequest(*request, status);
	} else {
		static MPI_Wait_t *symbol = (MPI_Wait_t *) Symbol::load(__func__);
		err = (*symbol)(request, status);
	}
	return err;
}

} // extern C

#pragma GCC visibility pop

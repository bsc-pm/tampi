/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#include <dlfcn.h>
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
	int TAMPI_Iwaitall(int count, MPI_Request requests[], MPI_Status statuses[])
	{
		if (Environment::isNonBlockingEnabled()) {
			Instrument::Guard<LibraryInterface> instrGuard;
			RequestManager<C>::processRequests({requests, count}, statuses, /* Non-blocking */ false);
		}
		return MPI_SUCCESS;
	}
} // extern C

#pragma GCC visibility pop

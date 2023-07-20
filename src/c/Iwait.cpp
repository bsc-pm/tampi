/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2022 Barcelona Supercomputing Center (BSC)
*/

#include <dlfcn.h>
#include <mpi.h>

#include "TAMPI_Decl.h"

#include "util/ErrorHandler.hpp"

#include "Environment.hpp"
#include "Interface.hpp"
#include "RequestManager.hpp"
#include "Symbol.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {
	int TAMPI_Iwait(MPI_Request *request, MPI_Status *status)
	{
		if (Environment::isNonBlockingEnabled()) {
			RequestManager<C>::processRequest(*request, status, /* Non-blocking */ false);
		}
		return MPI_SUCCESS;
	}
} // extern C

#pragma GCC visibility pop

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
#include "util/ErrorHandler.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {
	void tampi_iwaitall_(MPI_Fint *count, MPI_Fint requests[], MPI_Fint *statuses, MPI_Fint *err)
	{
		if (Environment::isNonBlockingEnabled()) {
			RequestManager<Fortran>::processRequests({requests, *count}, statuses, /* Non-blocking */ false);
		}
		*err = MPI_SUCCESS;
	}
} // extern C

#pragma GCC visibility pop

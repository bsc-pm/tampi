/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2020 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "include/TAMPI_Decl.h"

#include "util/ErrorHandler.hpp"

#include "Definitions.hpp"
#include "Environment.hpp"
#include "RequestManager.hpp"
#include "Symbol.hpp"

#include <assert.h>

#pragma GCC visibility push(default)

extern "C" {
	void tampi_iwait_(MPI_Fint *request, MPI_Fint *status, MPI_Fint *err)
	{
		if (Environment::isNonBlockingEnabled()) {
			RequestManager<Fortran>::processRequest(*request, status, /* Non-blocking */ false);
		}
		*err = MPI_SUCCESS;
	}
} // extern C

#pragma GCC visibility pop

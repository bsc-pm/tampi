/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <dlfcn.h>
#include <mpi.h>

#include "include/TAMPI_Decl.h"

#include "util/Error.hpp"

#include "Definitions.hpp"
#include "Environment.hpp"
#include "RequestManager.hpp"
#include "Symbols.hpp"

#include <assert.h>

extern "C" {
	int TAMPI_Iwaitall(int count, MPI_Request requests[], MPI_Status statuses[])
	{
		if (Environment<C>::isNonBlockingEnabled()) {
			RequestManager<C>::processRequests({requests, count}, statuses, /* Non-blocking */ false);
		}
		return MPI_SUCCESS;
	}
} // extern C


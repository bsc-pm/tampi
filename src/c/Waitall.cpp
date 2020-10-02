/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2020 Barcelona Supercomputing Center (BSC)
*/

#include <dlfcn.h>
#include <mpi.h>

#include "Definitions.hpp"
#include "Environment.hpp"
#include "RequestManager.hpp"
#include "Symbol.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {
	int MPI_Waitall(int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[])
	{
		int err = MPI_SUCCESS;
		if (Environment::isBlockingEnabled()) {
			RequestManager<C>::processRequests({array_of_requests, count}, array_of_statuses);
		} else {
			static MPI_Waitall_t *symbol = (MPI_Waitall_t *) Symbol::load(__func__);
			err = (*symbol)(count, array_of_requests, array_of_statuses);
		}
		return err;
	}
} // extern C

#pragma GCC visibility pop

/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <dlfcn.h>
#include <mpi.h>

#include "include/TAMPI.h"

#include "definitions.h"
#include "environment.h"
#include "process_request.h"
#include "symbols.h"

extern "C" {
	int TAMPI_Iwaitall(int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[])
	{
		if (C::Environment::isEnabled() && !C::Environment::inSerialContext()) {
			C::processRequests({array_of_requests, count}, array_of_statuses, /* Do not block */ false);
			return MPI_SUCCESS;
		}
		return MPI_Waitall(count, array_of_requests, array_of_statuses);
	}
} // extern C


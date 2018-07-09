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
	int TAMPI_Iwait(MPI_Request *request, MPI_Status *status)
	{
		if (C::Environment::isEnabled() && !C::Environment::inSerialContext()) {
			assert(request != NULL);
			C::processRequest(*request, status, /* Do not block */ false);
			return MPI_SUCCESS;
		}
		return MPI_Wait(request, status);
	}
} // extern C


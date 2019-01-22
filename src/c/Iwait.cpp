/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <dlfcn.h>
#include <mpi.h>

#include "include/TAMPI.h"

#include "util/error.hpp"

#include "Definitions.hpp"
#include "Environment.hpp"
#include "RequestManager.hpp"
#include "Symbols.hpp"

#include <assert.h>

extern "C" {
	int TAMPI_Iwait(MPI_Request *request, MPI_Status *status)
	{
		if (!Environment<C>::isNonBlockingEnabled()) {
			error::warn("TAMPI_Iwait calls not allowed");
			return MPI_ERR_OP;
		}
		
		RequestManager<C>::processRequest(*request, status, /* Non-blocking */ false);
		return MPI_SUCCESS;
	}
} // extern C


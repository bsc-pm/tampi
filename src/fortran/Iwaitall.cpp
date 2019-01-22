/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "include/TAMPI.h"

#include "util/array_view.hpp"
#include "util/error.hpp"

#include "Definitions.hpp"
#include "Environment.hpp"
#include "RequestManager.hpp"
#include "Symbols.hpp"

#include <assert.h>

extern "C" {
	void tampi_iwaitall_(MPI_Fint *count, MPI_Fint requests[], MPI_Fint *statuses, MPI_Fint *err)
	{
		if (!Environment<Fortran>::isNonBlockingEnabled()) {
			error::warn("TAMPI_Iwaitall calls not allowed");
			*err = MPI_ERR_OP;
			return;
		}
		
		RequestManager<Fortran>::processRequests({requests, *count}, statuses, /* Non-blocking */ false);
		*err = MPI_SUCCESS;
	}
} // extern C


/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "include/TAMPI.h"

#include "util/error.hpp"

#include "Definitions.hpp"
#include "Environment.hpp"
#include "RequestManager.hpp"
#include "Symbols.hpp"

#include <assert.h>

extern "C" {
	void tampi_iwait_(MPI_Fint *request, MPI_Fint *status, MPI_Fint *err)
	{
		if (!Environment<Fortran>::isNonBlockingEnabled()) {
			error::warn("TAMPI_Iwait calls not allowed");
			*err = MPI_ERR_OP;
			return;
		}
		
		RequestManager<Fortran>::processRequest(*request, status, /* Non-blocking */ false);
		*err = MPI_SUCCESS;
	}
} // extern C


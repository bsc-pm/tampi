/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2020 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "include/TAMPI_Decl.h"

#include "Definitions.hpp"
#include "Environment.hpp"
#include "RequestManager.hpp"
#include "Symbol.hpp"

extern "C" {
	void mpi_waitall_(MPI_Fint *count, MPI_Fint array_of_requests[], MPI_Fint *array_of_statuses, MPI_Fint *err)
	{
		if (Environment<Fortran>::isBlockingEnabled()) {
			RequestManager<Fortran>::processRequests({array_of_requests, *count}, array_of_statuses);
			*err = MPI_SUCCESS;
		} else {
			static mpi_waitall_t *symbol = (mpi_waitall_t *) Symbol::load(__func__);
			(*symbol)(count, array_of_requests, array_of_statuses, err);
		}
	}

	void tampi_waitall_internal_(MPI_Fint *count, MPI_Fint array_of_requests[], MPI_Fint *array_of_statuses, MPI_Fint *err)
	{
		*err = MPI_SUCCESS;
		if (!Environment<Fortran>::isNonBlockingEnabled()) {
			mpi_waitall_(count, array_of_requests, array_of_statuses, err);
		}
	}
} // extern C


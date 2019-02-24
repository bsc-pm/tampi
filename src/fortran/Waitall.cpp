/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "Definitions.hpp"
#include "Environment.hpp"
#include "RequestManager.hpp"
#include "Symbols.hpp"

extern "C" {
	void mpi_waitall_(MPI_Fint *count, MPI_Fint array_of_requests[], MPI_Fint *array_of_statuses, MPI_Fint *err)
	{
		if (Environment<Fortran>::isBlockingEnabled()) {
			RequestManager<Fortran>::processRequests({array_of_requests, *count}, array_of_statuses);
			*err = MPI_SUCCESS;
		} else {
			static mpi_waitall_t *symbol = (mpi_waitall_t *) Symbol::loadNextSymbol(__func__);
			(*symbol)(count, array_of_requests, array_of_statuses, err);
		}
	}
} // extern C


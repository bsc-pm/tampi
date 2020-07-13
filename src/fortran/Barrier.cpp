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
	void mpi_ibarrier_(MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err);

	void mpi_barrier_(MPI_Fint *comm, MPI_Fint *err)
	{
		if (Environment<Fortran>::isBlockingEnabled()) {
			MPI_Fint request;
			mpi_ibarrier_(comm, &request, err);
			if (*err == MPI_SUCCESS)
				RequestManager<Fortran>::processRequest(request);
		} else {
			static mpi_barrier_t *symbol = (mpi_barrier_t *) Symbol::load(__func__);
			(*symbol)(comm, err);
		}
	}

	void tampi_ibarrier_internal_(MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err)
	{
		mpi_ibarrier_(comm, request, err);

		if (Environment<Fortran>::isNonBlockingEnabled()) {
			if (*err == MPI_SUCCESS) {
				tampi_iwait_(request, MPI_F_STATUS_IGNORE, err);
			}
		}
	}
} // extern C


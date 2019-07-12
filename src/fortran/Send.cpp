/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2019 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "include/TAMPI_Decl.h"

#include "Definitions.hpp"
#include "Environment.hpp"
#include "RequestManager.hpp"
#include "Symbol.hpp"

extern "C" {
	void mpi_isend_(void *buf, MPI_Fint *count, MPI_Fint *datatype,
			MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm,
			MPI_Fint *request, MPI_Fint *err);
	
	void mpi_send_(void *buf, MPI_Fint *count, MPI_Fint *datatype,
			MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *err)
	{
		if (Environment<Fortran>::isBlockingEnabled()) {
			MPI_Fint request;
			mpi_isend_(buf, count, datatype, dest, tag, comm, &request, err);
			if (*err == MPI_SUCCESS)
				RequestManager<Fortran>::processRequest(request);
		} else {
			static mpi_send_t *symbol = (mpi_send_t *) Symbol::loadNextSymbol(__func__);
			(*symbol)(buf, count, datatype, dest, tag, comm, err);
		}
	}
	
	void tampi_isend_internal_(void *buf, MPI_Fint *count, MPI_Fint *datatype,
			MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm,
			MPI_Fint *request, MPI_Fint *err)
	{
		mpi_isend_(buf, count, datatype, dest, tag, comm, request, err);
		
		if (Environment<Fortran>::isNonBlockingEnabled()) {
			if (*err == MPI_SUCCESS) {
				tampi_iwait_(request, MPI_F_STATUS_IGNORE, err);
			}
		}
	}
} // extern C


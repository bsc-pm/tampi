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
	void mpi_issend_(MPI3CONST void *buf, MPI_Fint *count, MPI_Fint *datatype,
			MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm,
			MPI_Fint *request, MPI_Fint *err);
	
	void mpi_ssend_(MPI3CONST void *buf, MPI_Fint *count, MPI_Fint *datatype,
			MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *err)
	{
		if (Environment<Fortran>::isBlockingEnabled()) {
			MPI_Fint request;
			mpi_issend_(buf, count, datatype, dest, tag, comm, &request, err);
			if (*err == MPI_SUCCESS)
				RequestManager<Fortran>::processRequest(request);
		} else {
			static mpi_ssend_t *symbol = (mpi_ssend_t *) Symbol::loadNextSymbol(__func__);
			(*symbol)(buf, count, datatype, dest, tag, comm, err);
		}
	}
} // extern C


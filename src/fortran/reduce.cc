/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h> // defines MPI_VERSION

#if MPI_VERSION >=3

#include "definitions.h"
#include "environment.h"
#include "process_request.h"
#include "symbols.h"

extern "C" {
	void mpi_ireduce_(const void *sendbuf, void *recvbuf, MPI_Fint *count,
			MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root,
			MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err);

	void mpi_reduce_(const void *sendbuf, void *recvbuf, MPI_Fint *count,
			MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root,
			MPI_Fint *comm, MPI_Fint *err)
	{
		if (Fortran::Environment::isEnabled()) {
			MPI_Fint request;
			mpi_ireduce_(sendbuf, recvbuf, count, datatype, op, root, comm, &request, err);
			Fortran::processRequest(request);
		} else {
			static Fortran::mpi_reduce_t *symbol = (Fortran::mpi_reduce_t *) Symbol::loadNextSymbol(__func__);
			(*symbol)(sendbuf, recvbuf, count, datatype, op, root, comm, err);
		}
	}
} // extern C

#endif // MPI_VERSION


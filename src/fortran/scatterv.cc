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
	void mpi_iscatterv_(const void *sendbuf, const MPI_Fint sendcounts[], const MPI_Fint displs[], MPI_Fint *sendtype,
			void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root,
			MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err);

	void mpi_scatterv_(const void *sendbuf, const MPI_Fint sendcounts[], const MPI_Fint displs[], MPI_Fint *sendtype,
			void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *root, MPI_Fint *comm, MPI_Fint *err)
	{
		if (Fortran::Environment::isEnabled()) {
			MPI_Fint request;
			mpi_iscatterv_(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, &request, err);
			Fortran::processRequest(request);
		} else {
			static Fortran::mpi_scatterv_t *symbol = (Fortran::mpi_scatterv_t *) Symbol::loadNextSymbol(__func__);
			(*symbol)(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, comm, err);
		}
	}
} // extern C

#endif // MPI_VERSION


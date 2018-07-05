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
	void mpi_ialltoallw_(const void *sendbuf, const MPI_Fint sendcounts[],
			const MPI_Fint sdispls[], const MPI_Fint sendtypes[],
			void *recvbuf, const MPI_Fint recvcounts[],
			const MPI_Fint rdispls[], const MPI_Fint recvtypes[],
			MPI_Fint *comm, MPI_Fint *req, MPI_Fint *err);
	
	void mpi_alltoallw_(const void *sendbuf, const MPI_Fint sendcounts[],
			const MPI_Fint sdispls[], const MPI_Fint sendtypes[],
			void *recvbuf, const MPI_Fint recvcounts[],
			const MPI_Fint rdispls[], const MPI_Fint recvtypes[],
			MPI_Fint *comm, MPI_Fint *err)
	{
		if (Fortran::Environment::isEnabled()) {
			MPI_Fint request;
			mpi_ialltoallw_(sendbuf, sendcounts, sdispls, sendtypes,
					recvbuf, recvcounts, rdispls, recvtypes,
					comm, &request, err);
			Fortran::processRequest(request);
		} else {
			static Fortran::mpi_alltoallw_t *symbol = (Fortran::mpi_alltoallw_t *) Symbol::loadNextSymbol(__func__);
			(*symbol)(sendbuf, sendcounts, sdispls, sendtypes, recvbuf, recvcounts, rdispls, recvtypes, comm, err);
		}
	}
}

#endif // MPI_VERSION

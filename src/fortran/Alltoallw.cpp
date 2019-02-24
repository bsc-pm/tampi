/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h> // defines MPI_VERSION

#if MPI_VERSION >=3

#include "Definitions.hpp"
#include "Environment.hpp"
#include "RequestManager.hpp"
#include "Symbols.hpp"

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
		if (Environment<Fortran>::isBlockingEnabled()) {
			MPI_Fint request;
			mpi_ialltoallw_(sendbuf, sendcounts, sdispls, sendtypes,
					recvbuf, recvcounts, rdispls, recvtypes,
					comm, &request, err);
			if (*err == MPI_SUCCESS)
				RequestManager<Fortran>::processRequest(request);
		} else {
			static mpi_alltoallw_t *symbol = (mpi_alltoallw_t *) Symbol::loadNextSymbol(__func__);
			(*symbol)(sendbuf, sendcounts, sdispls, sendtypes, recvbuf, recvcounts, rdispls, recvtypes, comm, err);
		}
	}
}

#endif // MPI_VERSION

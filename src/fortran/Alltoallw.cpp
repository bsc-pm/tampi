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
	void mpi_ialltoallw_(void *sendbuf, MPI_Fint sendcounts[],
			MPI_Fint sdispls[], MPI_Fint sendtypes[],
			void *recvbuf, MPI_Fint recvcounts[],
			MPI_Fint rdispls[], MPI_Fint recvtypes[],
			MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err);

	void mpi_alltoallw_(void *sendbuf, MPI_Fint sendcounts[],
			MPI_Fint sdispls[], MPI_Fint sendtypes[],
			void *recvbuf, MPI_Fint recvcounts[],
			MPI_Fint rdispls[], MPI_Fint recvtypes[],
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
			static mpi_alltoallw_t *symbol = (mpi_alltoallw_t *) Symbol::load(__func__);
			(*symbol)(sendbuf, sendcounts, sdispls, sendtypes, recvbuf, recvcounts, rdispls, recvtypes, comm, err);
		}
	}

	void tampi_ialltoallw_internal_(void *sendbuf, MPI_Fint sendcounts[],
			MPI_Fint sdispls[], MPI_Fint sendtypes[],
			void *recvbuf, MPI_Fint recvcounts[],
			MPI_Fint rdispls[], MPI_Fint recvtypes[],
			MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err)
	{
		mpi_ialltoallw_(sendbuf, sendcounts, sdispls, sendtypes,
				recvbuf, recvcounts, rdispls, recvtypes,
				comm, request, err);

		if (Environment<Fortran>::isNonBlockingEnabled()) {
			if (*err == MPI_SUCCESS) {
				tampi_iwait_(request, MPI_F_STATUS_IGNORE, err);
			}
		}
	}
}


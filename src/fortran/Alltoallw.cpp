/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2022 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "include/TAMPI_Decl.h"

#include "Environment.hpp"
#include "Interface.hpp"
#include "RequestManager.hpp"
#include "Symbol.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

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
		if (Environment::isBlockingEnabled()) {
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

		if (Environment::isNonBlockingEnabled()) {
			if (*err == MPI_SUCCESS) {
				tampi_iwait_(request, MPI_F_STATUS_IGNORE, err);
			}
		}
	}
} // extern C

#pragma GCC visibility pop

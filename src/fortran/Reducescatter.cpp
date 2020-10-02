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

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {
	void mpi_ireduce_scatter_(void *sendbuf, void *recvbuf,
			MPI_Fint recvcounts[], MPI_Fint *datatype, MPI_Fint *op,
			MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err);

	void mpi_reduce_scatter_(void *sendbuf, void *recvbuf,
			MPI_Fint recvcounts[], MPI_Fint *datatype, MPI_Fint *op,
			MPI_Fint *comm, MPI_Fint *err)
	{
		if (Environment::isBlockingEnabled()) {
			MPI_Fint request;
			mpi_ireduce_scatter_(sendbuf, recvbuf, recvcounts, datatype, op, comm, &request, err);
			if (*err == MPI_SUCCESS)
				RequestManager<Fortran>::processRequest(request);
		} else {
			static mpi_reduce_scatter_t *symbol = (mpi_reduce_scatter_t *) Symbol::load(__func__);
			(*symbol)(sendbuf, recvbuf, recvcounts, datatype, op, comm, err);
		}
	}

	void tampi_ireduce_scatter_internal_(void *sendbuf, void *recvbuf,
			MPI_Fint recvcounts[], MPI_Fint *datatype, MPI_Fint *op,
			MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err)
	{
		mpi_ireduce_scatter_(sendbuf, recvbuf, recvcounts, datatype, op, comm, request, err);

		if (Environment::isNonBlockingEnabled()) {
			if (*err == MPI_SUCCESS) {
				tampi_iwait_(request, MPI_F_STATUS_IGNORE, err);
			}
		}
	}
} // extern C

#pragma GCC visibility pop

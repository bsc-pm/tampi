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
	void mpi_iexscan_(const void *sendbuf, void *recvbuf,
			MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op,
			MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err);

	void mpi_exscan_(const void *sendbuf, void *recvbuf,
			MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *op,
			MPI_Fint *comm, MPI_Fint *err)
	{
		if (Environment<Fortran>::isBlockingEnabled()) {
			MPI_Fint request;
			mpi_iexscan_(sendbuf, recvbuf, count, datatype, op, comm, &request, err);
			if (*err == MPI_SUCCESS)
				RequestManager<Fortran>::processRequest(request);
		} else {
			static mpi_exscan_t *symbol = (mpi_exscan_t *) Symbol::loadNextSymbol(__func__);
			(*symbol)(sendbuf, recvbuf, count, datatype, op, comm, err);
		}
	}
} // extern C

#endif // MPI_VERSION


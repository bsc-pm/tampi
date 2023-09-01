/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "TAMPI_Decl.h"

#include "Declarations.hpp"
#include "Environment.hpp"
#include "Interface.hpp"
#include "RequestManager.hpp"
#include "Symbol.hpp"
#include "instrument/Instrument.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {

void mpi_igatherv_(void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype,
		void *recvbuf, MPI_Fint recvcounts[], MPI_Fint displs[], MPI_Fint *recvtype,
		MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err);

void mpi_gatherv_(void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype,
		void *recvbuf, MPI_Fint recvcounts[], MPI_Fint displs[], MPI_Fint *recvtype,
		MPI_Fint *root, MPI_Fint *comm, MPI_Fint *err)
{
	if (Environment::isBlockingEnabledForCurrentThread()) {
		Instrument::Guard<LibraryInterface> instrGuard;
		Instrument::enter<IssueNonBlockingOp>();

		MPI_Fint request;
		mpi_igatherv_(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, &request, err);

		Instrument::exit<IssueNonBlockingOp>();

		if (*err == MPI_SUCCESS)
			RequestManager<Fortran>::processRequest(request);
	} else {
		static mpi_gatherv_t *symbol = (mpi_gatherv_t *) Symbol::load(__func__);
		(*symbol)(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, err);
	}
}

void tampi_igatherv_internal_(void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype,
		void *recvbuf, MPI_Fint recvcounts[], MPI_Fint displs[], MPI_Fint *recvtype,
		MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err)
{
	mpi_igatherv_(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, request, err);

	if (*err == MPI_SUCCESS)
		tampi_iwait_(request, MPI_F_STATUS_IGNORE, err);
}

} // extern C

#pragma GCC visibility pop

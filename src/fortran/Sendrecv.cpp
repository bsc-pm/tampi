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

void mpi_irecv_(
		void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *src, MPI_Fint *tag,
		MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err);

void mpi_isend_(
		void *sendbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest,
		MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err );

void mpi_sendrecv_(
		void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype, MPI_Fint *dest, MPI_Fint *sendtag,
		void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype, MPI_Fint *source, MPI_Fint *recvtag,
		MPI_Fint *comm, MPI_Fint *status, MPI_Fint *err)
{
	if (Environment::isBlockingEnabledForCurrentThread()) {
		Instrument::Guard<LibraryInterface> instrGuard;
		Instrument::enter<IssueNonBlockingOp>();

		MPI_Fint requests[2];
		mpi_irecv_(recvbuf, recvcount, recvtype, source, recvtag, comm, &requests[0], err);
		if (*err != MPI_SUCCESS)
			return;

		mpi_isend_(sendbuf, sendcount, sendtype, dest, sendtag, comm, &requests[1], err);
		if (*err != MPI_SUCCESS)
			return;

		Instrument::exit<IssueNonBlockingOp>();

		if (status != MPI_F_STATUS_IGNORE) {
			MPI_Fint statuses[2];
			RequestManager<Fortran>::processRequests({requests, 2}, statuses);
			*status = statuses[0];
		} else {
			RequestManager<Fortran>::processRequests({requests, 2});
		}
	} else {
		static mpi_sendrecv_t *symbol = (mpi_sendrecv_t *) Symbol::load(__func__);
		(*symbol)(sendbuf, sendcount, sendtype, dest, sendtag,
				recvbuf, recvcount, recvtype, source, recvtag,
				comm, status, err);
	}
}

} // extern C

#pragma GCC visibility pop

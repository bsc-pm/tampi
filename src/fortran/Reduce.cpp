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

void mpi_reduce_(void *sendbuf, void *recvbuf, MPI_Fint *count,
		MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root,
		MPI_Fint *comm, MPI_Fint *err)
{
	if (Environment::isBlockingEnabledForCurrentThread()) {
		Instrument::Guard<LibraryInterface> instrGuard;
		Instrument::enter<IssueNonBlockingOp>();

		static Symbol<mpi_ireduce_t> symbol("mpi_ireduce_");

		MPI_Fint request;
		symbol(sendbuf, recvbuf, count, datatype, op, root, comm, &request, err);

		Instrument::exit<IssueNonBlockingOp>();

		if (*err == MPI_SUCCESS)
			RequestManager<Fortran>::processRequest(request);
	} else {
		static Symbol<mpi_reduce_t> symbol(__func__);
		symbol(sendbuf, recvbuf, count, datatype, op, root, comm, err);
	}
}

void tampi_ireduce_internal_(void *sendbuf, void *recvbuf, MPI_Fint *count,
		MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *root,
		MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err)
{
	static Symbol<mpi_ireduce_t> symbol("mpi_ireduce_");

	symbol(sendbuf, recvbuf, count, datatype, op, root, comm, request, err);

	if (*err == MPI_SUCCESS)
		tampi_iwait_(request, MPI_F_STATUS_IGNORE, err);
}

} // extern C

#pragma GCC visibility pop

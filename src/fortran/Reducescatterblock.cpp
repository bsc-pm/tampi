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

void mpi_reduce_scatter_block_(void *sendbuf, void *recvbuf,
		MPI_Fint *recvcount, MPI_Fint *datatype, MPI_Fint *op,
		MPI_Fint *comm, MPI_Fint *err)
{
	if (Environment::isBlockingEnabledForCurrentThread()) {
		Instrument::Guard<LibraryInterface> instrGuard;
		Instrument::enter<IssueNonBlockingOp>();

		static Symbol<mpi_ireduce_scatter_block_t> symbol("mpi_ireduce_scatter_block_");

		MPI_Fint request;
		symbol(sendbuf, recvbuf, recvcount, datatype, op, comm, &request, err);

		Instrument::exit<IssueNonBlockingOp>();

		if (*err == MPI_SUCCESS)
			RequestManager<Fortran>::processRequest(request);
	} else {
		static Symbol<mpi_reduce_scatter_block_t> symbol(__func__);
		symbol(sendbuf, recvbuf, recvcount, datatype, op, comm, err);
	}
}

void tampi_ireduce_scatter_block_internal_(void *sendbuf, void *recvbuf,
		MPI_Fint *recvcount, MPI_Fint *datatype, MPI_Fint *op,
		MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err)
{
	static Symbol<mpi_ireduce_scatter_block_t> symbol("mpi_ireduce_scatter_block_");

	symbol(sendbuf, recvbuf, recvcount, datatype, op, comm, request, err);

	if (*err == MPI_SUCCESS)
		tampi_iwait_(request, MPI_F_STATUS_IGNORE, err);
}

} // extern C

#pragma GCC visibility pop

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

void mpi_recv_(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *err)
{
	if (Environment::isBlockingEnabledForCurrentThread()) {
		Instrument::Guard<LibraryInterface> instrGuard;
		Instrument::enter<IssueNonBlockingOp>();

		static Symbol<mpi_irecv_t> symbol("mpi_irecv_");

		MPI_Fint request;
		symbol(buf, count, datatype, source, tag, comm, &request, err);

		Instrument::exit<IssueNonBlockingOp>();

		if (*err == MPI_SUCCESS)
			RequestManager<Fortran>::processRequest(request, status);
	} else {
		static Symbol<mpi_recv_t> symbol(__func__);
		symbol(buf, count, datatype, source, tag, comm, status, err);
	}

}

void tampi_irecv_internal_(void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source,
		MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *status, MPI_Fint *err)
{
	static Symbol<mpi_irecv_t> symbol("mpi_irecv_");

	symbol(buf, count, datatype, source, tag, comm, request, err);

	if (*err == MPI_SUCCESS)
		tampi_iwait_(request, status, err);
}

} // extern C

#pragma GCC visibility pop

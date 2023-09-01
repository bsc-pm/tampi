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

void mpi_issend_(void *buf, MPI_Fint *count, MPI_Fint *datatype,
		MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm,
		MPI_Fint *request, MPI_Fint *err);

void mpi_ssend_(void *buf, MPI_Fint *count, MPI_Fint *datatype,
		MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *err)
{
	if (Environment::isBlockingEnabledForCurrentThread()) {
		Instrument::Guard<LibraryInterface> instrGuard;
		Instrument::enter<IssueNonBlockingOp>();

		MPI_Fint request;
		mpi_issend_(buf, count, datatype, dest, tag, comm, &request, err);

		Instrument::exit<IssueNonBlockingOp>();

		if (*err == MPI_SUCCESS)
			RequestManager<Fortran>::processRequest(request);
	} else {
		static mpi_ssend_t *symbol = (mpi_ssend_t *) Symbol::load(__func__);
		(*symbol)(buf, count, datatype, dest, tag, comm, err);
	}
}

void tampi_issend_internal_(void *buf, MPI_Fint *count, MPI_Fint *datatype,
		MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm,
		MPI_Fint *request, MPI_Fint *err)
{
	mpi_issend_(buf, count, datatype, dest, tag, comm, request, err);

	if (*err == MPI_SUCCESS)
		tampi_iwait_(request, MPI_F_STATUS_IGNORE, err);
}

} // extern C

#pragma GCC visibility pop

/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "Declarations.hpp"
#include "Environment.hpp"
#include "Interface.hpp"
#include "RequestManager.hpp"
#include "Symbol.hpp"
#include "instrument/Instrument.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {

int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
	int err = MPI_SUCCESS;
	if (Environment::isBlockingEnabledForCurrentThread()) {
		Instrument::Guard<LibraryInterface> instrGuard;
		Instrument::enter<IssueNonBlockingOp>();

		static Symbol<MPI_Irecv_t> symbol("MPI_Irecv");

		MPI_Request request;
		err = symbol(buf, count, datatype, source, tag, comm, &request);

		Instrument::exit<IssueNonBlockingOp>();

		if (err == MPI_SUCCESS)
			RequestManager<C>::processRequest(request, status);
	} else {
		static Symbol<MPI_Recv_t> symbol(__func__);
		err = symbol(buf, count, datatype, source, tag, comm, status);
	}

	return err;
}

} // extern C

#pragma GCC visibility pop

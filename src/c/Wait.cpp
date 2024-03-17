/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2024 Barcelona Supercomputing Center (BSC)
*/

#include <mpi.h>

#include "Declarations.hpp"
#include "Environment.hpp"
#include "Symbol.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {

int MPI_Wait(MPI_Request *request, MPI_Status *status)
{
	if (Environment::isBlockingEnabledForCurrentThread()) {
		ErrorHandler::fail(__func__, " with task-aware behavior not supported");
		return MPI_ERR_UNSUPPORTED_OPERATION;
	} else {
		static Symbol<Prototypes<C>::mpi_wait_t> symbol(__func__);
		return symbol(request, status);
	}
}

int MPI_Waitall(int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[])
{
	if (Environment::isBlockingEnabledForCurrentThread()) {
		ErrorHandler::fail(__func__, " with task-aware behavior not supported");
		return MPI_ERR_UNSUPPORTED_OPERATION;
	} else {
		static Symbol<Prototypes<C>::mpi_waitall_t> symbol(__func__);
		return symbol(count, array_of_requests, array_of_statuses);
	}
}

int TAMPI_Iwait(MPI_Request *, MPI_Status *)
{
	ErrorHandler::fail(__func__, " not supported");
	return MPI_ERR_UNSUPPORTED_OPERATION;
}

int TAMPI_Iwaitall(int, MPI_Request [], MPI_Status [])
{
	ErrorHandler::fail("TAMPI: ", __func__, " not supported");
	return MPI_ERR_UNSUPPORTED_OPERATION;
}

} // extern C

#pragma GCC visibility pop

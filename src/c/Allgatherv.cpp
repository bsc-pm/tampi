/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2022 Barcelona Supercomputing Center (BSC)
*/

#include <dlfcn.h>
#include <mpi.h>

#include "Environment.hpp"
#include "Interface.hpp"
#include "RequestManager.hpp"
#include "Symbol.hpp"

using namespace tampi;

#pragma GCC visibility push(default)

extern "C" {
	int MPI_Allgatherv(MPI3CONST void* sendbuf, int sendcount, MPI_Datatype sendtype,
			void* recvbuf, MPI3CONST int recvcounts[], MPI3CONST int displs[],
			MPI_Datatype recvtype, MPI_Comm comm)
	{
		int err = MPI_SUCCESS;
		if (Environment::isBlockingEnabled()) {
			MPI_Request request;
			err = MPI_Iallgatherv(sendbuf, sendcount, sendtype,
					recvbuf, recvcounts, displs, recvtype,
					comm, &request);
			if (err == MPI_SUCCESS)
				RequestManager<C>::processRequest(request);
		} else {
			static MPI_Allgatherv_t *symbol = (MPI_Allgatherv_t *) Symbol::load(__func__);
			err = (*symbol)(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
		}
		return err;
	}
} // extern C

#pragma GCC visibility pop

/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <dlfcn.h>
#include <mpi.h>

#if MPI_VERSION >=3

#include "Definitions.hpp"
#include "Environment.hpp"
#include "RequestManager.hpp"
#include "Symbols.hpp"

extern "C" {
	int MPI_Gather(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
			void *recvbuf, int recvcount, MPI_Datatype recvtype,
			int root, MPI_Comm comm)
	{
		int err = MPI_SUCCESS;
		if (Environment<C>::isBlockingEnabled()) {
			MPI_Request request;
			err = MPI_Igather(sendbuf, sendcount, sendtype,
					recvbuf, recvcount, recvtype,
					root, comm, &request);
			if (err == MPI_SUCCESS)
				RequestManager<C>::processRequest(request);
		} else {
			static MPI_Gather_t *symbol = (MPI_Gather_t *) Symbol::loadNextSymbol(__func__);
			err = (*symbol)(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm);
		}
		return err;
	}
} // extern C

#endif // MPI_VERSION


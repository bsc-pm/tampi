/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#include <dlfcn.h>
#include <mpi.h>

#include "definitions.h"
#include "environment.h"
#include "process_request.h"
#include "symbols.h"

extern "C" {
	int MPI_Sendrecv_replace(
			void *buf, int count, MPI_Datatype datatype, int dest, int sendtag,
			int source, int recvtag, MPI_Comm comm, MPI_Status *status)
	{
		int err = MPI_SUCCESS;
		if (C::Environment::isEnabled()) {
			if (source == MPI_PROC_NULL || dest == MPI_PROC_NULL || count == 0) {
				err = MPI_Sendrecv(
						buf, count, datatype, dest, sendtag,
						buf, count, datatype, source, recvtag,
						comm, status);
			} else {
				int packedSize;
				void *helperbuf = nullptr;
				
				// Setup a buffer for receiving
				int err = MPI_Pack_size(count, datatype, comm, &packedSize);
				int alloc = MPI_Alloc_mem(packedSize, MPI_INFO_NULL, &helperbuf);
				
				// Receive into temporary buffer
				if (err == MPI_SUCCESS  && alloc == MPI_SUCCESS) {
					err = MPI_Sendrecv(buf, count, datatype, dest, sendtag,
							helperbuf, packedSize, MPI_BYTE, source, recvtag,
							comm, status);
				}
				
				// Copy into user's buffer
				if (err == MPI_SUCCESS && alloc == MPI_SUCCESS) {
					int position = 0;
					err = MPI_Unpack(helperbuf, packedSize, &position,
							buf, count, datatype, comm);
				}
				
				// Release resources
				if (alloc == MPI_SUCCESS) {
					err = MPI_Free_mem(helperbuf);
				}
			}
		} else {
			static C::MPI_Sendrecv_replace_t *symbol = (C::MPI_Sendrecv_replace_t *) Symbol::loadNextSymbol(__func__);
			err = (*symbol)(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);
		}
		return err;
	}
} // extern C


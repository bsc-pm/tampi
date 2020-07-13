/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2020 Barcelona Supercomputing Center (BSC)
*/

#include <dlfcn.h>
#include <mpi.h>

#include "Definitions.hpp"
#include "Environment.hpp"
#include "RequestManager.hpp"
#include "Symbol.hpp"

extern "C" {
	int MPI_Sendrecv_replace(void *buf, int count, MPI_Datatype datatype, int dest,
			int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status)
	{
		int err = MPI_SUCCESS;
		if (Environment::isBlockingEnabled()) {
			if (source == MPI_PROC_NULL || dest == MPI_PROC_NULL || count == 0) {
				err = MPI_Sendrecv(
						buf, count, datatype, dest, sendtag,
						buf, count, datatype, source, recvtag,
						comm, status);
			} else {
				int packedSize;
				void *helperbuf = nullptr;

				// Setup a buffer for receiving
				err = MPI_Pack_size(count, datatype, comm, &packedSize);
				if (err != MPI_SUCCESS) return err;

				err = MPI_Alloc_mem(packedSize, MPI_INFO_NULL, &helperbuf);
				if (err != MPI_SUCCESS) return err;

				// Receive into temporary buffer
				err = MPI_Sendrecv(buf, count, datatype, dest, sendtag,
						helperbuf, packedSize, MPI_BYTE, source, recvtag,
						comm, status);

				// Copy into user's buffer
				if (err == MPI_SUCCESS) {
					int position = 0;
					err = MPI_Unpack(helperbuf, packedSize, &position,
							buf, count, datatype, comm);
				}

				// Release resources
				if (err == MPI_SUCCESS) {
					err = MPI_Free_mem(helperbuf);
				} else {
					MPI_Free_mem(helperbuf);
				}
			}
		} else {
			static MPI_Sendrecv_replace_t *symbol = (MPI_Sendrecv_replace_t *) Symbol::load(__func__);
			err = (*symbol)(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);
		}
		return err;
	}
} // extern C


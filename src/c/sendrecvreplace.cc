/*
 * Copyright (c) 2017 Barcelona Supercomputing Center. All rights
 *                    reserved.
 *
 * This file is part of OmpSs-MPI interoperability library.
 *
 * OmpSs-MPI interop. library is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
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


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
	int MPI_Sendrecv(
			MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag,
			void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag,
			MPI_Comm comm, MPI_Status *status)
	{
		int err = MPI_SUCCESS;
		if (C::Environment::isEnabled()) {
			MPI_Request requests[2];
			err = MPI_Irecv(recvbuf, recvcount, recvtype, source, recvtag, comm, &requests[0]);
			err = MPI_Isend(sendbuf, sendcount, sendtype, dest, sendtag, comm, &requests[1]);
			
			if (status != MPI_STATUS_IGNORE) {
				MPI_Status statuses[2];
				C::processRequests({requests, 2}, statuses);
				*status = statuses[0];
			} else {
				C::processRequests({requests, 2});
			}
		} else {
			static C::MPI_Sendrecv_t *symbol = (C::MPI_Sendrecv_t *) Symbol::loadNextSymbol(__func__);
			err = (*symbol)(sendbuf, sendcount, sendtype, dest, sendtag,
					recvbuf, recvcount, recvtype, source, recvtag,
					comm, status);
		}
		return err;
	}
} // extern C


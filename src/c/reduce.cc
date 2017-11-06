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

#if MPI_VERSION >=3

#include "definitions.h"
#include "environment.h"
#include "process_request.h"
#include "symbols.h"

extern "C" {
	int MPI_Reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
	{
		int err = MPI_SUCCESS;
		
		if (C::Environment::isEnabled()) {
			MPI_Request request;
			err = MPI_Ireduce(sendbuf, recvbuf, count, datatype, op, root, comm, &request);
			
			C::processRequest(request);
		} else {
			static C::MPI_Reduce_t *symbol = (C::MPI_Reduce_t *) Symbol::loadNextSymbol(__func__);
			err = (*symbol)(sendbuf, recvbuf, count, datatype, op, root, comm);
		}
		return err;
	}
} // extern C

#endif // MPI_VERSION


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
#include <mpi.h> // defines MPI_VERSION

#if MPI_VERSION >=3

#include "definitions.h"
#include "environment.h"
#include "process_request.h"
#include "symbols.h"

extern "C" {
	void mpi_igatherv_(const void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype,
			void *recvbuf, const MPI_Fint recvcounts[], const MPI_Fint displs[], MPI_Fint *recvtype,
			MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err);
	
	void mpi_gatherv_(const void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype,
			void *recvbuf, const MPI_Fint recvcounts[], const MPI_Fint displs[], MPI_Fint *recvtype,
			MPI_Fint *root, MPI_Fint *comm, MPI_Fint *err)
	{
		if (Fortran::Environment::isEnabled()) {
			MPI_Fint request;
			mpi_igatherv_(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, &request, err);
			Fortran::processRequest(request);
		} else {
			static Fortran::mpi_gatherv_t *symbol = (Fortran::mpi_gatherv_t *) Symbol::loadNextSymbol(__func__);
			(*symbol)(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, comm, err);
		}
	}
} // extern C

#endif // MPI_VERSION


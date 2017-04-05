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

#include "mpi/request.h"
#include "mpi/status.h"
#include "print.h"
#include "ticket.h"

using namespace nanos::mpi;

extern "C" {
    void mpi_igatherv_( const void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype,
        void *recvbuf, const MPI_Fint recvcounts[], const MPI_Fint displs[], MPI_Fint *recvtype,
        MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err );

    void mpi_gatherv_( const void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype,
                       void *recvbuf, const MPI_Fint recvcounts[],
                       const MPI_Fint displs[], MPI_Fint *recvtype,
                       MPI_Fint *root, MPI_Fint *comm, MPI_Fint *err )
    {
        nanos::log::intercepted_call( __func__ );

        MPI_Fint req;
        mpi_igatherv_( sendbuf, sendcount, sendtype,
                recvbuf, recvcounts, displs, recvtype,
                root, comm, &req, err );

        Fortran::Ticket ticket( req );
        ticket.wait();
    }

} // extern C

#endif // MPI_VERSION


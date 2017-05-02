/*
 * Copyright (c) 2016 Barcelona Supercomputing Center. All rights
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

#include "ticket.h"
#include "print.h"

using namespace nanos::mpi;

extern "C" {
    void mpi_ialltoall_( const void *sendbuf, MPI_Fint *sendcount,
                        MPI_Fint *sendtype, void *recvbuf,
                        MPI_Fint *recvcount, MPI_Fint *recvtype,
                        MPI_Fint *comm, MPI_Fint *req, MPI_Fint *ierror );

    void mpi_alltoall_( const void *sendbuf, MPI_Fint *sendcount,
                        MPI_Fint *sendtype, void *recvbuf,
                        MPI_Fint *recvcount, MPI_Fint *recvtype,
                        MPI_Fint *comm, MPI_Fint *ierror )
    {
        nanos::log::intercepted_call( __func__ );

        MPI_Fint req;
        mpi_ialltoall_( sendbuf, sendcount, sendtype,
                        recvbuf, recvcount, recvtype,
                        comm, &req, ierror );

        Fortran::Ticket ticket( req );
        ticket.wait();
    }
}

#endif // MPI_VERSION

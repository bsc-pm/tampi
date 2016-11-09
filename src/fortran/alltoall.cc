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

#include "mpi/request.h"
#include "mpi/status.h"
#include "ticket.h"
#include "print.h"

using namespace nanos::mpi;
using ticket_t = Ticket<Fortran::request,Fortran::ignored_status,1>;

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
        print::intercepted_call( __func__ );

        Fortran::request req;
        mpi_ialltoall_( sendbuf, sendcount, sendtype,
                        recvbuf, recvcount, recvtype,
                        comm, &static_cast<MPI_Fint&>(req), ierror );

        ticket_t ticket( {req}, *ierror );
        ticket.wait();
        *ierror = ticket.getReturnError();
    }
}

#endif // MPI_VERSION

/*
 * Copyright (c) 2015 Barcelona Supercomputing Center. All rights
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
#include "smartpointer.h"
#include "ticket.h"
#include "print.h"

using namespace nanos::mpi;
using ticket = Ticket<Fortran::request,Fortran::ignored_status,1>;

extern "C" {
    void mpi_iallreduce_( const void *sendbuf, void *recvbuf, MPI_Fint *count,
                       MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm,
                       MPI_Fint *request, MPI_Fint *err );

    void mpi_allreduce_( const void *sendbuf, void *recvbuf, MPI_Fint *count,
                      MPI_Fint *datatype, MPI_Fint *op, MPI_Fint *comm, MPI_Fint *err )
    {
        print::intercepted_call( __func__ );

        Fortran::request req;
        mpi_iallreduce_( sendbuf, recvbuf, count, datatype,
                         op, comm, &static_cast<MPI_Fint&>(req), err );

        nanos::shared_pointer<ticket> waitCond( new ticket( {req}, *err ) );
        waitCond->wait();
        *err = waitCond->getReturnError();
    }
}

#endif // MPI_VERSION
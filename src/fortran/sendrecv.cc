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
#include <mpi.h>

#include "api_def.h"
#include "print.h"
#include "ticket.h"

using namespace nanos::mpi;

extern "C" {
    void mpi_irecv_( void *recvbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *src,
                MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err );

    void mpi_isend_( MPI3CONST void *sendbuf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *dest,
                MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err );

    void mpi_sendrecv_( MPI3CONST void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype,
                           MPI_Fint *dest, MPI_Fint *sendtag,
                           void *recvbuf, MPI_Fint *recvcount, MPI_Fint *recvtype,
                           MPI_Fint *source, MPI_Fint *recvtag,
                           MPI_Fint *comm, MPI_Fint *status, MPI_Fint *err )
    {
        std::array<MPI_Fint, 2> reqs;
        mpi_irecv_( recvbuf, recvcount, recvtype, source, recvtag, comm,
                    &reqs[0], err );

        mpi_isend_( sendbuf, sendcount, sendtype, dest, sendtag, comm,
                    &reqs[1], err );

	// TODO: copy back status information when not ignored
        //if( status == MPI_F_STATUS_IGNORE ) {
           Fortran::Ticket ticket( {reqs.begin(), reqs.end()} );
           ticket.wait();
        //} else {
        //}
    }
} // extern C


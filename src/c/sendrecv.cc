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
#include <mpi.h>

#include "mpicommon.h"
#include "ticket.h"
#include "sendrecv.h"
#include <nanox-dev/smartpointer.hpp>

extern "C" {
    int MPI_Sendrecv( MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype,
        int dest, int sendtag,
        void *recvbuf, int recvcount, MPI_Datatype recvtype,
        int source, int recvtag,
        MPI_Comm comm, MPI_Status *status)
    {
        int err;
        nanos::mpi::sendrecv( sendbuf, sendcount, sendtype, dest, sendtag,
                        recvbuf, recvcount, recvtype, source, recvtag,
                        comm, status, &err );
        return err;
    }
} // extern C

namespace nanos {
namespace mpi {
    using ticket = TicketTraits<MPI_Comm,1>::ticket_type;

    shared_pointer<ticket> isendrecv( MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype,
                    int dest, int sendtag,
                    void *recvbuf, int recvcount, MPI_Datatype recvtype,
                    int source, int recvtag,
                    MPI_Comm comm )
    {
        // TODO do not forget to assign MPI function return value to ticket error
        ticket *result = new ticket();
        int err = MPI_Isend( sendbuf, sendcount, sendtype, dest, sendtag,
                    comm, &result->getData().getRequest<0>() );
        // Currently the following line  would be redundant as 
        // the second communication is always issued:
        // result->getData().setError( err );
        
        err = MPI_Irecv( recvbuf, recvcount, recvtype, source, recvtag,
                    comm, &result->getData().getRequest<1>() );
        result->getData().setError( err );

        return shared_pointer<ticket>(result);
    }

} // namespace mpi
} // namespace nanos


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

#include "mpi/common.h"
#include "mpi/error.h"
#include "mpi/status.h"
#include "smartpointer.h"
#include "ticket.h"

template < nanos::mpi::StatusKind kind >
using ticket = nanos::mpi::Ticket<MPI_Request,MPI_Status,kind,int,2>;

template < typename TicketType >
shared_pointer<TicketType> isendrecv( MPI3CONST void *sendbuf, int sendcount, 
                                      MPI_Datatype sendtype, int dest, int sendtag,
                                      void *recvbuf, int recvcount, 
                                      MPI_Datatype recvtype, int source, int recvtag,
                                      MPI_Comm comm );

#include "sendrecv.h"

extern "C" {
    int MPI_Sendrecv( MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype,
                      int dest, int sendtag, 
                      void *recvbuf, int recvcount, MPI_Datatype recvtype,
                      int source, int recvtag, 
                      MPI_Comm comm, MPI_Status *status)
    {
        int err;
        if( status == MPI_STATUS_IGNORE ) {
            using ticket = ticket<nanos::mpi::StatusKind::ignore>;
            nanos::mpi::sendrecv<ticket>( sendbuf, sendcount, sendtype,
                                          dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag,
                                          comm, status, &err );
        } else {
            using ticket = ticket<nanos::mpi::StatusKind::attend>;
            nanos::mpi::sendrecv<ticket>( sendbuf, sendcount, sendtype,
                                          dest, sendtag, recvbuf, recvcount,
                                          recvtype, source, recvtag,
                                          comm, status, &err );
        }
        return err;
    }
} // extern C

namespace nanos {
namespace mpi {
    
    template < typename TicketType >
    shared_pointer<TicketType> isendrecv( MPI3CONST void *sendbuf, int sendcount,
                                          MPI_Datatype sendtype,
                                          int dest, int sendtag,
                                          void *recvbuf, int recvcount, 
                                          MPI_Datatype recvtype,
                                          int source, int recvtag,
                                          MPI_Comm comm )
    {
        using ticket = TicketType;
        
        shared_pointer<ticket> result( new ticket() );
        int err = MPI_Isend( sendbuf, sendcount, sendtype, dest, sendtag,
                             comm, result->getRequestSet().at(0) );
        
        err = MPI_Irecv( recvbuf, recvcount, recvtype, source, recvtag,
                    comm, result->getRequestSet().at(1) );
        result->setError( err );

        return result;
    }

    template
    shared_pointer<ticket<nanos::mpi::StatusKind::ignore> > isendrecv( MPI3CONST void *sendbuf, int sendcount,
                                          MPI_Datatype sendtype,
                                          int dest, int sendtag,
                                          void *recvbuf, int recvcount,
                                          MPI_Datatype recvtype,
                                          int source, int recvtag,
                                          MPI_Comm comm );

    template
    shared_pointer<ticket<nanos::mpi::StatusKind::attend> > isendrecv( MPI3CONST void *sendbuf, int sendcount,
                                          MPI_Datatype sendtype,
                                          int dest, int sendtag,
                                          void *recvbuf, int recvcount,
                                          MPI_Datatype recvtype,
                                          int source, int recvtag,
                                          MPI_Comm comm );

} // namespace mpi
} // namespace nanos


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
#ifndef SENDRECV_H
#define SENDRECV_H

#include "print.h"
#include "mpicommon.h"
#include "ticket.h"
#include <nanox-dev/smartpointer.hpp>

namespace nanos {
namespace mpi {

template< typename IntType, typename DataType, typename CommType >
shared_pointer< typename TicketTraits<CommType,1>::ticket_type >
isendrecv( MPI3CONST void *sendbuf, IntType sendcount, DataType sendtype,
            IntType dest, IntType sendtag,
            void *recvbuf, IntType recvcount, DataType recvtype,
            IntType source, IntType recvtag, CommType comm );

template< typename IntType, typename DataType, typename CommType, 
        typename StatusType, typename ErrType>
void sendrecv( MPI3CONST void *sendbuf, IntType sendcount, DataType sendtype,
            IntType dest, IntType sendtag,
            void *recvbuf, IntType recvcount, DataType recvtype,
            IntType source, IntType recvtag,
            CommType comm, StatusType *status, ErrType *ierror )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Sendrecv" );

    auto waitCond =
        isendrecv( sendbuf, sendcount, sendtype, dest, sendtag, 
                    recvbuf, recvcount, recvtype, source, recvtag, comm );
    waitCond->wait( status, ierror );
}

} // namespace mpi
} // namespace nanos

#endif // SENDRECV_H

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
#ifndef RECV_H
#define RECV_H

#include "print.h"
#include "ticket.h"
#include "smartpointer.h"

namespace nanos {
namespace mpi {

template< typename TicketType, typename IntType, typename DataType, 
        typename CommType, typename StatusType, typename ErrType >
void recv( void *buf, IntType count, DataType datatype, 
        IntType source, IntType tag, CommType comm, StatusType *status,
        ErrType *ierror )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Recv" );

    shared_pointer<TicketType> waitCond = irecv<TicketType>( buf, count, datatype, source, tag, comm );
    waitCond->wait( status, ierror );
}

} // namespace mpi
} // namespace nanos

#endif // RECV_H

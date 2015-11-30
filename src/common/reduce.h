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
#ifndef REDUCE_H
#define REDUCE_H

#include "print.h"
#include "ticket.h"
#include "smartpointer.h"

namespace nanos {
namespace mpi {

template< typename TicketType, typename IntType, typename DataType, typename OpType, 
        typename CommType, typename ErrType >
void reduce( const void *sendbuf, void *recvbuf, IntType count,
            DataType datatype, OpType op, IntType root, 
            CommType comm, ErrType *ierror )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Reduce" );

    shared_pointer<TicketType> waitCond = 
            ireduce( sendbuf, recvbuf, count, datatype, op, root, comm );
    waitCond->wait( ierror );
}

} // namespace mpi
} // namespace nanos

#endif // REDUCE_H

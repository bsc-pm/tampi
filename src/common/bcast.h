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
#ifndef BCAST_H
#define BCAST_H

// Note: no need to use an ifdef guard if MPI_VERSION < 3.
// This file should not be included in that case from the .cc file.

#include "print.h"
#include "mpicommon.h"
#include "ticket.h"
#include <nanox-dev/smartpointer.hpp>

namespace nanos {
namespace mpi {

template < typename IntType, typename DataType, typename CommType >
shared_pointer< typename TicketTraits<CommType,1>::ticket_type >
ibcast( void *buf, IntType count, DataType datatype,
            IntType root, CommType comm );

template< typename IntType, typename DataType, typename CommType, typename ErrType>
void bcast( void *buf, IntType count, DataType datatype, IntType root, 
            CommType comm, ErrType *ierror )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Bcast" );

    auto waitCond = ibcast( buf, count, datatype, root, comm );
    waitCond->wait( ierror );
}

} // namespace mpi
} // namespace nanos

#endif // BCAST_H

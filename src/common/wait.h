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
#ifndef WAIT_H
#define WAIT_H

#include "print.h"
#include "ticket.h"
#include "ticketchecker.h"
#include "smartpointer.h"

namespace nanos {
namespace mpi {

template< typename TicketType, typename ReqType, typename StatusType,
        typename ErrType >
void wait( ReqType *request, StatusType *status, ErrType *ierror )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Wait" );

    shared_pointer<TicketType> waitCond = 
                            new TicketType( TicketType::checker( request ) );
    waitCond->wait( status, ierror );
}

} // namespace mpi
} // namespace nanos

#endif // WAIT_H

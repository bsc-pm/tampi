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

#include "mpi/status.h"
#include "smartpointer.h"
#include "ticket.h"
#include "print.h"


namespace nanos {
namespace mpi {

template < StatusKind kind >
using ticket =  Ticket<C::request,C::status<kind>,1>;

template < StatusKind kind >
using checker = TicketChecker<C::request,C::status<kind>,1>;

} // namespace mpi
} // namespace nanos


extern "C" {

int MPI_Wait( MPI_Request *request, MPI_Status *status )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Wait" );

    using namespace nanos::mpi;
    
    int err;
    if( status == MPI_STATUS_IGNORE ) {
        using ticket = ticket<StatusKind::ignore>;
        using checker = checker<StatusKind::ignore>;

        auto waitCond = nanos::make_shared(
                new ticket( checker( request )) );
        waitCond->wait( &err );
    } else {
        using ticket = ticket<StatusKind::attend>;
        using checker = checker<StatusKind::attend>;

        auto waitCond = nanos::make_shared(
                new ticket( checker( request )) );
        waitCond->wait( status, &err );
    }
    return err;
}

} // extern C


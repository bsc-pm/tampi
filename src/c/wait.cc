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

template < nanos::mpi::StatusKind kind >
using ticket = nanos::mpi::Ticket<MPI_Request,MPI_Status,kind,int,1>;

template < nanos::mpi::StatusKind kind >
using checker = nanos::mpi::TicketChecker<MPI_Request,MPI_Status,kind,int,1>;

extern "C" {

int MPI_Wait( MPI_Request *request, MPI_Status *status )
{
    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Wait" );

    int err;
    if( status == MPI_STATUS_IGNORE ) {
        using ticket = ticket<nanos::mpi::StatusKind::ignore>;
        using checker = checker<nanos::mpi::StatusKind::ignore>;

        auto waitCond = shared_pointer<ticket>(
                new ticket( checker( 1, request, status )) );
        waitCond->wait( &err );
    } else {
        using ticket = ticket<nanos::mpi::StatusKind::attend>;
        using checker = checker<nanos::mpi::StatusKind::attend>;

        auto waitCond = shared_pointer<ticket>(
                new ticket( checker( 1, request, status )) );
        waitCond->wait( status, &err );
    }
    return err;
}

} // extern C


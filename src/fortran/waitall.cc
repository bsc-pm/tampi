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

#include "print.h"
#include "ticket.h"
#include "mpicommon.h"
#include <nanox-dev/smartpointer.hpp>

extern "C" {

void mpi_waitall_( MPI_Fint *count, MPI_Fint array_of_requests[],
                      MPI_Fint array_of_statuses[], MPI_Fint *err )
{
    using namespace nanos::mpi;
    typedef Fortran::TicketTraits<0>::ticket_type ticket;
    typedef ticket::checker_type ticket_checker;

    print::dbg( "[MPI Async. Overload Library] Intercepted MPI_Waitall" );

    auto waitCond = shared_pointer<ticket>( 
                           new ticket( ticket_checker( *count, array_of_requests ) )
                        );
    waitCond->wait( *count, array_of_statuses, err );
}

} // extern C

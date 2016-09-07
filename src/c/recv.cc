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

#include "mpi/request.h"
#include "mpi/status.h"
#include "ticketqueue.h"
#include "print.h"
#include "smartpointer.h"
#include "ticket.h"

using namespace nanos::mpi;


extern "C" {
    int MPI_Recv( void *buf, int count, MPI_Datatype datatype,
        int source, int tag, MPI_Comm comm, MPI_Status *status )
    {
        print::intercepted_call( __func__ );

	C::request req;
        int err = MPI_Irecv( buf, count, datatype, source, tag, comm,
                             &static_cast<MPI_Request&>(req) );

        if( status == MPI_STATUS_IGNORE ) {
            using ticket_t = Ticket<C::request,C::ignored_status,1>;
            ticket_t ticket( {req}, err );
			TicketQueue::wait( ticket );
            err = ticket.getReturnError();
        } else {
            using ticket_t = Ticket<C::request,C::status,1>;
            ticket_t ticket( {req}, err );
			TicketQueue::wait( ticket );
            err = ticket.getReturnError();
			*status = ticket.getStatuses()[0];
        }
        return err;
    }
} // extern C


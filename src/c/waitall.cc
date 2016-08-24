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

using namespace nanos::mpi;

template < StatusKind kind >
using ticket =  Ticket<C::request,C::status<kind>,0>;

extern "C" {
    int MPI_Waitall(int count, MPI_Request array_of_requests[],
                          MPI_Status array_of_statuses[])
    {
        print::intercepted_call( __func__ );

	using requests_array = std::vector<C::request>;
	using statuses_array = std::vector<C::status<StatusKind::attend> >;
    
        int err = MPI_SUCCESS;
        if( status == MPI_STATUS_IGNORE ) {
            using ticket = ticket<StatusKind::ignore>;
            shared_pointer<ticket> waitCond( new ticket( 
                       std::move( utils::transform_to<requests_array>(*count, array_of_requests) ));
            err = waitCond->wait();
        } else {
            using ticket = ticket<StatusKind::attend>;
            shared_pointer<ticket> waitCond( new ticket(
                       std::move( utils::transform_to<requests_array>(*count, array_of_requests) ),
                       std::move( utils::transform_to<statuses_array>(*count, array_of_statuses) )
                       );
            *err = waitCond->wait();
        }
        return err;
    }
} // extern C


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

#include "array_utils.h"
#include "mpi/request.h"
#include "mpi/status.h"
#include "ticket.h"
#include "smartpointer.h"
#include "print.h"

using namespace nanos::mpi;
using namespace nanos::utils;


extern "C" {
    int MPI_Waitall(int count, MPI_Request array_of_requests[],
                          MPI_Status array_of_statuses[])
    {
        print::intercepted_call( __func__ );

        using requests_array = std::vector<C::request>;
        using statuses_array = std::vector<C::status >;
    
        int err = MPI_SUCCESS;
        if( array_of_statuses == MPI_STATUSES_IGNORE ) {
            using ticket_t = Ticket<C::request,C::ignored_status,0>;
            ticket_t ticket(
                       std::move( transform_to<requests_array>()(count, reinterpret_cast<C::request*>(array_of_requests)) ),
                       err );
            ticket.wait();
            err = ticket.getReturnError();
        } else {
            using ticket_t = Ticket<C::request,C::status,0>;
            ticket_t ticket(
                       std::move( transform_to<requests_array>()(count, reinterpret_cast<C::request*>(array_of_requests)) ),
                       err );
            ticket.wait();
            err = ticket.getReturnError();
            std::copy( ticket.getStatuses().begin(), ticket.getStatuses().end(), array_of_statuses );
        }
        return err;
    }
} // extern C


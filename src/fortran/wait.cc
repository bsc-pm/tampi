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

template< StatusKind kind >
using ticket = Ticket<Fortran::request,Fortran::status<kind>,1>;

extern "C" {
    void mpi_wait_( MPI_Fint *request, MPI_Fint *status, MPI_Fint *err )
    {
        print::intercepted_call( __func__ );
    
        if( status == MPI_F_STATUS_IGNORE ) {
            using ticket = ticket<StatusKind::ignore>;
            shared_pointer<ticket> waitCond( new ticket(*request, *err) );
            *err = waitCond->wait();
        } else {
            using ticket = ticket<StatusKind::attend>;
            shared_pointer<ticket> waitCond( new ticket(*request, *err) );
            *err = waitCond->wait();
        }
    }
} // extern C

} // namespace mpi
} // namespace nanos


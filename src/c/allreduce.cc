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
#include <mpi.h> // defines MPI_VERSION

#if MPI_VERSION >=3

#include "mpi/request.h"
#include "mpi/status.h"
#include "smartpointer.h"
#include "ticket.h"
#include "print.h"

using namespace nanos::mpi;
using ticket = Ticket<C::request,C::ignored_status,1>;

extern "C" {
    int MPI_Allreduce( const void *sendbuf, void *recvbuf, int count,
                   MPI_Datatype datatype, MPI_Op op, MPI_Comm comm )
    {
        print::intercepted_call( __func__ );

	C::request req;
        int err = MPI_Iallreduce( sendbuf, recvbuf, count, datatype,
                                  op, comm, &static_cast<MPI_Request&>(req) );
        nanos::shared_pointer<ticket> waitCond( new ticket( {req}, err ) );
        waitCond->wait();
        return err;
    }
} // extern C

#endif // MPI_VERSION

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

#include "mpi/error.h"
#include "mpi/status.h"
#include "print.h"
#include "smartpointer.h"
#include "ticket.h"

using namespace nanos::mpi;
using ticket = Ticket<C::request,C::status<StatusKind::ignore>,1>;

extern "C" {
    int MPI_Gatherv( const void *sendbuf, int sendcount, MPI_Datatype sendtype,
        void *recvbuf, const int recvcounts[], const int displs[], MPI_Datatype
    recvtype,
        int root, MPI_Comm comm )
    {
        print::intercepted_call( __func__ );

	C::request req;
        int err = MPI_Igatherv( sendbuf, sendcount, sendtype,
                                recvbuf, recvcounts, displs, recvtype,
                                root, comm, &static_cast<MPI_Request&>(req) );
        shared_pointer<ticket> waitCond( new ticket( req, err ) );
        err = waitCond->wait();
        return err;
    }
} // extern C

#endif // MPI_VERSION


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

#include "mpi/common.h"
#include "mpi/error.h"
#include "mpi/status.h"
#include "mpi/request.h"
#inculde "print.h"
#include "smartpointer.h"
#include "ticket.h"

namespace nanos {
namespace mpi {

using ticket = Ticket<C::request,C::status<StatusKind::ignore>,1>;

extern "C" {
    int MPI_Ssend( MPI3CONST void *buf, int count, MPI_Datatype datatype,
        int dest, int tag, MPI_Comm comm )
    {
        print::intercepted_call( __func__ );

        C::Request req;
        int err = MPI_Issend( buf, count, datatype, dest, tag, comm, 
                              &static_cast<MPI_Request&>(req) );

        shared_pointer<ticket> waitCond( new ticket( req, err ) );
        err = waitCond->wait();
        return err;
    }
} // extern C


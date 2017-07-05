/*
 * Copyright (c) 2017 Barcelona Supercomputing Center. All rights
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

#include "ticket.h"
#include "print.h"
#include "api_def.h"

using namespace nanos::mpi;


extern "C" {
   API_DEF( int, MPI_Recv,
              ( void *buf, int count, MPI_Datatype datatype,
                int source, int tag, MPI_Comm comm, MPI_Status *status )
            )
   {
      nanos::log::intercepted_call( __func__ );

      MPI_Request req;
      int err = MPI_Irecv( buf, count, datatype, source, tag, comm,
            &req );

      // TODO: copy back status information
      //if( status == MPI_STATUS_IGNORE ) {
      C::Ticket ticket( req );
nanos::log::verbose_output("before wait");
      ticket.wait();
nanos::log::verbose_output("after wait");
      //} else {
      //}
      return err;
   }
} // extern C


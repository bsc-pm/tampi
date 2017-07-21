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
   API_DEF( int, MPI_Sendrecv,
              ( MPI3CONST void *sendbuf, int sendcount, MPI_Datatype sendtype,
                int dest, int sendtag,
                void *recvbuf, int recvcount, MPI_Datatype recvtype,
                int source, int recvtag,
                MPI_Comm comm, MPI_Status *status )
            )
   {
      nanos::log::intercepted_call( __func__ );

      std::array<MPI_Request,2> reqs;
      int err = MPI_Irecv( recvbuf, recvcount, recvtype, source, recvtag, comm,
            &reqs[0] );

      err = MPI_Isend( sendbuf, sendcount, sendtype, dest, sendtag, comm,
            &reqs[1] );

      if( status != MPI_STATUS_IGNORE ) {
         MPI_Status statuses[2];
         C::Ticket ticket( {std::begin(reqs), std::end(reqs)}, statuses );
         ticket.wait();
	 *status = statuses[0];
      } else {
         C::Ticket ticket( {std::begin(reqs), std::end(reqs)} );
         ticket.wait();
      }
      return err;
   }
} // extern C


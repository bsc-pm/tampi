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

#include "mpi/error.h"
#include "mpi/status.h"
#include "smartpointer.h"
#include "ticket.h"

template < nanos::mpi::StatusKind kind >
using ticket = nanos::mpi::Ticket<MPI_Request,MPI_Status,kind,int,1>;

template < typename TicketType >
shared_pointer<TicketType> irecv( void *buf, int count, MPI_Datatype datatype, 
                                  int source, int tag, MPI_Comm comm );

#include "recv.h"

extern "C" {
    int MPI_Recv( void *buf, int count, MPI_Datatype datatype,
        int source, int tag, MPI_Comm comm, MPI_Status *status )
    {
        using ticket_ignore_status = ticket<nanos::mpi::StatusKind::ignore>;
        using ticket_attend_status = ticket<nanos::mpi::StatusKind::attend>;

        int err;
        if( status == MPI_STATUS_IGNORE ) {
	        nanos::mpi::recv<ticket_ignore_status>( buf, count, datatype, source, tag, comm, status, &err );
        } else {
	        nanos::mpi::recv<ticket_attend_status>( buf, count, datatype, source, tag, comm, status, &err );
        }
        return err;
    }
} // extern C

namespace nanos {
namespace mpi {

    template < typename TicketType >
    shared_pointer<TicketType> irecv( void *buf, int count, MPI_Datatype datatype, int source, int tag,
            MPI_Comm comm )
    {
        using ticket = TicketType;
        
        shared_pointer<ticket> result( new ticket() );
        int err = MPI_Irecv( buf, count, datatype, source, tag, comm, result->getRequestSet().at(0) );
        result->setError( err );

        return result;
    }
    
} // namespace mpi
} // namespace nanos


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
using ticket = nanos::mpi::Ticket<MPI_Fint,MPI_Fint,kind,MPI_Fint,1>;

template < typename TicketType >
shared_pointer<TicketType> irecv( void *buf, MPI_Fint *count, MPI_Fint *datatype, 
                                  MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm );

#include "recv.h"

extern "C" {
    void mpi_recv_( void *buf, MPI_Fint *count, MPI_Fint *datatype,
        MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *status, MPI_Fint *err )
    {

        if( status == MPI_F_STATUS_IGNORE ) {
           using ticket = ticket<nanos::mpi::StatusKind::ignore>;
	        nanos::mpi::recv<ticket>( buf, count, datatype, source, tag, comm, status, err );
        } else {
           using ticket = ticket<nanos::mpi::StatusKind::attend>;
	        nanos::mpi::recv<ticket>( buf, count, datatype, source, tag, comm, status, err );
        }
    }

    void mpi_irecv_( void *buf, MPI_Fint *count, MPI_Fint *datatype,
        MPI_Fint *source, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err );
}

namespace nanos {
namespace mpi {

    template< typename TicketType >
    shared_pointer<TicketType> irecv( void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *source, MPI_Fint *tag,
            MPI_Fint *comm )
    {
        using ticket = TicketType;
        
        shared_pointer<ticket> result( new ticket() );
        mpi_irecv_( buf, count, datatype, source, tag, comm,
            result->getRequestSet().at(0),
            &result->getError().value() );
        return shared_pointer<ticket>(result);
    }
    
} // namespace mpi
} // namespace nanos


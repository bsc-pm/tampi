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
#include "smartpointer.h"
#include "ticket.h"

namespace nanos {
namespace mpi {

using ticket = nanos::mpi::Ticket<Fortran::request,Fortran::status<StatusKind::ignore>,1>;

shared_pointer<ticket> isend( MPI3CONST void *buf, MPI_Fint *count, 
                              MPI_Fint *datatype, MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm );

} // namespace mpi
} // namespace nanos

#include "send.h"

namespace nanos {
namespace mpi {

extern "C" {
    void mpi_send_( MPI3CONST void *buf, MPI_Fint *count, MPI_Fint *datatype,
                    MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, MPI_Fint *err )
    {
        nanos::mpi::send<ticket>( buf, count, datatype, dest, tag, comm, err );
    }

    void mpi_isend_( MPI3CONST void *buf, MPI_Fint *count, MPI_Fint *datatype,
                     MPI_Fint *dest, MPI_Fint *tag, MPI_Fint *comm, 
                     MPI_Fint *request, MPI_Fint *err );
}
    
    shared_pointer< ticket > isend( MPI3CONST void *buf, MPI_Fint *count,
                                    MPI_Fint *datatype, MPI_Fint *dest, 
                                    MPI_Fint *tag, MPI_Fint *comm )
    {
        shared_pointer<ticket> result( new ticket() );
        mpi_isend_( buf, count, datatype, dest, tag, comm, 
            result->getChecker().getRequest(),
            result->getChecker().getError() );
        return result;
    }

} // namespace mpi
} // namespace nanos

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
#include "smartpointer.h"
#include "ticket.h"

using ticket = nanos::mpi::Ticket<MPI_Fint,MPI_Fint,nanos::mpi::StatusKind::ignore,MPI_Fint,1>;

shared_pointer<ticket> ibcast( void *buf, MPI_Fint *count, MPI_Fint *datatype,
                               MPI_Fint *root, MPI_Fint *comm );

#include "bcast.h"

extern "C" {

void mpi_bcast_(void *buffer, MPI_Fint *count, MPI_Fint *datatype,
    MPI_Fint *root, MPI_Fint *comm, MPI_Fint *err )
{
    nanos::mpi::bcast<ticket>( buffer, count, datatype, root, comm, err );
}

void mpi_ibcast_(void *buffer, MPI_Fint *count, MPI_Fint *datatype,
    MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err );

} // extern C

namespace nanos {
namespace mpi {

    shared_pointer<ticket> ibcast( void *buf, MPI_Fint *count, MPI_Fint *datatype,
                                     MPI_Fint *root, MPI_Fint *comm )
    {
        shared_pointer<ticket> result( new ticket() );
        mpi_ibcast_( buf, count, datatype, root, comm,
            result->getRequestSet().at(0),
            &result->getError().value() );
        return result;
    }

} // namespace mpi
} // namespace nanos

#endif // MPI_VERSION



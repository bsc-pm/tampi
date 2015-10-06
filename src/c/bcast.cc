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

#include "mpicommon.h"
#include "ticket.h"
#include "bcast.h"
#include <nanox-dev/smartpointer.hpp>

extern "C" {
    int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype,
        int root, MPI_Comm comm)
    {
        int err;
        nanos::mpi::bcast( buffer, count, datatype, root, comm, &err );
        return err;
    }
} // extern C

namespace nanos {
namespace mpi {
    typedef typename TicketTraits<MPI_Comm,1>::ticket_type ticket;

    template<>
    shared_pointer< ticket > ibcast( void *buf, int count, MPI_Datatype datatype,
            int root, MPI_Comm comm )
    {
        // TODO do not forget to assign MPI function return value to ticket error
        ticket *result = new ticket();
        int err = MPI_Ibcast( buf, count, datatype, root, comm, &result->getData().getRequest<0>() );
        result->getData().setError( err );

        return shared_pointer<ticket>(result);
    }

} // namespace mpi
} // namespace nanos

#endif // MPI_VERSION


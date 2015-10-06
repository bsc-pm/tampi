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

#if MPI_VERSION >=3

#include "mpicommon.h"
#include "ticket.h"
#include "gatherv.h"
#include <nanox-dev/smartpointer.hpp>

extern "C" {
    void mpi_gatherv_( const void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype,
        void *recvbuf, const MPI_Fint recvcounts[], const MPI_Fint displs[], MPI_Fint *recvtype,
        MPI_Fint *root, MPI_Fint *comm, MPI_Fint *err )
    {
        nanos::mpi::gatherv( sendbuf, sendcount, sendtype,
                        recvbuf, recvcounts, displs, recvtype,
                        root, comm, err );
    }

    void mpi_igatherv_( const void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype,
        void *recvbuf, const MPI_Fint recvcounts[], const MPI_Fint displs[], MPI_Fint *recvtype,
        MPI_Fint *root, MPI_Fint *comm, MPI_Fint *request, MPI_Fint *err );
} // extern C

namespace nanos {
namespace mpi {
    typedef typename TicketTraits<MPI_Fint*,1>::ticket_type ticket;

    template<>
    shared_pointer< ticket> igatherv( const void *sendbuf, MPI_Fint *sendcount, MPI_Fint *sendtype,
        void *recvbuf, const MPI_Fint recvcounts[], const MPI_Fint displs[], MPI_Fint *recvtype,
        MPI_Fint *root, MPI_Fint *comm )
    {
        // TODO do not forget to assign MPI function return value to ticket error
        ticket *result = new ticket();
        mpi_igatherv_( sendbuf, sendcount, sendtype, 
                recvbuf, recvcounts, displs, recvtype, 
                root, comm, 
                &result->getData().getRequest<0>(), // Store output request into ticket
                &result->getData().getError() );    // Store output error   into ticket
        return shared_pointer<ticket>(result);
    }

} // namespace mpi
} // namespace nanos
#endif // MPI_VERSION


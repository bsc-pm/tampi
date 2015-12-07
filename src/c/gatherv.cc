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

namespace nanos {
namespace mpi {

using ticket = Ticket<C::request,C::status<StatusKind::ignore>,1>;

shared_pointer<ticket> igatherv( const void *sendbuf, int sendcount,
                                 MPI_Datatype sendtype,
                                 void* recvbuf, const int recvcounts[], 
                                 const int displs[], MPI_Datatype recvtype, 
                                 int root, MPI_Comm comm );

} // namespace mpi
} // namespace nanos

#include "gatherv.h"

extern "C" {
    int MPI_Gatherv( const void *sendbuf, int sendcount, MPI_Datatype sendtype,
        void *recvbuf, const int recvcounts[], const int displs[], MPI_Datatype
    recvtype,
        int root, MPI_Comm comm )
    {
        int err;
        nanos::mpi::gatherv<nanos::mpi::ticket>( sendbuf, sendcount, sendtype,
                        recvbuf, recvcounts, displs, recvtype,
                        root, comm, &err );
        return err;
    }
} // extern C

namespace nanos {
namespace mpi {

    shared_pointer<ticket> igatherv( const void *sendbuf, int sendcount, MPI_Datatype sendtype,
        void *recvbuf, const int recvcounts[], const int displs[], 
	     MPI_Datatype recvtype, int root, MPI_Comm comm )
    {
        shared_pointer<ticket> result( new ticket() );
        int err = MPI_Igatherv( sendbuf, sendcount, sendtype, 
                    recvbuf, recvcounts, displs, recvtype, 
                    root, comm, result->getChecker().getRequest() );
        result->getChecker().setError( err );

        return result;
    }

} // namespace mpi
} // namespace nanos

#endif // MPI_VERSION


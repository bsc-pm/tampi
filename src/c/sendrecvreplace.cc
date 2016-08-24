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
#include "print.h"

extern "C" {

int MPI_Sendrecv_replace( void *buf, int count, MPI_Datatype datatype,
                       int dest, int sendtag,
                       int source, int recvtag,
                       MPI_Comm comm, MPI_Status *status)
{
    /*
     * Note: this implementation tries to mimic OpenMPI's: 
     * https://github.com/open-mpi/ompi/blob/master/ompi/mpi/c/sendrecv_replace.c
     * First, it creates a temporary buffer where it stores the received message in
     * countiguous storage. Then, it unpacks it in the original -- user provided --
     * buffer and releases the temporary buffer.
     */

    if ( source == MPI_PROC_NULL || dest == MPI_PROC_NULL || count == 0 ) {
        return MPI_Sendrecv( buf, count, datatype, dest, sendtag,
                             buf, count, datatype, source, recvtag,
                             comm, status );
    } else {
        print::intercepted_call( __func__ );

        int err = MPI_SUCCESS;
        int alloc = MPI_SUCCESS;
        int packed_size;
        void *helperbuf = nullptr;

        /* setup a buffer for recv */
        err = MPI_Pack_size( count, datatype, comm, &packed_size );
        alloc = MPI_Alloc_mem( packed_size, MPI_INFO_NULL, &helperbuf );

        /* recv into temporary buffer */
        if ( err == MPI_SUCCESS  && alloc == MPI_SUCCESS ) {
            err = MPI_Sendrecv( buf, count, datatype, dest, sendtag,
                           helperbuf, packed_size, MPI_BYTE, source, recvtag,
                           comm, status);
        }

        /* copy into users buffer */
        if ( err == MPI_SUCCESS && alloc == MPI_SUCCESS ) {
            int position = 0;
            err = MPI_Unpack( helperbuf, packed_size, &position,
                        buf, count, datatype, comm );
        }

        /* release resources */
        if ( alloc == MPI_SUCCESS ) {
            err = MPI_Free_mem( helperbuf );
        }
        return err;
    }
}

} // extern C


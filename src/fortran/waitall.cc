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

#include "array_utils.h"
#include "mpi/common.h"
#include "mpi/request.h"
#include "mpi/status.h"
#include "ticket.h"
#include "print.h"

using namespace nanos::mpi;
using namespace nanos::utils;

extern "C" {
   void mpi_waitall_( MPI_Fint *count, MPI_Fint array_of_requests[],
         MPI_Fint *array_of_statuses, MPI_Fint *err )
   {
      nanos::log::intercepted_call( __func__ );

      // TODO: copy back status information when not ignored
      //if( array_of_statuses == MPI_F_STATUSES_IGNORE ) {
      Fortran::Ticket ticket( array_of_requests, array_of_requests+(*count) );
      ticket.wait();
      //} else {
      //}
   }
} // extern C


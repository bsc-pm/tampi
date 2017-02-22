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
#include <dlfcn.h>
#include <cassert>

#include "environment.h"

using namespace nanos::mpi;

template<>
TicketQueue<Fortran::Ticket>* Fortran::environment::_queue = nullptr;

extern "C" {

void mpi_init_( MPI_Fint * err )
{
   // Look for next defined MPI_Init
   // Used to support other profiling tools
   void (*mpi_init_fn)(MPI_Fint*);
   mpi_init_fn = (void (*)(MPI_Fint*)) dlsym(RTLD_NEXT, "mpi_init_");
   assert(mpi_init_fn != 0);

   // Initialize the environment
   Fortran::environment::initialize();

   // Call MPI_Init
   mpi_init_fn(err);
}

void mpi_init_thread_( MPI_Fint * required, MPI_Fint * provided, MPI_Fint * err )
{
   // Look for next defined MPI_Init
   // Used to support other profiling tools
   void (*mpi_init_thread_fn)(MPI_Fint*, MPI_Fint*, MPI_Fint*);
   mpi_init_thread_fn = (void (*)(MPI_Fint*, MPI_Fint*, MPI_Fint*)) dlsym(RTLD_NEXT, "mpi_init_thread_");
   assert(mpi_init_thread_fn != 0);

   // Initialize the environment
   Fortran::environment::initialize();

   // Call MPI_Init_thread
   mpi_init_thread_fn(required, provided, err);
}

void mpi_finalize_( MPI_Fint * err )
{
   // Look for next defined MPI_Finalize
   // Used to support other profiling tools
   void (*mpi_finalize_fn)(MPI_Fint*);
   mpi_finalize_fn = (void (*)(MPI_Fint*)) dlsym(RTLD_NEXT, "mpi_finalize_");
   assert(mpi_finalize_fn != 0);

   // Call MPI_Finalize
   mpi_finalize_fn(err);

   // Finalize the environment
   Fortran::environment::finalize();
}

} // extern C

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
#include <dlfcn.h>
#include <cassert>
#include <stdarg.h>

#include "configuration.h"
#include "environment.h"

using namespace nanos::mpi;

template<>
TicketQueue<C::Ticket>* C::environment::_queue = nullptr;

extern "C" {

int MPI_Init( int *argc, char*** argv )
{
    // Look for next defined MPI_Init
    // Used to support other profiling tools
    int (*mpi_init_fn)(int*, char ***);
    mpi_init_fn = (int (*)(int*, char***)) dlsym(RTLD_NEXT, "MPI_Init");
    assert(mpi_init_fn != 0);

    // Initialize the environment
    C::environment::initialize();

    // Call MPI_Init
    int error = mpi_init_fn(argc, argv);
    return error;
}

int MPI_Init_thread( int *argc, char ***argv, int required, int *provided )
{
    // Look for next defined MPI_Init
    // Used to support other profiling tools
    int (*mpi_init_thread_fn)(int*, char ***, int, int*);
    mpi_init_thread_fn = (int (*)(int*, char***, int, int*)) dlsym(RTLD_NEXT, "MPI_Init_thread");
    assert(mpi_init_thread_fn != 0);

    // Initialize the environment
    C::environment::initialize();

    // Call MPI_Init_thread
    int error = mpi_init_thread_fn(argc, argv, required, provided);
    return error;
}

int MPI_Finalize()
{
    // Look for next defined MPI_Finalize
    // Used to support other profiling tools
    int (*mpi_finalize_fn)();
    mpi_finalize_fn = (int (*)()) dlsym(RTLD_NEXT, "MPI_Finalize");
    assert(mpi_finalize_fn != 0);

    // Call MPI_Finalize
    int error = mpi_finalize_fn();

    // Finalize the environment
    C::environment::finalize();

    return error;
}

int MPI_Pcontrol( int level, ... )
{
   va_list args;
   va_start( args, level );

   int task_level = va_arg( args, int );
   int num_spins  = va_arg( args, int );

   config.reset(level);

   if( !config.tlsTuneDisabled() ) {
      nanos::tls_view task_local_storage;
      detail::WaitProperties waitMode( task_level, num_spins );
      task_local_storage.store( waitMode );
   }

   va_end( args );

   return MPI_SUCCESS;
}

} // extern C

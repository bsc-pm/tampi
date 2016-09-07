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

//#include "debug/mpierrortranslator.hpp"
#include "environment.h"

nanos::mpi::TicketQueue* nanos::mpi::TicketQueue::_queue = nullptr;

/*!
 * \file init-finalize.cc This file is intended for debug purposes only.
 * Use is not recommended along profiling tools as it may not be compatible
 * with them.
 */

extern "C" {

int MPI_Init( int *argc, char*** argv )
{
    // Look for next defined MPI_Init
    // Used to support other profiling tools
    int (*mpi_init_fn)(int*, char ***);
    mpi_init_fn = (int (*)(int*, char***)) dlsym(RTLD_NEXT, "MPI_Init");
    assert(mpi_init_fn != 0);

	// Initialize the environment
	nanos::mpi::environment::initialize();

    // Call MPI_Init
    int error = mpi_init_fn(argc, argv);

    // Set MPI error handler to throw exceptions in MPI_COMM_WORLD
    //nanos::error::MPIErrorTranslator<MPI_Comm>(MPI_COMM_WORLD);

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
    nanos::mpi::environment::initialize();

    // Call MPI_Init_thread
    int error = mpi_init_thread_fn(argc, argv, required, provided);

    // Set MPI error handler to throw exceptions in MPI_COMM_WORLD
    //nanos::error::MPIErrorTranslator<MPI_Comm>(MPI_COMM_WORLD);

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
	nanos::mpi::environment::finalize();

	return error;
}

} // extern C

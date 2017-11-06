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

#include <cassert>
#include <dlfcn.h>
#include <mpi.h>
#include <stdarg.h>

#include "definitions.h"
#include "environment.h"
#include "symbols.h"

extern "C" {

int MPI_Init(int *argc, char*** argv)
{
	static C::MPI_Init_t *symbol = (C::MPI_Init_t *) Symbol::loadNextSymbol(__func__);
	
	// Disable the interoperability
	C::Environment::disable();
	
	// Call MPI_Init
	return (*symbol)(argc, argv);
}

int MPI_Init_thread(int *argc, char ***argv, int required, int *provided)
{
	static C::MPI_Init_thread_t *symbol = (C::MPI_Init_thread_t *) Symbol::loadNextSymbol(__func__);
	
	// Call MPI_Init_thread
	int err = (*symbol)(argc, argv, required, provided);
	
	// Initialize the environment if needed
	if (required == MPI_TASK_MULTIPLE && *provided != MPI_TASK_MULTIPLE) {
		C::Environment::enable();
		C::Environment::initialize();
		*provided = MPI_TASK_MULTIPLE;
	} else {
		C::Environment::disable();
	}
	
	return err;
}

int MPI_Finalize()
{
	static C::MPI_Finalize_t *symbol = (C::MPI_Finalize_t *) Symbol::loadNextSymbol(__func__);
	
	// Call MPI_Finalize
	int err = (*symbol)();
	
	// Finalize the environment
	if (C::Environment::isEnabled()) {
		C::Environment::finalize();
	}
	
	return err;
}

} // extern C

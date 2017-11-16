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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <mpi.h>
#include <dlfcn.h>
#include <cassert>

#include "definitions.h"
#include "environment.h"
#include "symbols.h"

extern "C" {

void mpi_init_(MPI_Fint *err)
{
	static Fortran::mpi_init_t *symbol = (Fortran::mpi_init_t *) Symbol::loadNextSymbol(__func__);
	
	// Call to MPI_Init
	(*symbol)(err);
	
	// Disable the interoperability
	Fortran::Environment::disable();
}

void mpi_init_thread_(MPI_Fint *required, MPI_Fint *provided, MPI_Fint *err)
{
	static Fortran::mpi_init_thread_t *symbol = (Fortran::mpi_init_thread_t *) Symbol::loadNextSymbol(__func__);
	
	/* NOTE: This should be modified when MPI_TASK_MULTIPLE
 	 * mode is supported by MPI */
	MPI_Fint actualRequired = *required;
	if (actualRequired == MPI_TASK_MULTIPLE) {
		actualRequired = MPI_THREAD_MULTIPLE;
	}
	
	// Call to MPI_Init_thread
	(*symbol)(&actualRequired, provided, err);
	
	if (*required == MPI_TASK_MULTIPLE && *provided == MPI_THREAD_MULTIPLE) {
		Fortran::Environment::enable();
		Fortran::Environment::initialize();
		*provided = MPI_TASK_MULTIPLE;
	} else {
		Fortran::Environment::disable();
	}
}

void mpi_finalize_(MPI_Fint *err)
{
	static Fortran::mpi_finalize_t *symbol = (Fortran::mpi_finalize_t *) Symbol::loadNextSymbol(__func__);
	
	// Call MPI_Finalize
	(*symbol)(err);
	
	// Finalize the interoperability
	if (Fortran::Environment::isEnabled()) {
		Fortran::Environment::finalize();
	}
}

} // extern C

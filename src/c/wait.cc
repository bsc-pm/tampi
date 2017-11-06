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

#include <dlfcn.h>
#include <mpi.h>

#include "definitions.h"
#include "environment.h"
#include "process_request.h"
#include "symbols.h"

extern "C" {
	int MPI_Wait(MPI_Request *request, MPI_Status *status)
	{
		int err = MPI_SUCCESS;
		if (C::Environment::isEnabled()) {
			assert(request != NULL);
			C::processRequest(*request, status);
		} else {
			static C::MPI_Wait_t *symbol = (C::MPI_Wait_t *) Symbol::loadNextSymbol(__func__);
			err = (*symbol)(request, status);
		}
		return err;
	}
} // extern C


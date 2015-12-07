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
#include "debug/mpierrortranslator.hpp"

/*!
 * \file init-finalize.cc This file is intended for debug purposes only.
 * Do not use with profiling tools as it is not compatible with them.
 */

extern "C" {

int MPI_Init( int *argc, char*** argv )
{
	int error = PMPI_Init(argc, argv);
	nanos::error::MPIErrorTranslator<MPI_Comm>(MPI_COMM_WORLD);
}

int MPI_Finalize()
{
	return PMPI_Finalize();
}

} // extern C

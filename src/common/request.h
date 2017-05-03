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

#ifndef REQUEST_H
#define REQUEST_H

#include <mpi.h>

// MPI Test fortran specializations
extern "C" {

//! MPI_Request_get_status Fortran API declaration
void pmpi_request_get_status_( MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint* );

//! MPI_Test Fortran API declaration
void pmpi_test_( MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint* );

//! MPI_Testall Fortran API declaration
void pmpi_testall_( MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint[], MPI_Fint* );

//! MPI_Testany Fortran API declaration
void pmpi_testany_( MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint* );

//! MPI_Testsome Fortran API declaration
void pmpi_testsome_( MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint[], MPI_Fint[], MPI_Fint* );

} // extern C

#endif // REQUEST_H

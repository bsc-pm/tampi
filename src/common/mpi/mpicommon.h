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
#ifndef COMMON_H
#define COMMON_H

#include <mpi.h>

/*! \file mpicommon.h
  Specifies which datatypes are used for each kind of data in each
  language.
  Examples: requests.
    - C:       MPI_Request
    - Fortran: MPI_Fint
 */

//! Determines the number of elements that define an status
/*!
  Statuses in MPI are composed by few integers that can either
  represent counters or error codes.
  In C, they are defined in a struct. However, in Fortran they are
  defined as an integer array, so we need to know how many integers
  an status is composed of to create the correct representation of
  Fortran MPI statuses in C/C++ structures.
 */
#define SIZEOF_MPI_STATUS sizeof(MPI_Status)/sizeof(int)

//! Read only buffers must be defined as const in versions MPI 3.0 and later.
#if MPI_VERSION >= 3
    #define MPI3CONST const
#else
    #define MPI3CONST
#endif

namespace nanos {
namespace mpi {

namespace C {

using request_type = MPI_Request;
using status_type = MPI_Status;
using comm_type = MPI_Comm;
using int_type = int;

} // namespace C

namespace Fortran {

using request_type = MPI_Fint;
using status_type = MPI_Fint[SIZEOF_MPI_STATUS];
using comm_type = MPI_Fint;
using int_type = MPI_Fint;

} // namespace Fortran

/*
template < typename T >
struct type_traits;

template <>
struct type_traits<C::comm_type>
{
	using comm_type = C::comm_type;
	using int_type = C::int_type;
	using request_type = C::request_type;
	using status_type = C::status_type;
};

template <>
struct type_traits<Fortran::comm_type>
{
	using comm_type = Fortran::comm_type;
	using int_type = Fortran::int_type;
	using request_type = Fortran::request_type;
	using status_type = Fortran::status_type;
};
*/

} // namespace mpi
} // namespace nanos

#endif // COMMON_H

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

#endif // COMMON_H

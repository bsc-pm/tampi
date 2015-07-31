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

typedef MPI_Request request_type;
typedef MPI_Status  status_type;
typedef MPI_Comm    comm_type;
typedef int         int_type;

} // namespace C

namespace Fortran {

typedef MPI_Fint    request_type;
typedef MPI_Fint    status_type[SIZEOF_MPI_STATUS];
typedef MPI_Fint    comm_type;
typedef MPI_Fint    int_type;

} // namespace Fortran

//! Type trait that helps to define sets of datatypes for each language.
/*!
  \param comm_type Communicator data type (MPI_Comm/MPI_Fint) that is being used.
 */
template < typename comm_type >
struct MPITraits;

template <>
struct MPITraits< C::comm_type > {
	typedef C::request_type request_type;
	typedef C::status_type status_type;
	typedef C::int_type int_type;
};

template <>
struct MPITraits< Fortran::comm_type* > {
	typedef Fortran::request_type request_type;
	typedef Fortran::status_type status_type;
	typedef Fortran::int_type int_type;
};

} // namespace mpi
} // namespace nanos

#endif // COMMON_H

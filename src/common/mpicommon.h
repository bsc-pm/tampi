#ifndef COMMON_H
#define COMMON_H

#include <mpi.h>

#define SIZEOF_MPI_STATUS sizeof(MPI_Status)/sizeof(int)

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

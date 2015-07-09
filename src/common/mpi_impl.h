#ifndef TICKET_TRAITS_H
#define TICKET_TRAITS_H

#include <array>
#include <iterator>

#include "mpicommon.h"
#include "status.h"

extern "C" {
void mpi_test_( MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint* );
void mpi_testall_( MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint[][SIZEOF_MPI_STATUS], MPI_Fint* );
}

namespace nanos {
namespace mpi {

template <size_t count=0>
struct test_impl {

static bool test ( size_t nelems, C::request_type requests[], C::status_type statuses[], C::int_type *error )
{
    Flag<int> flag;
    *error = MPI_Testall( nelems, requests, flag, statuses );
    return flag;
}

static bool test ( size_t nelems, Fortran::request_type requests[], Fortran::status_type statuses[], Fortran::int_type *error )
{
    Flag<MPI_Fint> flag;
    mpi_testall_( (MPI_Fint*)&nelems, requests, flag, 
                   statuses, // array of statuses
                   error );
    return flag;
}

};

template<>
struct test_impl<1> {

static bool test ( std::size_t nelems, C::request_type *request, C::status_type *status, C::int_type *error )
{
    Flag<int> flag;
    *error = MPI_Test( request, flag, status );
    return flag;
}

static bool test ( std::size_t nelems, Fortran::request_type *request, Fortran::status_type *status, Fortran::int_type *error )
{
    Flag<MPI_Fint> flag;
    mpi_test_( request, flag, reinterpret_cast<MPI_Fint*>(status), error );
    return flag;
}

};

} // namespace mpi
} // namespace nanos

#endif

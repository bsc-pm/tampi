#ifndef TICKET_TRAITS_H
#define TICKET_TRAITS_H

#include <array>
#include <iterator>

#include "mpicommon.h"
#include "status.h"

extern "C" {
//! MPI_Test Fortran API declaration
void mpi_test_( MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint* );

//! MIP_Testall Fortran API declaration
void mpi_testall_( MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint[][SIZEOF_MPI_STATUS], MPI_Fint* );
}

namespace nanos {
namespace mpi {

//! Provides an abstraction layer to call MPI_Test/MPI_Testall 
/*!
  It calls the corresponding MPI Fortran or C function.
 */
template <size_t count=0>
struct test_impl {
//! Test if requests has been completed. Overload for C data types.
/*!
  \return Boolean that indicates whether the communication has finished or not.
 */
static bool test ( size_t nelems, C::request_type requests[], C::status_type statuses[], C::int_type *error )
{
    Flag<int> flag;
    *error = MPI_Testall( nelems, requests, flag, statuses );
    return flag;
}

//! Test if requests has been completed. Overload for Fortan data types.
/*!
  \return Boolean that indicates whether the communication has finished or not.
 */
static bool test ( size_t nelems, Fortran::request_type requests[], Fortran::status_type statuses[], Fortran::int_type *error )
{
    Flag<MPI_Fint> flag;
    mpi_testall_( (MPI_Fint*)&nelems, requests, flag, 
                   statuses, // array of statuses
                   error );
    return flag;
}

};

//! Provides an abstraction layer to call MPI_Test
/*!
  It calls the corresponding MPI Fortran or C function.
  This is an overload for single request tests.
 */
template<>
struct test_impl<1> {

//! Test if a single request has been completed. Overload for C data types.
/*!
  \return Boolean that indicates whether the communication has finished or not.
 */
static bool test ( std::size_t nelems, C::request_type *request, C::status_type *status, C::int_type *error )
{
    Flag<int> flag;
    *error = MPI_Test( request, flag, status );
    return flag;
}

//! Test if a single request has been completed. Overload for Fortan data types.
/*!
  \return Boolean that indicates whether the communication has finished or not.
 */
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

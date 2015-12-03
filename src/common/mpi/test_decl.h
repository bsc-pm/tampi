
#ifndef TEST_DECL_H
#define TEST_DECL_H

#include "request_fwd.h"
#include "status_fwd.h"
#include "requestset_fwd.h"
#include "statusset_fwd.h"

#include <mpi.h>

#include <type_traits>

extern "C" {
//! MPI_Test Fortran API declaration
void mpi_test_( MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint* );

//! MPI_Testall Fortran API declaration
void mpi_testall_( MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint[], MPI_Fint* );
} // extern C

namespace nanos {
namespace mpi {

template < class request, class status >
bool test_impl( request &req, status &st );

template < class request >
bool test_impl( request &req );

template < class request, class status, size_t length >
bool testall_impl( RequestSet<request,length> &requests, StatusSetBase<status,length> &statuses );

template < class request, size_t length >
bool testall_impl( RequestSet<request,length> &requests );

} // namespace mpi
} // namespace nanos

#endif // TEST_DECL_H

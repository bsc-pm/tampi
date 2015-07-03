#ifndef TICKET_TRAITS_H
#define TICKET_TRAITS_H

#include "mpicommon.h"

extern "C" {
void mpi_test_( MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint* );
void mpi_testall_( MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint[][SIZEOF_MPI_STATUS], MPI_Fint* );
}

namespace nanos {
namespace mpi {

template <int count=-1>
struct test_impl {

static bool test ( std::size_t count, MPI_Request requests[], MPI_Status statuses[], int &error )
{
    Flag<int> flag;
    error = MPI_Testall( count, requests, flag, statuses );
    return flag;
}

static bool test ( std::size_t count, MPI_Fint requests[], MPI_Fint statuses[][SIZEOF_MPI_STATUS], MPI_Fint &error )
{
    Flag<MPI_Fint> flag;
    mpi_testall_( (MPI_Fint*)&count, requests, flag, statuses, &error );
    return flag;
}

};

struct test_impl<1> {

static bool test ( std::size_t count, MPI_Request *request, MPI_Status *status, int &error )
{
    Flag<int> flag;
    error = MPI_Test( request, flag, status );
    return flag;
}

static bool test ( std::size_t count, MPI_Fint *request, MPI_Fint *status, MPI_Fint &error )
{
    Flag<MPI_Fint> flag;
    mpi_test_( request, flag, status, &error );
    return flag;
}

};

} // namespace mpi
} // namespace nanos

#endif

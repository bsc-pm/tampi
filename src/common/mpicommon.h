#ifndef COMMON_H
#define COMMON_H

#include <mpi.h>

#define SIZEOF_MPI_STATUS sizeof(MPI_Status)/sizeof(int)

#if MPI_VERSION >= 3
    #define MPI3CONST const
#else
    #define MPI3CONST
#endif

namespace C {

typedef MPI_Request request_type;
typedef MPI_Status  status_type;
typedef int         int_type;

} // namespace C

namespace Fortran {

typedef MPI_Fint    request_type;
typedef MPI_Fint    status_type[SIZEOF_MPI_STATUS];
typedef MPI_Fint    int_type;

} // namespace Fortran

#endif // COMMON_H

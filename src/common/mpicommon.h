#ifndef COMMON_H
#define COMMON_H

#include <mpi.h>

#define SIZEOF_MPI_STATUS sizeof(MPI_Status)/sizeof(int)

#if MPI_VERSION >= 3
    #define MPI3CONST const
#else
    #define MPI3CONST
#endif

#endif // COMMON_H

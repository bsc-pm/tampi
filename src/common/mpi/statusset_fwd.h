
#ifndef STATUS_SET_FWD_H
#define STATUS_SET_FWD_H

#include <mpi.h>
#include "status_fwd.h"

namespace nanos {
namespace mpi {

template < class Status, size_t length >
class StatusSetBase;

namespace C {
using status_plain = MPI_Status;
using status_array_plain = MPI_Status[];

template < StatusKind kind = StatusKind::attend, size_t length = 0 >
using StatusSet = StatusSetBase<status<kind>, length>;
}

namespace Fortran {
using status_plain = MPI_Fint(&)[SIZEOF_MPI_STATUS];
using status_array_plain = MPI_Fint(*)[SIZEOF_MPI_STATUS];

template < StatusKind kind = StatusKind::attend, size_t length = 0 >
using StatusSet = StatusSetBase<status<kind>, length>;
}

}// namespace nanos
}// namespace mpi

#endif // STATUS_SET_FWD_H

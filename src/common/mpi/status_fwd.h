
#ifndef MPI_STATUS_FWD_H
#define MPI_STATUS_FWD_H

#include <mpi.h>
#include <algorithm>
#include <array>

namespace nanos {
namespace mpi {

using fortran_status = std::array<MPI_Fint,SIZEOF_MPI_STATUS>;
using fortran_statuses_array = fortran_status[];

enum class StatusKind
{
	ignore,
	attend
};

template < typename UnderlyingType, StatusKind kind = StatusKind::attend >
class status;

}// namespace nanos
}// namespace mpi

#endif // MPI_STATUS_FWD_H

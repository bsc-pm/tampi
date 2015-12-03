
#ifndef REQUEST_SET_FWD_H
#define REQUEST_SET_FWD_H

#include <mpi.h>
#include "status_fwd.h"

namespace nanos {
namespace mpi {

template < class Request, size_t length = 0 >
class RequestSet;

}// namespace nanos
}// namespace mpi

#endif // REQUEST_SET_FWD_H

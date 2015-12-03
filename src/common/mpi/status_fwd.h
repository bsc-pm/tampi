
#ifndef STATUS_FWD_H
#define STATUS_FWD_H

#include <mpi.h>
#include <algorithm>
#include <array>

namespace nanos {
namespace mpi {

#ifdef DEBUG_MODE
enum class StatusKind
{
	ignore = 1,
	attend = 1
};
#else
enum class StatusKind
{
	ignore = 1,
	attend = 0
};
#endif

namespace Fortran {
	template < StatusKind kind = StatusKind::attend >
	class status;
}

namespace C {
	template < StatusKind kind = StatusKind::attend >
	class status;
}

template < typename T >
struct ignore : public std::false_type
{
};

template <>
struct ignore<C::status<StatusKind::ignore> > : public std::true_type
{
};

template <>
struct ignore<Fortran::status<StatusKind::ignore> > : public std::true_type
{
};

}// namespace nanos
}// namespace mpi

#endif // STATUS_FWD_H

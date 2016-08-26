
#ifndef STATUS_H
#define STATUS_H

#include "common.h"

#include <cassert>
#include <mpi.h>

#include <algorithm>
#include <array>

namespace nanos {
namespace mpi {

namespace C {

class status
{
	public:
		using value_type = MPI_Status;
		using base_type = MPI_Status;

	private:
		value_type _value;
	
	public:
		status() :
			_value()
		{
		}
	
		status( value_type const& value ) :
			_value(value)
		{
		}
	
		status( status const& other ) = default;

		status& operator=( status const& other ) = default;

		status& operator=( value_type const& other )
		{
			_value = other;
			return *this;
		}

		operator value_type& ()
		{
			return _value;
		}

		operator const value_type& () const
		{
			return _value;
		}

		operator value_type* ()
		{
			return &_value;
		}
};

#ifdef DEBUG_MODE
using ignored_status = status;
#else
class ignored_status
{
	public:
		using value_type = MPI_Status;
		using base_type = MPI_Status;

		operator value_type* ()
		{
			return MPI_STATUS_IGNORE;
		}
};
#endif

} // namespace C

namespace Fortran {

class status
{
	public:
		using value_type = MPI_Fint;
		using base_type = std::array<MPI_Fint,SIZEOF_MPI_STATUS>;

	private:
		base_type _value;

	public:
		status() :
			_value()
		{
		}

		status( status const& other ) = default;

		status( value_type const* value ) :
			_value( reinterpret_cast<base_type const&>(*value) )
		{
			assert( value != MPI_F_STATUS_IGNORE );
		}

		status& operator=( status const& other ) = default;

		status& operator=( base_type const& other )
		{
			_value = other;
			return *this;
		}

		operator value_type* ()
		{
			return _value.data();
		}

		operator base_type& ()
		{
			return _value;
		}
};

#ifdef DEBUG_MODE
using ignored_status = status;
#else
class ignored_status
{
	public:
		using value_type = MPI_Fint;
		using base_type = std::array<MPI_Fint,SIZEOF_MPI_STATUS>;

		operator value_type* ()
		{
			return MPI_F_STATUS_IGNORE;
		}
};
#endif

} // namespace Fortran

} // namespace mpi
} // namespace nanos

#endif // STATUS_H

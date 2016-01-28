
#ifndef STATUS_H
#define STATUS_H

#include "common.h"
#include "status_fwd.h"

#include <cassert>
#include <mpi.h>

#include <algorithm>
#include <array>

namespace nanos {
namespace mpi {

namespace C {

template <>
class status<StatusKind::attend>
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

		void copy( value_type *other ) const
		{
			if( other != MPI_STATUS_IGNORE )
				*other = _value;
		}
	
		operator value_type& ()
		{
			return _value;
		}

		operator value_type* ()
		{
			return &_value;
		}
};

#ifndef DEBUG_MODE
template <>
class status<StatusKind::ignore>
{
	public:
		using value_type = MPI_Status;
		using base_type = MPI_Status;

		operator value_type* ()
		{
			return MPI_STATUS_IGNORE;
		}

        void copy( value_type *other ) const
        {
        }
};
#endif

} // namespace C

namespace Fortran {

template <>
class status<StatusKind::attend>
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

		void copy( value_type *other ) const
		{
			if( other != MPI_F_STATUS_IGNORE ) {
				auto array = reinterpret_cast<base_type&>(*other);
				array = _value;
			}
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

#ifndef DEBUG_MODE
template <>
class status<StatusKind::ignore>
{
	public:
		using value_type = MPI_Fint;
		using base_type = std::array<MPI_Fint,SIZEOF_MPI_STATUS>;

		operator value_type* ()
		{
			return MPI_F_STATUS_IGNORE;
		}

        void copy( value_type *other ) const
        {
        }
};
#endif

} // namespace Fortran

} // namespace mpi
} // namespace nanos

#endif // STATUS_H

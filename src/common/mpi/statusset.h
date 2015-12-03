
#ifndef STATUS_SET_H
#define STATUS_SET_H

#include "statusset_fwd.h"
#include "status.h"

#include <cassert>
#include <mpi.h>

#include <array>
#include <vector>

namespace nanos {
namespace mpi {

template <size_t length>
class StatusSetBase< C::status<StatusKind::attend>, length>
{
	private:
		using status = C::status<StatusKind::attend>;

		std::array<status,length> _statuses;

	public:
		StatusSetBase() :
			_statuses()
		{
		}

		StatusSetBase( StatusSetBase const& other ) :
			_statuses( other._statuses )
		{
		}

		StatusSetBase( status::value_type const* statuses, size_t size ) :
			_statuses()
		{
			assert( statuses != MPI_STATUSES_IGNORE )
			assert( size <= length );
			auto begin = statuses;
			auto end = statuses + size;
			std::copy( begin, end, _statuses.begin() );
		}

		operator status::value_type* ()
		{
			return reinterpret_cast<status::value_type*>( _statuses.data() );
		}

		void copy( status::value_type* status )
		{
			*status = static_cast<status::value_type>( _statuses.back() );
		}

		void copy( MPI_Status array_of_statuses[], size_t size )
		{
			assert( size >= _statuses.size() );
			std::copy( _statuses.begin(), _statuses.end(), array_of_statuses );
		}
};

template <>
class StatusSetBase<C::status<StatusKind::attend>, 0>
{
	private:
		using status = C::status<StatusKind::attend>;

		std::vector<status> _statuses;

	public:
		StatusSetBase() :
			_statuses()
		{
		}

		StatusSetBase( StatusSetBase const& other ) :
			_statuses( other._statuses )
		{
		}

		StatusSetBase( status::value_type const* statuses, size_t size ) :
			_statuses()
		{
			assert( statuses != MPI_STATUSES_IGNORE );
			_statuses.reserve(size);
			std::copy( statuses, statuses+size, _statuses.begin() );
		}

		operator status::value_type* ()
		{
			_statuses.data();
		}

		void copy( status::value_type* status )
		{
			assert( status != MPI_STATUS_IGNORE );
			*status = static_cast<status::value_type>( _statuses.back() );
		}

		void copy( status::value_type array_of_statuses[], size_t size )
		{
			assert( size >= _statuses.size() );
			auto begin = _statuses.begin();
			auto end = _statuses.end();
			std::copy( begin, end, array_of_statuses );
		}
};

template <size_t length>
class StatusSetBase<Fortran::status<StatusKind::attend>, length>
{
	private:
		using status = Fortran::status<StatusKind::attend>;

		std::array<status,length> _statuses;

	public:
		StatusSetBase() :
			_statuses()
		{
		}

		StatusSetBase( StatusSetBase const& other ) = default;

		StatusSetBase( status::value_type const* statuses, size_t size ) :
			_statuses()
		{
			assert( size <= length );
			assert( statuses != MPI_F_STATUSES_IGNORE );

			// Need to reinterpret an array of MPI_Fint
			// into a contiguous buffer of MPI_Fint[SIZEOF_MPI_STATUS]
			// so that the pointer arithmetic is done correctly
			// (we want to advance SIZEOF_MPI_STATUS elements each iteration)
			auto begin = reinterpret_cast<status::base_type const*>(*statuses);
			auto end = begin + size;
			std::copy( begin, end, _statuses.begin() );
		}

		StatusSetBase& operator=( StatusSetBase const& other ) = default;

		operator status::value_type* ()
		{
			return reinterpret_cast<status::value_type*>( _statuses.data() );
		}

		void copy( status::value_type *status )
		{
			assert( status != MPI_F_STATUS_IGNORE );
			*status = static_cast<status::value_type>( _statuses.back() );
		}

		void copy( status::value_type array_of_statuses[], size_t size )
		{
			assert( size <= length );
			std::copy( _statuses.begin(), _statuses.end(), reinterpret_cast<status::base_type*>(array_of_statuses) );
		}
};

template <>
class StatusSetBase<Fortran::status<StatusKind::attend>, 0>
{
	private:
		using status = Fortran::status<StatusKind::attend>;

		std::vector<status> _statuses;

	public:
		StatusSetBase() :
			_statuses()
		{
		}

		StatusSetBase( StatusSetBase const& other ) :
			_statuses( other._statuses )
		{
		}

		StatusSetBase( status::value_type const* array_of_statuses, size_t size ) :
			_statuses()
		{
			assert( array_of_statuses != MPI_F_STATUSES_IGNORE );

			_statuses.reserve(size);
			auto begin = reinterpret_cast<status::base_type const*>(array_of_statuses);
			auto end = begin + size;
			std::copy( begin, end, _statuses.begin() );
		}

		operator status::value_type* ()
		{
			return reinterpret_cast<status::value_type*>(_statuses.data());
		}

		void copy( status::value_type *status )
		{
			assert( status != MPI_F_STATUS_IGNORE );
			auto st_array = reinterpret_cast<status::base_type&>(*status);
			st_array = static_cast<status::base_type>(_statuses.back());
		}

		void copy( status::value_type array_of_statuses[], size_t size )
		{
			assert( array_of_statuses != MPI_F_STATUSES_IGNORE );
			assert( _statuses.size() <= size );

			std::copy( _statuses.begin(), _statuses.end(), reinterpret_cast<status::base_type*>(array_of_statuses) );
		}
};

#ifndef DEBUG_MODE
//! Note: Status is never ignored in debug mode.

template < size_t length >
class StatusSetBase<C::status<StatusKind::ignore>, length>
{
	private:
		using status = C::status<StatusKind::ignore>;
	public:
		StatusSetBase() = default;

		StatusSetBase( MPI_Status array_of_statuses[], size_t size )
		{
			assert( value = MPI_STATUSES_IGNORE );
		}

		operator status::value_type* ()
		{
			return MPI_STATUSES_IGNORE;
		}

		void copy( status::value_type *status )
		{
		}

		void copy( status::value_type array_of_statuses[], size_t size )
		{
		}
};

template < size_t length >
class StatusSetBase<Fortran::status<StatusKind::ignore>, length> {
	private:
		using status = Fortran::status<StatusKind::ignore>;
	public:
		StatusSetBase() = default;

		StatusSetBase( StatusSetBase const& other ) = default;

		StatusSetBase( status::value_type *value, size_t size )
		{
			assert( vaule == MPI_F_STATUSES_IGNORE );
		}

		operator status::value_type* ()
		{
			return MPI_F_STATUSES_IGNORE;
		}

		void copy( status::value_type *status )
		{
			assert( vaule == MPI_F_STATUS_IGNORE );
		}

		void copy( status::value_type array_of_statuses[], size_t size )
		{
			assert( vaule == MPI_F_STATUSES_IGNORE );
		}
};
#endif

} // namespace mpi
} // namespace nanos

#endif // STATUS_SET_H


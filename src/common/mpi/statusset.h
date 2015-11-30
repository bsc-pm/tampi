
#ifndef STATUS_SET_H
#define STATUS_SET_H

#include "status.h"

#include <mpi.h>

#include <array>
#include <vector>

namespace nanos {
namespace mpi {

template < typename UnderlyingType, StatusKind kind = StatusKind::attend, size_t length = 0 >
class StatusSet;

template <size_t length>
class StatusSet<MPI_Status, StatusKind::attend, length>
{
	private:
		std::array<status<MPI_Status,StatusKind::attend>,length> _statuses;

	public:
		StatusSet() :
			_statuses()
		{
		}

		StatusSet( StatusSet const& other ) :
			_statuses()
		{
			std::copy( other._statuses.begin(), other._statuses.end(), _statuses.begin() );
		}

		StatusSet( MPI_Status const* statuses, size_t size ) :
			_statuses()
		{
			//assert( statuses != MPI_STATUSES_IGNORE )
			auto begin = statuses;
			auto end = statuses + size;
			std::copy( begin, end, _statuses.begin() );
		}

		operator MPI_Status* ()
		{
			return _statuses.data();
		}

		void copy( MPI_Status* status )
		{
			*status = _statuses.front();
		}

		void copy( MPI_Status array_of_statuses[], size_t size )
		{
			//assert( size >= _statuses.size() );
			std::copy( _statuses.begin(), _statuses.end(), array_of_statuses );
		}
};

template <>
class StatusSet<MPI_Status, StatusKind::attend, 0>
{
	private:
		std::vector<status<MPI_Status,StatusKind::attend> > _statuses;

	public:
		StatusSet() :
			_statuses()
		{
		}

		StatusSet( StatusSet const& other ) :
			_statuses( other._statuses )
		{
		}

		StatusSet( MPI_Status const* statuses, size_t size ) :
			_statuses()
		{
			//assert( statuses != MPI_STATUSES_IGNORE );
			_statuses.reserve(size);
			std::copy( statuses, statuses+size, _statuses.begin() );
		}

		operator MPI_Status* ()
		{
			_statuses.data();
		}

		void copy( MPI_Status* status )
		{
			auto begin = _statuses.begin();
			auto end = _statuses.begin() +1;
			std::copy( begin, end, status );
		}

		void copy( MPI_Status array_of_statuses[], size_t size )
		{
			//assert( size >= _statuses.size() );
			auto begin = _statuses.begin();
			auto end = _statuses.end();
			std::copy( begin, end, array_of_statuses );
		}
};

template <size_t length>
class StatusSet<MPI_Fint, StatusKind::attend, length>
{
	private:
		std::array<status<MPI_Fint,StatusKind::attend>,length> _statuses;

	public:
		StatusSet() :
			_statuses()
		{
		}

		StatusSet( StatusSet const& other ) :
			_statuses()
		{
			std::copy( other._statuses.begin(), other._statuses.end(), _statuses.begin() );
		}

		StatusSet( MPI_Fint const* statuses, size_t size ) :
			_statuses()
		{
			//assert( statuses != MPI_F_STATUSES_IGNORE )
			auto begin = reinterpret_cast<const fortran_status*>(statuses);
			auto end = begin + size;
			std::copy( begin, end, _statuses.begin() );
		}

		StatusSet& operator=( StatusSet const& other )
		{
			std::copy( other._statuses.begin(), other._statuses.end(), _statuses.begin() );
		}

		operator MPI_Fint* ()
		{
			return _statuses.data();
		}

		void copy( MPI_Fint *status )
		{
			auto begin = _statuses.begin();
			auto end = _statuses.begin() +1;
			std::copy( begin, end, reinterpret_cast<fortran_status*>(status) );
		}

		void copy( MPI_Fint array_of_statuses[], size_t size )
		{
			auto begin = _statuses.begin();
			auto end = _statuses.end();
			std::copy( begin, end, reinterpret_cast<fortran_status*>(array_of_statuses) );
		}
};

template <>
class StatusSet<MPI_Fint, StatusKind::attend, 0>
{
	private:
		std::vector<status<MPI_Fint,StatusKind::attend> > _statuses;

	public:
		StatusSet() :
			_statuses()
		{
		}

		StatusSet( StatusSet const& other ) :
			_statuses( other._statuses )
		{
		}

		StatusSet( MPI_Fint const* array_of_statuses, size_t size ) :
			_statuses()
		{
			//assert( statuses != MPI_F_STATUSES_IGNORE )
			_statuses.reserve(size);
			auto begin = reinterpret_cast<const fortran_statuses_array &>(*array_of_statuses);
			auto end = begin + size;
			std::copy( begin, end, _statuses.begin() );
		}

		operator fortran_status* ()
		{
			_statuses.data();
		}

		void copy( MPI_Fint *status )
		{
			auto begin = _statuses.begin();
			auto end = _statuses.begin() +1;
			std::copy( begin, end, reinterpret_cast<fortran_status*>(status) );
		}

		void copy( MPI_Fint array_of_statuses[], size_t size )
		{
			auto begin = _statuses.begin();
			auto end = _statuses.end();
			std::copy( begin, end, reinterpret_cast<fortran_status*>(array_of_statuses) );
		}
};

template < size_t length >
class StatusSet<MPI_Status, StatusKind::ignore, length>
{
	public:
		StatusSet() = default;

		StatusSet( MPI_Status array_of_statuses[], size_t size )
		{
			//assert( value = MPI_STATUSES_IGNORE );
		}

		operator MPI_Status* ()
		{
			return MPI_STATUSES_IGNORE;
		}

		void copy( MPI_Status *status )
		{
		}

		void copy( MPI_Status array_of_statuses[], size_t size )
		{
		}
};

template < size_t length >
class StatusSet<MPI_Fint, StatusKind::ignore, length> {
	public:
		StatusSet() = default;

		StatusSet( MPI_Fint *value, size_t size )
		{
			//assert( vaule == MPI_F_STATUSES_IGNORE );
		}

		operator MPI_Status* ()
		{
			return MPI_F_STATUSES_IGNORE;
		}

		void copy( MPI_Fint *status )
		{
		}

		void copy( MPI_Fint array_of_statuses[], size_t size )
		{
		}
};

} // namespace mpi
} // namespace nanos

#endif // STATUS_SET_H


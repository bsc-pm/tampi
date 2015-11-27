
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
			auto begin = statuses;
			auto end = statuses + (SIZEOF_MPI_STATUS*size);
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
};

template <>
class StatusSet<MPI_Fint, StatusKind::attend, 0>
{
	private:
		std::vector<status<MPI_Fint> > _statuses;

	public:
		StatusSet() :
			_statuses()
		{
		}

		StatusSet( StatusSet const& other ) :
			_statuses( other._statuses )
		{
		}

		StatusSet( MPI_Fint const* statuses, size_t size ) :
			_statuses()
		{
			//assert( statuses != MPI_F_STATUSES_IGNORE )
			_statuses.reserve(size);
			auto begin = statuses;
			auto end = statuses + ( size * SIZEOF_MPI_STATUS );
			std::copy( begin, end, _statuses.begin() );
		}

		operator MPI_Fint* ()
		{
			_statuses.data();
		}
};

template < size_t length >
class StatusSet<MPI_Status, StatusKind::ignore, length>
{
	public:
		operator MPI_Status* ()
		{
			return MPI_STATUSES_IGNORE;
		}
};

template < size_t length >
class StatusSet<MPI_Fint, StatusKind::ignore, length> {
	public:
		operator MPI_Status* ()
		{
			return MPI_F_STATUSES_IGNORE;
		}
};

template < typename UnderlyingType >
class StatusSet<UnderlyingType, StatusKind::attend, 1> : status<UnderlyingType,StatusKind::attend>
{
};

} // namespace mpi
} // namespace nanos

#endif // STATUS_SET_H



#ifndef MPI_STATUS_H
#define MPI_STATUS_H

#include "common.h"
#include "status_fwd.h"

#include <mpi.h>
#include <algorithm>
#include <array>

namespace nanos {
namespace mpi {

template <>
class status<MPI_Status, StatusKind::attend>
{
	private:
		MPI_Status _value;
	
	public:
		status() :
			_value()
		{
		}
	
		status( MPI_Status const& value ) :
			_value(value)
		{
		}
	
		status( status const& other ) :
			_value(other._value)
		{
		}

		status& operator=( status const& other )
		{
			_value = other._value;
			return *this;
		}
	
		operator MPI_Status ()
		{
			return _value;
		}

		operator MPI_Status* ()
		{
			return reinterpret_cast<MPI_Status*>( &_value );
		}
};

template <>
class status<MPI_Fint, StatusKind::attend>
{
	private:
		fortran_status _value;

	public:
		status() :
			_value()
		{
		}

		status( MPI_Fint const* value ) :
			_value( reinterpret_cast<fortran_status const&>(*value) )
		{
			//assert( value != MPI_F_STATUS_IGNORE );
		}

		status( status const& other ) :
			_value( other._value )
		{
		}

		status& operator=( status const& other )
		{
			auto begin = other._value.begin();
			auto end = other._value.end();
			std::copy( begin, end, _value.begin() );
			return *this;
		}

		status& operator=( fortran_status const& other )
		{
			_value = other;
			return *this;
		}

		operator MPI_Fint* ()
		{
			return _value.data();
		}

		operator fortran_status& ()
		{
			return _value;
		}

		friend auto std::copy<>( status *first, status *last, nanos::mpi::fortran_status *d_first ) -> decltype(d_first);
};

template <>
class status<MPI_Status, StatusKind::ignore>
{
	public:
		operator MPI_Status* ()
		{
			return MPI_STATUS_IGNORE;
		}
};

template <>
class status<MPI_Fint, StatusKind::ignore>
{
		operator MPI_Fint* ()
		{
			return MPI_F_STATUS_IGNORE;
		}
};

template < StatusKind kind >
struct is_ignore : public std::false_type
{
};

template <>
struct is_ignore<StatusKind::ignore> : public std::true_type
{
};

} // namespace mpi
} // namespace nanos

namespace std {
		template <>
		inline nanos::mpi::fortran_status* copy( nanos::mpi::status<MPI_Fint,nanos::mpi::StatusKind::attend> *first, nanos::mpi::status<MPI_Fint,nanos::mpi::StatusKind::attend> *last, nanos::mpi::fortran_status *d_first )
		{
			while( first != last )
			{
				d_first = reinterpret_cast<nanos::mpi::fortran_status*>(
										std::copy( first->_value.begin(), first->_value.end(), d_first->begin() )
								);
				first++;
			}
			return d_first;
		}
}

#endif // MPI_STATUS_H


#ifndef MPI_STATUS_H
#define MPI_STATUS_H

#include <mpi.h>
#include <algorithm>

#define SIZEOF_MPI_STATUS sizeof(MPI_Status)/sizeof(MPI_Fint)

namespace nanos {
namespace mpi {

enum class StatusKind
{
	ignore,
	attend
};

template < typename UnderlyingType, StatusKind kind = StatusKind::attend >
class status;

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
	
		status( MPI_Status const* value ) :
			_value(*value)
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
		MPI_Fint _value[SIZEOF_MPI_STATUS];

	public:
		status() :
			_value()
		{
		}

		status( MPI_Fint const* value ) :
			_value()
		{
			//assert( value != MPI_F_STATUS_IGNORE );
			auto begin = value;
			auto end = begin+SIZEOF_MPI_STATUS;
			std::copy( begin, end, _value );
		}

		status( status const& other ) :
			_value()
		{
			auto begin = other._value;
			auto end = begin+SIZEOF_MPI_STATUS;
			std::copy( begin, end, _value );
		}

		status& operator=( status const& other )
		{
			auto begin = other._value;
			auto end = begin+SIZEOF_MPI_STATUS;
			std::copy( begin, end, _value );
			return *this;
		}

		operator MPI_Fint* ()
		{
			return _value;
		}
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

} // namespace mpi
} // namespace nanos

#endif // MPI_STATUS_H

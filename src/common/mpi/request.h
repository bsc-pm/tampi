
#ifndef REQUEST_H
#define REQUEST_H

#include <mpi.h>

#include "status.h"

extern "C" {
//! MPI_Test Fortran API declaration
void mpi_test_( MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint* );

//! MIP_Testall Fortran API declaration
void mpi_testall_( MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint[][SIZEOF_MPI_STATUS], MPI_Fint* );
}

namespace nanos {
namespace mpi {

template < typename UnderlyingType >
class request;

template <>
class request<MPI_Request>
{
private:
	MPI_Request _value;

public:
	request() = default;

	request( MPI_Request const& value ) :
		_value( value )
	{
	}

	request( request const& other ) = default;

	request& operator=( request const& other ) = default;

	request& operator=( MPI_Request other )
	{
		_value = other;
	}

	bool test()
	{
		using status_t = status<MPI_Status,StatusKind::ignore>;
		int flag;
		MPI_Test(
				&_value, 
				&flag, 
				static_cast<MPI_Status*>( status_t() )
			);
		return flag == 1;
	}

	template < StatusKind kind >
	bool test( status<MPI_Status,kind> &st )
	{
		int flag;
		MPI_Test( &_value, &flag, static_cast<MPI_Status*>(st) );
		return flag == 1;
	}

	operator MPI_Request ()
	{
		return _value;
	}

	operator MPI_Request* ()
	{
		return &_value;
	}

	MPI_Request* data()
	{
		return &_value;
	}
};

/*
template <>
class request<MPI_Fint>
{
	private:
		MPI_Fint _value;
	public:
		request() :
			_value()
		{
		}

		request( MPI_Fint v )
			_value( v )
		{
		}
	
		request( request const& other ) :
			_value( other._value )
		{
		}

		bool test()
		{
			int flag;
			mpi_test_( &_value, &flag, status<MPI_Status,ignore>().data() );
			return flag == 1;
		}

		template< StatusKind kind >
		bool test( status<MPI_Fint,kind> &status )
		{
			int flag;
			mpi_test_( &_value, &flag, status.data() );
			return flag == 1;
		}

		operator MPI_Fint ()
		{
			return _value;
		}

		operator MPI_Fint* ()
		{
			return &_value;
		}
};
*/

} // namespace mpi
} // namespace nanos

#endif // REQUEST_H

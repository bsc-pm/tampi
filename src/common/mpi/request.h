
#ifndef REQUEST_H
#define REQUEST_H

#include <mpi.h>

#include "error.h"
#include "status.h"

#include <vector>

namespace nanos {
namespace mpi {

namespace C {

class request
{
	private:
		MPI_Request _value;
	
	public:
		using value_type = MPI_Request;
	
		request() :
			_value( MPI_REQUEST_NULL )
		{
		}
	
		request( MPI_Request value ) :
			_value( value )
		{
		}
	
		request( request const& other ) = default;
	
		request& operator=( request const& other ) = default;
	
		request& operator=( value_type other )
		{
			_value = other;
			return *this;
		}
	
		value_type* data()
		{
			return &_value;
		}
	
		operator value_type& ()
		{
			return _value;
		}

		operator value_type* ()
		{
			return &_value;
		}

		bool test();
};

}// namespace C

namespace Fortran {

class request
{
	private:
		MPI_Fint _value;
	public:
		using value_type = MPI_Fint;

		request() :
			_value( MPI_Request_c2f(MPI_REQUEST_NULL) )
		{
		}

		request( MPI_Fint v ) :
			_value( v )
		{
		}
	
		request( request const& other ) :
			_value( other._value )
		{
		}

		value_type* data()
		{
			return &_value;
		}

		operator value_type& ()
		{
			return _value;
		}

		operator value_type* ()
		{
			return &_value;
		}

		bool test();
};

} // namespace Fortran


// MPI Test fortran specializations
extern "C" {

//! MPI_Test Fortran API declaration
void pmpi_test_( MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint* );

//! MPI_Testall Fortran API declaration
void pmpi_testall_( MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint[], MPI_Fint* );

//! MPI_Testany Fortran API declaration
void pmpi_testany_( MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint* );

//! MPI_Testsome Fortran API declaration
void pmpi_testsome_( MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint[], MPI_Fint[], MPI_Fint* );

} // extern C

} // namespace mpi
} // namespace nanos

#endif // REQUEST_H

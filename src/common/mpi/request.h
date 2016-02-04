
#ifndef REQUEST_H
#define REQUEST_H

#include <mpi.h>

#include "status.h"
#include "test_decl.h"

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
	
		bool test()
		{
			return test_impl( *this );
		}
	
		template < StatusKind kind >
		bool test( status<kind> &st )
		{
			return test_impl( *this, st );
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

		bool test()
		{
			return test_impl( *this );
		}

		template< StatusKind kind >
		bool test( status<kind> &st )
		{
			return test_impl( *this, st );
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
};

} // namespace Fortran

namespace C {
	inline bool test_impl( request &req )
	{
		int flag;
		MPI_Test(
					req.data(),
					&flag,
					MPI_STATUS_IGNORE
				);
		return flag;
	}

	template < StatusKind kind >
	inline bool test_impl( request &req, status<kind> &st )
	{
		int flag;
		MPI_Test(
					req.data(),
					&flag,
					static_cast<typename status<kind>::value_type*>(st)
				);
		return flag;
	}
} // namespace C

namespace Fortran {
	inline bool test_impl( request &req )
	{
		MPI_Fint flag, error;
		mpi_test_(
					req.data(),
					&flag,
					MPI_F_STATUS_IGNORE,
					&error
				);
		return flag == 1;
	}

	template < StatusKind kind >
	inline bool test_impl( request &req, status<kind> &st )
	{
		MPI_Fint flag, error;
		mpi_test_(
					req.data(),
					&flag,
					static_cast<typename status<kind>::value_type*>(st),
					&error
				);
		return flag == 1;
	}
} // namespace Fortran

} // namespace mpi
} // namespace nanos

#endif // REQUEST_H

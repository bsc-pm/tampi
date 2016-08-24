
#ifndef REQUEST_H
#define REQUEST_H

#include <mpi.h>

#include "status.h"

extern "C" {

//! MPI_Test Fortran API declaration
void mpi_test_( MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint* );

//! MPI_Testall Fortran API declaration
void mpi_testall_( MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint[], MPI_Fint* );

//! MPI_Testany Fortran API declaration
void mpi_testany_( MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint*, MPI_Fint*, MPI_Fint* );

//! MPI_Testsome Fortran API declaration
void mpi_testsome_( MPI_Fint*, MPI_Fint[], MPI_Fint*, MPI_Fint[], MPI_Fint[], MPI_Fint* );

} // extern C

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
	
		template < StatusKind kind = StatusKind::ignore >
		bool test( status<kind> &return_status = status<kind>() )
		{
			int flag;
			MPI_Test( _value, &flag,
			          &static_cast<MPI_Status&>(return_status) );
			return flag == 1;
		}

		template < StatusKind kind = StatusKind::ignore >
		static std::pair<bool,int> test_any( std::vector<request>& requests,
		                      status<kind>& return_status = status<kind>() )
		{
			int flag, index;
			MPI_Testany( requests.size(),
			             reinterpret_cast<MPI_Request*>(requests.data()),
			             &index, &flag,
			             &static_cast<MPI_Status&>(return_status) );
			return { flag == 1 && index != MPI_UNDEFINED,
			         index };
		}

		static bool test_all( std::vector<request>& requests,
		                      std::vector<status<StatusKind::attend> >& return_statuses )
		{
			int flag;
			return_statuses.resize( requests.size() );
			MPI_Testall( requests.size(),
			             reinterpret_cast<MPI_Request*>(requests.data()), &flag,
			             reinterpret_cast<MPI_Status*>(return_statuses.data()) );
			return flag == 1;
		}

		static std::vector<int> test_some( std::vector<request>& requests,
		                      std::vector<status<StatusKind::attend> >& return_statuses )
		{
			int ready_count = 0;
			std::vector<int> ready_indices(requests.size());

			MPI_Testsome( requests.size(),
			              reinterpret_cast<MPI_Request*>(requests.data()),
			              &ready_count, ready_indices.data(),
			              reinterpret_cast<MPI_Status*>(return_statuses.data()) );
			if( ready_count == MPI_UNDEFINED ) {
				ready_indices.clear();
			} else {
				ready_indices.resize( ready_count );
			}
			return ready_indices;
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
	
		template < StatusKind kind = StatusKind::ignore >
		bool test( status<kind> &return_status = status<kind>() )
		{
			int err, flag;
			mpi_test_( &_value, &flag, 
			           &static_cast<MPI_Fint&>(return_status),
			           &err );
			return flag == 1;
		}

		template < StatusKind kind = StatusKind::ignore >
		static std::pair<bool,int> test_any( std::vector<request>& requests,
		                      status<kind>& return_status = status<kind>() )
		{
			int err, flag, index;
			mpi_testany_( requests.size(),
			             reinterpret_cast<MPI_Fint*>(requests.data()),
			             &index, &flag,
			             &static_cast<MPI_Fint&>(return_status),
			             &err );
			return { flag == 1 && index != MPI_UNDEFINED,
			         index };
		}

		static bool test_all( std::vector<request>& requests,
		                      std::vector<status<StatusKind::attend> >& return_statuses )
		{
			int err, flag;
			int count = requests.size();

			return_statuses.resize( count );
			mpi_testall_( &count,
			             reinterpret_cast<MPI_Fint*>(requests.data()),
			             &flag,
			             reinterpret_cast<MPI_Fint*>(return_statuses.data()),
			             &err );
			return flag == 1;
		}

		static std::vector<int> test_some( std::vector<request>& requests,
		                      std::vector<status<StatusKind::attend> >& return_statuses )
		{
			int err, ready_count;
			int count = requests.size();
			std::vector<int> ready_indices(requests.size());

			mpi_testsome_( &count,
			               reinterpret_cast<MPI_Fint*>(requests.data()),
			               &ready_count, ready_indices.data(),
			               reinterpret_cast<MPI_Fint*>(return_statuses.data()),
			               &err );
			if( ready_count == MPI_UNDEFINED ) {
				ready_indices.clear();
			} else {
				ready_indices.resize( ready_count );
			}
			return ready_indices;
		}
};

} // namespace Fortran

} // namespace mpi
} // namespace nanos

#endif // REQUEST_H

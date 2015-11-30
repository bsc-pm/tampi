
#ifndef REQUEST_SET_H
#define REQUEST_SET_H

#include "request.h"
#include "statusset.h"

#include <mpi.h>

#include <array>
#include <vector>

namespace nanos {
namespace mpi {

template < typename RequestType, size_t length = 0 >
class RequestSet {
	private:
		std::array<request<RequestType>,length> _requests;

	public:
		RequestSet() :
			_requests()
		{
		}

		RequestSet( RequestType *requests, size_t size ) :
			_requests()
		{
			auto begin = requests;
			auto end = requests + size;
			std::copy( begin, end, _requests.begin() );
		}

		template < class StatusType, StatusKind kind >
		bool test( StatusSet<StatusType,kind,length> &statuses );

		size_t size() const
		{
			return _requests.size();
		}

		operator RequestType* ()
		{
			return reinterpret_cast<RequestType*>(_requests.data());
		}

		auto at( size_t pos ) const -> typename std::add_lvalue_reference<decltype(_requests.at(pos))>::type
		{
			return _requests.at(pos);
		}

		auto at( size_t pos ) -> typename std::add_lvalue_reference<decltype(_requests.at(pos))>::type
		{
			return _requests.at(pos);
		}
};

template < typename RequestType >
class RequestSet<RequestType, 0> {
	private:
		std::vector<request<RequestType> > _requests;

	public:
		RequestSet() :
			_requests()
		{
		}

		RequestSet( RequestType *requests, size_t size ) :
			_requests()
		{
			_requests.reserve(size);
			auto begin = reinterpret_cast<request<RequestType>*>( requests );
			std::copy( begin, begin+size, _requests.begin() );
		}

		template < typename StatusType, StatusKind kind >
		bool test( StatusSet<StatusType,kind,0> &statuses )
		{
			return testall_impl( *this, statuses );
		}

		size_t size() const
		{
			return _requests.size();
		}

		operator RequestType* ()
		{
			return reinterpret_cast<RequestType*>(_requests.data());
		}

		auto at( size_t pos ) const -> typename std::add_lvalue_reference<decltype(_requests.at(pos))>::type
		{
			return _requests.at(pos);
		}

		auto at( size_t pos ) -> typename std::add_lvalue_reference<decltype(_requests.at(pos))>::type
		{
			return _requests.at(pos);
		}
};

template < class RequestType, class StatusType, StatusKind kind, size_t length >
bool testall_impl( RequestSet<RequestType,length> &requests, StatusSet<StatusType,kind,length> &statuses );

template < size_t length, StatusKind kind >
bool testall_impl( RequestSet<MPI_Request,length> &requests, StatusSet<MPI_Status,kind,length> &statuses )
{
	int flag;
	MPI_Testall( requests.size(), static_cast<MPI_Request*>(requests), &flag, static_cast<MPI_Status*>(statuses) );
	return flag == 1;
}

template < size_t length >
bool testall_impl( RequestSet<MPI_Fint,length> &requests, StatusSet<MPI_Fint,StatusKind::attend,length> &statuses )
{
	int flag, error;
   MPI_Fint size = requests.size();
	auto statuses_array = static_cast<fortran_status*>(statuses);
	mpi_testall_( &size, static_cast<MPI_Fint*>(requests), &flag, reinterpret_cast<MPI_Fint(*)[SIZEOF_MPI_STATUS]>(statuses_array), &error );
	return flag == 1;
}

} // namespace mpi
} // namespace nanos

#endif // REQUEST_SET_H


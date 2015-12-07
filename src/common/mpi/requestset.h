
#ifndef REQUEST_SET_H
#define REQUEST_SET_H

#include "request.h"
#include "statusset.h"
#include "test_decl.h"

#include <mpi.h>

#include <array>
#include <vector>

namespace nanos {
namespace mpi {

template < typename Request, size_t length >
class RequestSet {
	private:
		std::array<Request,length> _requests;

	public:
		RequestSet() :
			_requests()
		{
		}

		RequestSet( Request *requests, size_t size ) :
			_requests()
		{
			auto begin = requests;
			auto end = requests + size;
			std::copy( begin, end, _requests.begin() );
		}

		template < class Status >
		bool test( StatusSetBase<Status,length> &statuses )
		{
			return testall_impl( *this, statuses );
		}

		size_t size() const
		{
			return _requests.size();
		}

		operator typename Request::value_type* ()
		{
			return reinterpret_cast<typename Request::value_type*>(_requests.data());
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

template < typename Request >
class RequestSet<Request, 0> {
	private:
		std::vector<Request> _requests;

	public:
		RequestSet() :
			_requests()
		{
		}

		RequestSet( RequestSet const& other ) = default;

		RequestSet( typename Request::value_type *requests, size_t size ) :
			_requests()
		{
			_requests.reserve(size);
			std::copy( requests, requests+size, _requests.begin() );
		}

		template < typename Status >
		bool test( StatusSetBase<Status,0> &statuses )
		{
			return testall_impl( *this, statuses );
		}

		size_t size() const
		{
			return _requests.size();
		}

		operator typename Request::value_type* ()
		{
			return reinterpret_cast<typename Request::value_type*>(_requests.data());
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

template < size_t length, StatusKind kind >
bool testall_impl( RequestSet<C::request,length> &requests, StatusSetBase<C::status<kind>,length> &statuses )
{
	int flag;
	MPI_Testall(
					requests.size(),
					static_cast<C::request::value_type*>(requests),
					&flag,
					static_cast<typename C::status<kind>::value_type*>(statuses)
				);
	return flag == 1;
}

template < size_t length >
bool testall_impl( RequestSet<C::request,length> &requests )
{
	int flag;
	MPI_Testall(
					requests.size(),
					static_cast<C::request::value_type*>(requests),
					&flag,
					MPI_STATUSES_IGNORE
				);
	return flag == 1;
}

template < size_t length, StatusKind kind >
bool testall_impl( RequestSet<Fortran::request,length> &requests, StatusSetBase<Fortran::status<kind>,length> &statuses )
{
	MPI_Fint flag, error;
   MPI_Fint size = requests.size();

	mpi_testall_(
				&size,
				requests.data(),
				&flag,
				static_cast<typename Fortran::status<kind>::value_type*>(statuses),
				&error
			);

	return flag == 1;
}

template < size_t length >
bool testall_impl( RequestSet<Fortran::request,length> &requests )
{
	MPI_Fint flag, error;
   MPI_Fint size = requests.size();

	mpi_testall_(
				&size,
				requests.data(),
				&flag,
				MPI_F_STATUSES_IGNORE,
				&error
			);

	return flag == 1;
}

} // namespace mpi
} // namespace nanos

#endif // REQUEST_SET_H


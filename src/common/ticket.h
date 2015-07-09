#ifndef TICKET_H
#define TICKET_H

#include <mpi.h>
#include <iterator>
#include <utility>

#include <cassert>

#include "flag.h"
#include "pollingchecker.h"
#include "mpi_impl.h"
#include "status.h"

namespace nanos {
namespace mpi {

template < typename Checker >
class Ticket;

template <
    typename RequestType, 
    typename StatusType, 
    typename IntType,
    size_t count = 0 // Fixed number of items. 0 : use dynamic container
>
class TicketChecker : public PollingChecker {
    typedef RequestType request_type;
    typedef StatusType  status_type;
    typedef IntType     int_type;

protected:
    Requests<RequestType, count> _requests;
    Statuses<StatusType,  count> _statuses;
    IntType _error;
public:
    TicketChecker() : _requests(), _statuses(), _error() {}

    TicketChecker( IntType len, RequestType r[] ) : _requests( static_cast<size_t>(len) ), _statuses( static_cast<size_t>(len) ), _error()
    {
        assert( len <= _requests.capacity() );
        std::copy( r, r+len, _requests.begin() );
    }

    TicketChecker( TicketChecker const& t ) : _requests( t._requests ), _statuses( t._statuses ), _error( t._error ) {}

    virtual ~TicketChecker() {}

    virtual bool test() {
        return test_impl<count>::test( _requests.size(), _requests.data(), _statuses.data(), &_error );
    }

    IntType&       getError()       { return _error; }
    IntType const& getError() const { return _error; }

    template < int position >
    RequestType &getRequest() { return _requests[position]; }

    Requests<RequestType,count> &getRequests() { return _requests; }

    template < int position >
    StatusType &getStatus() { return _statuses[position]; }

    Statuses<StatusType, count> &getStatuses() { return _statuses; }
};

template <class TicketChecker>
class Ticket : public SinglePollingCond<TicketChecker> {
public:
    Ticket() : SinglePollingCond< TicketChecker >() {}

    Ticket( Ticket const& t ) : SinglePollingCond< TicketChecker > (t) {}

    Ticket( TicketChecker const& tc ) : SinglePollingCond< TicketChecker > (tc) {}

    virtual ~Ticket() {}

    TicketChecker &getData() { return this->_conditionChecker; }

    template < typename int_type >
    void wait( int_type *err ) {
        if( MPI_SUCCESS == getData().getError() )
            this->waitForPollCompletion();
        if( err )
            *err = getData().getError();
    }

    template < typename status_type, typename int_type >
    void wait( status_type *status, int_type *err ) {
        wait( err );
        getData().getStatuses().copy( status );
    }

    template < typename status_type, typename int_type >
    void wait( int_type nelem, status_type array_of_statuses/*[]*/, int_type *err ) {
        wait( err );
        //getData().getStatuses().copy( static_cast<size_t>(nelem), array_of_statuses );
    }
};

namespace C {

typedef MPI_Request request_type;
typedef MPI_Status  status_type;
typedef int         int_type;

// Ticket type 
// Count: fixed number of requests
// Count=0 (default) variable (dynamic) number of requests
template<size_t count=0>
struct Ticket {
  typedef TicketChecker<request_type, status_type, int_type, count> checker_type;
  typedef nanos::mpi::Ticket< checker_type > type;
};

} // namespace C

namespace Fortran {

typedef MPI_Fint request_type;
typedef MPI_Fint status_type[SIZEOF_MPI_STATUS];
typedef MPI_Fint int_type;

// Ticket type 
// Count: fixed number of requests
// Count=0 (default) variable (dynamic) number of requests
template<size_t count=0>
struct Ticket {
  typedef TicketChecker<request_type, status_type, int_type, count> checker_type;
  typedef nanos::mpi::Ticket< checker_type > type;
};

} // namespace Fortran

} // namespace mpi
} // namespace nanos

#endif // TICKET_H

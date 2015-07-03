#ifndef TICKET_H
#define TICKET_H

#include <mpi.h>
#include <array>
#include <vector>
#include <iterator>
#include <utility>

#include <cassert>

#include "flag.h"
#include "pollingchecker.h"
#include "test_impl.h"

namespace nanos {
namespace mpi {

template < typename Checker >
class Ticket;

template <
    typename RequestType, 
    typename StatusType, 
    typename IntType,
    int count = -1 // Fixed number of items. -1 : use dynamic container
>
class TicketChecker : public PollingChecker {
    typedef RequestType request_type;
    typedef StatusType  status_type;
    typedef IntType     int_type;
    friend class Ticket< TicketChecker<RequestType,StatusType,IntType,count> >;

    std::array<RequestType, count> _requests;
    std::array<StatusType,  count> _statuses;
    IntType _error;
public:
    TicketChecker() : _requests(), _statuses(), _error() {}

    TicketChecker( IntType len, RequestType r[] ) : _requests(), _statuses(), _error()
    {
        assert( len <= _requests.size() );
        std::copy( r, r+len, _requests.begin() );
    }

    TicketChecker( TicketChecker const& t ) : _requests( t._requests ), _statuses( t._statuses ), _error( t._error ) {}

    virtual ~TicketChecker() {}

    virtual bool test() {
            return test_impl<count>::test( _requests.size(), _requests.data(), _statuses.data(), _error );
    }

    IntType const& getError()     const { return error; }

    template < int position >
    RequestType &getRequest()
    {
        return _requests[position];
    }
};

template <
    typename RequestType, 
    typename StatusType, 
    typename IntType
    //-1 : Dont care about number of items. Will use dynamic storage
>
class TicketChecker<RequestType,StatusType,IntType,-1> : public PollingChecker {
    typedef RequestType request_type;
    typedef StatusType  status_type;
    typedef IntType     int_type;
    friend class Ticket< TicketChecker<RequestType,StatusType,IntType,-1> >;

    std::vector<RequestType> _requests;
    std::vector<StatusType>  _statuses;
    IntType _error;
public:
    TicketChecker() : _requests(), _statuses(), _error() {}

    TicketChecker( IntType len, RequestType *r )
        : _requests( r, r+len ),
          _statuses(len), _error()
    {
        assert( len <= _requests.size() );
    }

    TicketChecker( TicketChecker const& t ) : _requests( t._requests ), _statuses( t._statuses ), _error( t._error ) {}

    TicketChecker( TicketChecker &&t ) :
        _requests( std::forward(t._requests) ), 
        _statuses( std::forward(t._statuses) ), 
        _error( std::forward(t._error) )
    {}

    virtual ~TicketChecker() {}

    virtual bool test() {
        return test_impl<-1>::test( _requests.size(), _requests.data(), _statuses.data(), _error );
    }

    IntType const& getError() const { return _error; }

    template < int position >
    RequestType &getRequest()
    {
        return _requests[position];
    }
};


template <class TicketChecker>
class Ticket : public SinglePollingCond<TicketChecker> {
    typedef typename TicketChecker::int_type     int_type;
    typedef typename TicketChecker::request_type request_type;
    typedef typename TicketChecker::status_type  status_type;

public:
    Ticket() : SinglePollingCond< TicketChecker >() {}

    Ticket( TicketChecker const& t ) : SinglePollingCond< TicketChecker > (t) {}

    //Ticket( TicketChecker &&t ) : SinglePollingCond< TicketChecker > ( std::forward(t) ) {}

    virtual ~Ticket() {}

    TicketChecker &getData() { return this->_conditionChecker; }

    void wait( int_type *err ) {
        if( MPI_SUCCESS == getData()._error )
            this->waitForPollCompletion();

        if( err )
            *err = getData()._error;
    }

    void wait( status_type *status, int_type *err ) {
        wait( err );
        if( status ) {
            // Copy only the last element
            std::copy( getData()._statuses.end()-1, getData()._statuses.end(), status );
        }
    }

    void wait( int_type nelem,status_type array_of_statuses[], int_type *err ) {
        assert( nelem < _statuses.size() );
        wait( err );
        if( statuses ) {
            std::copy( getData()._statuses.begin(),
                       getData()._statuses.end(),
                       std::begin(array_of_statuses)
                      );
        }
    }
};

namespace C {
// Ticket type 
// Count: fixed number of requests
// Count=-1 (default) variable (dynamic) number of requests
template<int count=-1>
struct Ticket {
  typedef nanos::mpi::Ticket< TicketChecker<MPI_Request,MPI_Status,int,count> > type;
};


} // namespace C

namespace Fortran {
// Ticket type 
// Count: fixed number of requests
// Count=-1 (default) variable (dynamic) number of requests
template<int count=-1>
struct Ticket {
  typedef nanos::mpi::Ticket< TicketChecker<MPI_Fint,MPI_Fint,MPI_Fint,count> > type;
};

} // namespace Fortran

} // namespace mpi
} // namespace nanos

#endif // TICKET_H

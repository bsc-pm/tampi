/*
 * Copyright (c) 2015 Barcelona Supercomputing Center. All rights
 *                    reserved.
 *
 * This file is part of OmpSs-MPI interoperability library.
 *
 * OmpSs-MPI interop. library is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef TICKET_H
#define TICKET_H

#include <mpi.h>
#include <iterator>
#include <utility>
#include <cassert>

#include <nanox/nanos.h>

#include "flag.h"
#include "pollingchecker.h"
#include "mpicommon.h"
#include "mpi_impl.h"
#include "status.h"

namespace nanos {
namespace mpi {

template < typename Checker >
class Ticket;

//! Polling ConditionChecker for MPI requests.
/*!
  TicketChecker contains the necessary data to perform tests over
  one or many MPI requests and store the status output value.
  \param RequestType Data type used to represent a request
  \param StatusType Data type used to represent a status
  \param IntType Data type used to represent integer values (lengths, error codes...)
  \param count Number of request that this ticket will contain. If 0 or not specified, the number of requests is not fixed (dynamic storage is used in this case).
 */
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
    //! Contains all the requests that must be checked.
    Requests<RequestType, count> _requests;

    //! Contain the result of the checks for each request.-
    Statuses<StatusType,  count> _statuses;

    //! Holds the return value of the last check.
    IntType _error;
public:
    //! Default constructor. 
    /*!
      Creates an empty ticket with uninitialized requests.
      Requests may be initialized later on.
     */
    TicketChecker() : _requests(), _statuses(), _error() {}

    //! Request array constructor
    /*!
      Creates a ticket that will check for the completion of
      a given set of requests.
      \warning The request capacity of the ticket must be enough to hold
      all of them. Using fixed size is not recommended in this case
      for this reason.
     */
    TicketChecker( IntType len, RequestType r[] ) : _requests( static_cast<size_t>(len) ), _statuses( static_cast<size_t>(len) ), _error()
    {
        assert( len <= _requests.capacity() );
        std::copy( r, r+len, _requests.begin() );
    }

    //! Copy constructor.
    TicketChecker( TicketChecker const& t ) : _requests( t._requests ), _statuses( t._statuses ), _error( t._error ) {}

    //! Destructor
    virtual ~TicketChecker() {}

    //! Calls MPI test.
    /*!
      Calls the specific mpi_test implementation depending on the data types 
      that are being used.
     */
    virtual bool test() {
        return test_impl<count>::test( _requests.size(), _requests.data(), _statuses.data(), &_error );
    }

    //! \return last check's error code. Non const-reference version.
    IntType&       getError()       { return _error; }

    //! \return last check's error code. Non const-reference version.
    IntType const& getError() const { return _error; }

    //! Sets lasts check's error code.
    void setError( IntType const& value ) { _error = value; }

    //! \return the ticket's Nth request.
    template < int position >
    RequestType &getRequest() { return _requests[position]; }

    //! \return a reference to the request container.
    Requests<RequestType,count> &getRequests() { return _requests; }

    //! \return the ticket's Nth requests status.
    template < int position >
    StatusType &getStatus() { return _statuses[position]; }

    //! \return a reference to the status container.
    Statuses<StatusType, count> &getStatuses() { return _statuses; }
};

//! Polling conditional variable for MPI conditional checkers
/*!
  Ticket is an extension of the SinglePollingCond that is used in combination
  with TicketChecker in order to test the compleness of MPI requests.
  \param TicketChecker MPI checker that will be used.
 */
template <class TicketChecker>
class Ticket : public SinglePollingCond<TicketChecker> {
public:
    typedef TicketChecker checker_type;

    //! Default constructor.
    Ticket() : SinglePollingCond< TicketChecker >() {}

    //! Copy constructor.
    Ticket( Ticket const& t ) : SinglePollingCond< TicketChecker > (t) {}

    //! ConditionChecker constructor.
    /*!
      Passes a ConditionChecker's reference to its superclass.
     */
    Ticket( TicketChecker const& tc ) : SinglePollingCond< TicketChecker > (tc) {}

    //! Destructor.
    virtual ~Ticket() {}

    //! Returns a reference to MPI request values.
    TicketChecker &getData() { return this->_conditionChecker; }

    //! Stops the task until the requests are completed.
    /*!
      \param err output error code.
     */
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

template < typename comm_type, size_t count >
struct TicketTraits {
	typedef typename MPITraits<comm_type>::request_type request_type;
	typedef typename MPITraits<comm_type>::status_type status_type;
	typedef typename MPITraits<comm_type>::int_type int_type;

	typedef Ticket< TicketChecker<request_type, status_type, int_type, count> > ticket_type;
};

namespace C {
  template< size_t count = 0 >
  struct TicketTraits {
    typedef typename nanos::mpi::Ticket< TicketChecker<request_type,status_type,int_type,count> > ticket_type;
  };
} // namespace C

namespace Fortran {
  template< size_t count = 0 >
  struct TicketTraits {
    typedef typename nanos::mpi::Ticket< TicketChecker<request_type,status_type,int_type,count> > ticket_type;
  };
} // namespace Fortran

} // namespace mpi
} // namespace nanos

#endif // TICKET_H

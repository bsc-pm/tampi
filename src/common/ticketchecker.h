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
#ifndef TICKET_CHECKER_H
#define TICKET_CHECKER_H

#ifdef HAVE_ABT_H
    #include "argobots/pollingcondition.h"
    using namespace abt;
#endif

#include "mpi/requestset.h"
#include "mpi/statusset.h"
#include "pollingchecker.h"
#include "ticket.h"

//#include <mpi.h>
//#include <iterator>
//#include <utility>
//#include <cassert>

namespace nanos {
namespace mpi {

//! Polling ConditionChecker for MPI requests.
/*!
  TicketChecker contains the necessary data to perform tests over
  one or many MPI requests and store the status output value.
  \param RequestType Data type used to represent a request
  \param StatusType Data type used to represent a status
  \param kind Specifies whether the status should be ignored or not {StatusKind::attend, StatusKind::ignore}
  \param ErrorType Data type used to represent error codes
  \param count Number of request that this ticket will contain. If 0 or not specified, the number of requests is not fixed (dynamic storage is used in this case).
 */
template <
    typename RequestType, 
    typename StatusType, 
    StatusKind ignoreStatus,
    typename ErrorType,
    size_t count = 0 // Fixed number of items. 0 : use dynamic container
>
class TicketChecker : public PollingChecker {
protected:
    //! Contains all the requests that must be checked.
    RequestSet<RequestType, count> _requests;

    //! Contain the result of the checks for each request.-
    StatusSet<StatusType, ignoreStatus, count> _statuses;

    //! Holds the return value of the last check.
    ErrorType _error;
public:
    //! Default constructor. 
    /*!
      Creates an empty ticket with uninitialized requests.
      Requests may be initialized later on.
     */
    TicketChecker() :
        _requests(),
        _statuses(),
        _error()
    {
    }

    //! Request array constructor
    /*!
      Creates a ticket that will check for the completion of
      a given set of requests.
      \warning The request capacity of the ticket must be enough to hold
      all of them. Using fixed size is not recommended in this case
      for this reason.
     */
    TicketChecker( size_t len, RequestType *r, StatusType *s ) :
        _requests( r, static_cast<size_t>(len) ),
        _statuses( s, static_cast<size_t>(len) ),
        _error()
    {
        //assert( len <= _requests.capacity() );
    }

    //! Copy constructor.
    TicketChecker( TicketChecker const& t ) :
        _requests( t._requests ),
        _statuses( t._statuses ),
         _error( t._error )
    {
    }

    //! Destructor
    virtual ~TicketChecker()
    {
    }

    //! Calls MPI test.
    /*!
      Calls the specific mpi_test implementation depending on the data types 
      that are being used.
     */
    virtual bool test()
    {
        return _requests.test( _statuses );
    }

    //! \return last check's error code. Non const-reference version.
    ErrorType& getError()
    {
        return _error;
    }

    //! \return last check's error code. Non const-reference version.
    ErrorType const& getError() const
    {
        return _error;
    }

    //! Sets lasts check's error code.
    void setError( ErrorType const& value )
    {
        _error = value;
    }

    //! \return the ticket's Nth request.
    auto getRequestSet() -> typename std::add_lvalue_reference<decltype(_requests)>::type
    {
        return _requests;
    }

    //! \return the ticket's Nth requests status.
    auto getStatusSet() -> typename std::add_lvalue_reference<decltype(_statuses)>::type
    {
        return _statuses;
    }
};

} // namespace mpi
} // namespace nanos

#endif // TICKET_CHECKER_H

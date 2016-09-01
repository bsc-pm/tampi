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

#ifdef HAVE_ABT_H
    #include "argobots/pollingcondition.h"
    using namespace abt;
#else
    #include "nanos/pollingcondition.h"
#endif

#include <stdexcept>
#include <type_traits>

#include <mpi.h>

namespace nanos {
namespace mpi {

//! Polling conditional variable for MPI conditional checkers
/*!
  Ticket is an extension of the SinglePollingCond that is used in combination
  with TicketChecker in order to test the compleness of MPI requests.
  \param TicketChecker MPI checker that will be used.
 */
template<
    typename Request, 
    typename Status, 
    size_t count = 0 // Fixed number of items. 0 : use dynamic container
>
class Ticket : public nanos::SinglePollingCond {
public:

    std::array<Request,count> _requests;
    std::array<Status,count> _statuses;
    Lock _test_lock;
    int  _error;
    bool _completed;

    Ticket( const std::array<Request,count>& reqs, int error ) :
        _requests( reqs ),
        _statuses(),
        _test_lock(),
        _error( error ),
        _completed( false )
    {
    }

    //! Copy constructor.
    Ticket( Ticket const& t ) = delete;

    //! Destructor.
    virtual ~Ticket() noexcept
    {
        if( !_completed )
            std::runtime_error( "Destroying unfinished MPI requests" );
    }

    const std::array<Status,count>& getStatuses() const
    {
        return _statuses;
    }

    int getReturnError() const
    {
        return _error;
    }

    virtual bool check()
    {

        std::unique_lock<Lock> lock( _test_lock );
        if( !_completed ) {
            _completed = Request::test_all( _requests, _statuses );
        }
        return _completed;
    }
};

template<
    typename Request,
    typename Status
>
class Ticket<Request,Status,0> : public SinglePollingCond {
public:

    std::vector<Request> _requests;
    std::vector<Status>  _statuses;
    Lock         _test_lock;
    int          _error;
    Atomic<bool> _completed;

    //! Default constructor.
    Ticket( const std::vector<Request>& reqs, int error = MPI_SUCCESS ) :
        _requests( reqs ),
        _statuses( reqs.size() ),
        _test_lock(),
        _error( error ),
        _completed( false )
    {
    }

    //! Copy constructor.
    Ticket( Ticket const& t ) = delete;

    //! Destructor.
    virtual ~Ticket()
    {
        if( !_completed )
            std::runtime_error( "Destroying an unsatisfied polling condition" );
    }

    const std::vector<Status>& getStatuses() const
    {
        return _statuses;
    }

    int getReturnError() const
    {
        return _error;
    }

    virtual bool check()
    {
        if( !_completed ) {
            std::unique_lock<Lock> lock( _test_lock );
            if( !_completed ) {
                _completed = Request::test_all( _requests, _statuses );
            }
        }
        return _completed;
    }
};

} // namespace mpi
} // namespace nanos

#endif // TICKET_H

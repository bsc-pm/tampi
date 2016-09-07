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

#include "genericticket.h"

#include "mpi/error.h"

#include <type_traits>

/* Nanos6 runtime API */
extern "C" {
void *nanos_get_current_task();
void nanos_block_current_task();
void nanos_unblock_task(void *blocked_task_handler);
} //extern C

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
class Ticket : public GenericTicket {
private:
    std::array<Request,count> _requests;
    std::array<Status,count> _statuses;
    int _error;
	bool _completed;
	void * _blocked_task;

public:
    Ticket( const std::array<Request,count>& reqs, int error = MPI_SUCCESS ) :
        _requests( reqs ),
        _statuses(),
		_error( error ),
        _completed( false ),
		_blocked_task( nullptr )
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

	virtual void signal()
	{
		nanos_unblock_task( _blocked_task );
	}

	void wait()
	{
		_blocked_task = nanos_get_current_task();
		nanos_block_current_task();
	}

	bool check()
	{
		if (!_completed) {
			_completed = Request::test_all( _requests, _statuses );
		}
		return _completed;
	}
};

template<
    typename Request,
    typename Status
>
class Ticket<Request,Status,0> : public GenericTicket {
private:
    std::vector<Request> _requests;
    std::vector<Status>  _statuses;
    int _error;
	bool _completed;
	void * _blocked_task;

public:
    //! Default constructor.
    Ticket( const std::vector<Request>& reqs, int error = MPI_SUCCESS ) :
        _requests( reqs ),
        _statuses( reqs.size() ),
		_error( error ),
        _completed( false ),
		_blocked_task( nullptr )
    {
    }

    //! Copy constructor.
    Ticket( Ticket const& t ) = delete;

    //! Destructor.
    virtual ~Ticket() noexcept
    {
        if ( !_completed )
            std::runtime_error( "Destroying unfinished MPI requests" );
    }

    const std::vector<Status>& getStatuses() const
    {
        return _statuses;
    }

    int getReturnError() const
    {
        return _error;
    }

    virtual void signal()
    {
		nanos_unblock_task( _blocked_task );
    }

    void wait()
    {
		_blocked_task = nanos_get_current_task();
		nanos_block_current_task();
    }

    bool check()
    {
        if (!_completed) {
            _completed = Request::test_all( _requests, _statuses );
        }
        return _completed;
    }
};

} // namespace mpi
} // namespace nanos

#endif // TICKET_H

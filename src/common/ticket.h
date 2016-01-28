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

#include "mpi/error.h"
#include "mpi/requestset.h"
#include "mpi/statusset.h"
#include "pollingchecker.h"
#include "ticketchecker.h"

#include <type_traits>

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
class TicketBase : public SinglePollingCond<TicketChecker<Request,Status,count> > {
public:
    using checker_type = TicketChecker<Request,Status,count>;
    using super = SinglePollingCond<checker_type>;

    //! Default constructor.
    TicketBase()
    {
    }

    //! Copy constructor.
    TicketBase( TicketBase const& t ) :
        super(t)
    {
    }

    //! ConditionChecker constructor.
    /*!
      Passes a ConditionChecker's reference to its superclass.
     */
    TicketBase( checker_type const& tc ) :
        super(tc)
    {
    }

    //! Destructor.
    virtual ~TicketBase()
    {
    }

    //! Returns a reference to MPI request values.
    auto getChecker() -> typename std::add_lvalue_reference<decltype(super::_conditionChecker)>::type
    {
        return this->_conditionChecker;
    }

    //! Stops the task until the requests are completed.
    /*!
      \param err output error code.
     */
    void wait( int *err )
    {
        if( getChecker().getError().success() )
            super::waitForPollCompletion();
        if( err )
            *err = getChecker().getError().value();
    }
};

template < class Request, class Status, size_t length = 0 >
class Ticket : public TicketBase<Request,Status,length> {
public:
    using checker_type = TicketChecker<Request,Status,length>;
    using super = TicketBase<Request,Status,length>;

    //! Default constructor.
    Ticket() = default;

    //! Copy constructor.
    Ticket( Ticket const& t ) :
        super(t)
    {
    }

    //! ConditionChecker constructor.
    /*!
      Passes a ConditionChecker's reference to its superclass.
     */
    Ticket( checker_type const& tc ) :
        super(tc)
    {
    }

    //! Destructor.
    virtual ~Ticket() = default;

    using super::getChecker;

    using super::wait;

    void wait( typename Status::value_type* status, int *err )
    {
        wait( err );
        super::getChecker().getStatusSet().copy( status );
    }

	void wait( size_t nelem, typename Status::value_type array_of_statuses[], int *err )
	{
	    wait( err );
	    super::getChecker().getStatusSet().copy( array_of_statuses, nelem );
	}
};

template<
    typename Request, 
    typename Status 
>
class Ticket<Request,Status,1> : public TicketBase<Request,Status,1> {
public:
    using checker_type = TicketChecker<Request,Status,1>;
    using super = TicketBase<Request,Status,1>;

    //! Default constructor.
    Ticket() = default;

    //! Copy constructor.
    Ticket( Ticket const& t ) :
        super(t)
    {
    }

    //! ConditionChecker constructor.
    /*!
      Passes a ConditionChecker's reference to its superclass.
     */
    Ticket( checker_type const& tc ) :
        super(tc)
    {
    }

    //! Destructor.
    virtual ~Ticket() = default;

    using super::getChecker;

    using super::wait;

    void wait( typename Status::value_type* status, int *err )
    {
        wait( err );
        getChecker().getStatus().copy( status );
    }
};

} // namespace mpi
} // namespace nanos

#endif // TICKET_H

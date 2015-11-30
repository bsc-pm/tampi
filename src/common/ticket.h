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
#endif

#include "mpi/error.h"
#include "mpi/requestset.h"
#include "mpi/statusset.h"
#include "pollingchecker.h"
#include "ticketchecker.h"

#include <type_traits>

//#include <mpi.h>
//#include <iterator>
//#include <utility>
//#include <cassert>

namespace nanos {
namespace mpi {

//! Polling conditional variable for MPI conditional checkers
/*!
  Ticket is an extension of the SinglePollingCond that is used in combination
  with TicketChecker in order to test the compleness of MPI requests.
  \param TicketChecker MPI checker that will be used.
 */
template<
    typename RequestType, 
    typename StatusType, 
    StatusKind ignoreStatus,
    typename ErrorType,
    size_t count = 0 // Fixed number of items. 0 : use dynamic container
>
class Ticket : public SinglePollingCond<TicketChecker<RequestType,StatusType,ignoreStatus,ErrorType,count> > {
public:
    using checker_type = TicketChecker<RequestType,StatusType,ignoreStatus,ErrorType,count>;
    using super = SinglePollingCond<checker_type>;

    //! Default constructor.
    Ticket()
    {
    }

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
    virtual ~Ticket()
    {
    }

    //! Returns a reference to MPI request values.
    RequestSet<RequestType,count> &getRequestSet()
    {
        return this->_conditionChecker.getRequestSet();
    }

    StatusSet<StatusType,ignoreStatus,count> &getStatusSet()
    {
        return this->_conditionChecker.getStatusSet();
    }

    Error<ErrorType> const& getError() const
    {
        return this->_conditionChecker.getError();
    }

    Error<ErrorType>& getError()
    {
        return this->_conditionChecker.getError();
    }

    void setError( Error<ErrorType> const& error )
    {
        return this->_conditionChecker.setError( error );
    }

    //! Stops the task until the requests are completed.
    /*!
      \param err output error code.
     */
    void wait( ErrorType *err )
    {
        if( getError().success() )
            super::waitForPollCompletion();
        if( err )
            *err = getError().value();
    }

    void wait( StatusType* status, ErrorType *err )
    {
        wait( err );
        getStatusSet().copy( status );
    }

	void wait( size_t nelem, StatusType array_of_statuses[], ErrorType *err )
	{
	    wait( err );
	    getStatusSet().copy( array_of_statuses, nelem );
	}
};

} // namespace mpi
} // namespace nanos

#endif // TICKET_H

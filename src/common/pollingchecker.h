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

#ifndef POLLINGCHECKER_H
#define POLLINGCHECKER_H

#include <nanox-dev/synchronizedcondition.hpp>

/*! \file pollingchecker.h
  Specializes SynchronizedCondition and ConditionChecker classes for
  conditions that have to be check periodically (polling).
 */

namespace nanos {
namespace mpi {

//! Provides a mechanism to safely check for a condition over time.
/*!
 PollingChecker is designed for conditions that have to be checked
 once in a while. It is designed for one time valid checks, that is,
 checks that must not be performed any more after they succeed.
 */
class PollingChecker : public ConditionChecker
{
    private:
        using super = ConditionChecker ;
        //! Provides thread safety for concurrent checks.
        Lock           _lock;
        //! Holds the result of the last check.
        Atomic<bool>   _completed;
    public:
        //! Default initializer. Condition result is initialized to false.
        PollingChecker () :
            super(), _lock(), _completed( false )
        {}

        //! Copy constructor.
    	  PollingChecker ( PollingChecker const& other ) :
	        super( other ), _lock(), _completed( other._completed )
        {}

        //! Destructor.
        virtual ~PollingChecker() {}

        //! Copy assignment operator.
	     PollingChecker const& operator=( PollingChecker const& other ) {
            _completed = other._completed;
            return *this;
        }

        //! Performs the periodic check. To be implemented by each specialization.
        virtual bool test() = 0;

        //! SynchronizedCondition generic check operation.
        /*!
          Calls the user implemented test if and only if the last time,
          if any, it did not succeed.
         */
        virtual bool checkCondition () {
            // We need to store the poll result because it may not be safe
            // to poll again if the task has already finished
            // (e.g. when the condition was satisfied before blocking the task)
            if( !_completed.value() ) {
                _lock.acquire();
                if( !_completed.value() ) {
                    _completed = test();
                }
                _lock.release();
            }
            return _completed.value();
        }
};


//! Conditional variable used for periodic checks.
/*! 
  Checks the condition and registers itself in a scheduler list to
  be checked when workers are idle or searching for work.
 */
template < class PollingChecker >
class SinglePollingCond : public SingleSyncCond< PollingChecker > {
public:
    //! Default constructor.
    SinglePollingCond() : SingleSyncCond<PollingChecker>() {}

    //! Constructs the condition variable with a reference of PollingChecker.
    SinglePollingCond( PollingChecker const& c ) : SingleSyncCond<PollingChecker>(c) {}

    //! Constructs the condition variable with a rvalue reference of PollingChecker.
    SinglePollingCond( PollingChecker && c ) : SingleSyncCond<PollingChecker>(c) {}

    //! Default destructor.
    virtual ~SinglePollingCond() {}

    //! Registers itself into scheduler's condition check list.
    void waitForPollCompletion() {
        nanos_polling_cond_wait( this );
    }
};

}// namespace mpi
}// namespace nanos

#endif // POLLINGCHECKER_H

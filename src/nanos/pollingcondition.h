
#ifndef POLLING_CONDITION_H
#define POLLING_CONDITION_H

#include <nanos.h>
#include <basethread.hpp>
#include <synchronizedcondition.hpp>

#include <mutex>

namespace nanos {

//! Conditional variable used for periodic checks.
/*! 
  Checks the condition and registers itself in a scheduler list to
  be checked when workers are idle or searching for work.
 */
class SinglePollingCond : public GenericSyncCond {
private:
    WorkDescriptor* _waiter;

public:
    //! Default constructor.
    SinglePollingCond() :
        _waiter(nullptr)
    {
    }

    //! Default destructor.
    virtual ~SinglePollingCond() noexcept
    {
    }

    virtual bool check() = 0;

    virtual void signal()
    {
        signal_one();
    }

    virtual void signal_one()
    {
        if( _waiter ) {
           Scheduler::wakeUp( _waiter );
            _waiter = nullptr;
        }
    }

    virtual void addWaiter( WorkDescriptor* wd )
    {
        _waiter = wd;
    }

    virtual bool hasWaiters()
    {
        return _waiter;
    }

    virtual WorkDescriptor* getAndRemoveWaiter()
    {
        WorkDescriptor* curr_waiter = _waiter;
        _waiter = nullptr;
        return curr_waiter;
    }

    //! Registers itself into scheduler's condition check list.
    void wait() {
        if( !check() ) {
            myThread->getTeam()->getSchedulePolicy().queue( myThread, this );
            Scheduler::waitOnCondition(this);
        }
    }
};

} // namespace nanos

#endif // POLLING_CONDITION_H

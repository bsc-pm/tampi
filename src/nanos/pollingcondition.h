
#ifndef POLLING_CONDITION_H
#define POLLING_CONDITION_H

#include <nanos.h>
#include <basethread.hpp>
#include <synchronizedcondition.hpp>
#include <workdescriptor.hpp>

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
    Atomic<bool>    _completed;

public:
    //! Default constructor.
    SinglePollingCond() :
        _waiter(nullptr),
        _completed(false)
    {
    }

    //! Default destructor.
    virtual ~SinglePollingCond() noexcept
    {
    }

    virtual void addWaiter( WorkDescriptor* wd )
    {
        // Avoid process to hang if the owner thread
        // is acquired by other process (DLB)
        wd->untie();

        _waiter = wd;
    }

    /*! \brief checks for polling completion.
     * Polling is considered completed if either
     * the polling thread has called signal()
     * or the waiting thread has verified that
     * the condition was already fulfilled even
     * before calling the wait() function.
     */
    virtual bool check()
    {
        return _completed;
    }

    virtual WorkDescriptor* getAndRemoveWaiter()
    {
        WorkDescriptor* curr_waiter = _waiter;
        _waiter = nullptr;
        return curr_waiter;
    }

    virtual bool hasWaiters()
    {
        return _waiter;
    }

    virtual void signal()
    {
        signal_one();
    }

    /*! \brief Signals the waiting workdescriptor (if exists)
     * Wakes waiting workdescriptor up if exists.
     * It assumes that the condition is satisfied, otherwise
     * its behaviour is undefined.
     */
    virtual void signal_one()
    {
        std::unique_lock<SinglePollingCond> block( *this );
	    _completed = true;

        if( _waiter ) {
           Scheduler::wakeUp( _waiter );
            _waiter = nullptr;
        }
    }

    //! Polls to check if the condition has finished
    virtual bool test() = 0;

    //! Registers itself into scheduler's condition check list.
    void wait() {
        std::unique_lock<SinglePollingCond> block( *this );
        if( !_completed )
            _completed = test();
        block.unlock();

        if( !_completed ) {
            myThread->getTeam()->getSchedulePolicy().queue( myThread, this );
            Scheduler::waitOnCondition(this);
        }
    }
};

} // namespace nanos

#endif // POLLING_CONDITION_H

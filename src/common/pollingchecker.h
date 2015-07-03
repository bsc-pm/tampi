#ifndef MPICONDITIONCHECKER_H
#define MPICONDITIONCHECKER_H

#include <nanox-dev/synchronizedcondition.hpp>

namespace nanos {
namespace mpi {

class PollingChecker : public ConditionChecker
{
    private:
	    typedef ConditionChecker super;
        Lock           _lock;
    	Atomic<bool>   _completed;
    public:
        PollingChecker () :
            super(), _lock(), _completed( false )
        {}

    	PollingChecker ( PollingChecker const& other ) :
	        super( other ), _lock(), _completed( other._completed )
        {}

        virtual ~PollingChecker() {}

	    PollingChecker const& operator=( PollingChecker const& other ) {
            _completed = other._completed;
            return *this;
    	}

        virtual bool test() = 0;

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


template < class PollingChecker >
class SinglePollingCond : public SingleSyncCond< PollingChecker > {
public:
    SinglePollingCond() : SingleSyncCond<PollingChecker>() {}
    SinglePollingCond( PollingChecker const& c ) : SingleSyncCond<PollingChecker>(c) {}
    SinglePollingCond( PollingChecker && c ) : SingleSyncCond<PollingChecker>(c) {}

    virtual ~SinglePollingCond() {}

    void waitForPollCompletion() {
        nanos_polling_cond_wait( this );
    }
};

}// namespace mpi
}// namespace nanos

#endif // MPICONDITIONCHECKER_H

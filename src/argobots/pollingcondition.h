
#ifndef POLLING_CONDITION_H
#define POLLING_CONDITION_H

#include "common/pollingchecker.h"
#include "synccondition.h"

namespace abt {

//! Conditional variable used for periodic checks.
/*! 
  Checks the condition and registers itself in a scheduler list to
  be checked when workers are idle or searching for work.
 */
template < class PollingChecker >
class SinglePollingCond : public SynchronizedCondition< PollingChecker > {
public:
    //! Default constructor.
    SinglePollingCond()
    {
    }

    //! Constructs the condition variable with a reference of PollingChecker.
    SinglePollingCond( PollingChecker const& c ) :
        SynchronizedCondition<PollingChecker>(c)
    {
    }

    //! Constructs the condition variable with a rvalue reference of PollingChecker.
    SinglePollingCond( PollingChecker && c ) :
        SynchronizedCondition<PollingChecker>(c)
    {
    }

    //! Default destructor.
    virtual ~SinglePollingCond() {}

    //! Registers itself into scheduler's condition check list.
    void waitForPollCompletion() {
        nanos_polling_cond_wait( this );
    }
};

} // namespace abt

#endif // POLLING_CONDITION_H

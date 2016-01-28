
#ifndef POLLING_CONDITION_H
#define POLLING_CONDITION_H

#include "common/pollingchecker.h"

#include <synchronizedcondition.hpp>

namespace nanos {

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

} // namespace nanos

#endif // POLLING_CONDITION_H

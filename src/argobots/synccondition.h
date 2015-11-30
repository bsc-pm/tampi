
#ifndef SYNC_CONDITION_H
#define SYNC_CONDITION_H

#include "genericsynccond.h"
#include "eventual.h"

#include <atomic>

namespace abt {

/*! \brief Abstract template synchronization class.
 */
template<class T>
class SynchronizedCondition : public GenericSyncCond
{
   protected:
      T _conditionChecker; /**< ConditionChecker associated to the SynchronizedCondition. */
		Eventual<T*> _synchronizationProvider;
   
   public:
     /*! \brief SynchronizedCondition default constructor
      */
      SynchronizedCondition() : 
			_conditionChecker(),
			_synchronizationProvider()
		{
		}

     /*! \brief SynchronizedCondition copy constructor
      */
      SynchronizedCondition ( SynchronizedCondition const& sc ) = delete;

     /*! \brief SynchronizedCondition copy assignment operator
      */
      SynchronizedCondition& operator=( const SynchronizedCondition & sc ) = delete;

     /*! \brief SynchronizedCondition constructor - 1
      */
      SynchronizedCondition ( T const& cc ) :
			_conditionChecker(cc),
			_synchronizationProvider()
		{
		}

     /*! \brief SynchronizedCondition destructor
      */
      virtual ~SynchronizedCondition()
		{
		}

     /*! \brief Wait until the condition has been satisfied
      */
      virtual void wait()
		{
			_synchronizationProvider.wait();
		}

     /*! \brief Signal the waiters if the condition has been satisfied. If they
      * are blocked they will be set to ready and enqueued.
      */
      virtual void signal_one()
		{
			signal();
		}

     /*! \brief Signal the waiters if the condition has been satisfied. If they
      * are blocked they will be set to ready and enqueued.
      */
      virtual void signal()
		{
			_synchronizationProvider.set( &_conditionChecker );
		}

     /*! \brief Change the condition checker associated to the synchronizedConditon object.
      */
      void setConditionChecker( T const& cc )
      {
         _conditionChecker = cc;
      }

     /*! \brief Check the condition
      */
      bool check ()
      {
         return _conditionChecker.checkCondition();
      }
};

} // namespace abt

#endif // SYNC_CONDITION_H


#include <mpi.h>
#include <iostream>

#include "synchronizedcondition_decl.hpp"
#include "schedule_decl.hpp"

namespace nanos {

 void GenericSyncCond::lock()
{
   _lock.acquire();
   memoryFence();
}

 void GenericSyncCond::unlock()
{
   memoryFence();
   _lock.release();
}

template <class _T>
 void SynchronizedCondition< _T>::wait()
{
   Scheduler::waitOnCondition(this);
}

template <class _T>
 void SynchronizedCondition< _T>::reference()
{
   _refcount++;
   memoryFence();
}

template <class _T>
 void SynchronizedCondition< _T>::unreference()
{
   _refcount--;
}

template <class _T>
 void SynchronizedCondition< _T>::waitConditionAndSignalers()
{
   Scheduler::waitOnCondition(this);
   while ( _refcount.value() > 0 ) {
      memoryFence();
   }
}

template <class _T>
 void SynchronizedCondition< _T>::signal()
{
   lock();
   while ( hasWaiters() ) {
      WD* wd = getAndRemoveWaiter();
      Scheduler::wakeUp(wd);
   }
   unlock(); 
}

template <class _T>
 void SynchronizedCondition< _T>::signal_one()
{
   lock();
   if ( hasWaiters() ) {
      WD* wd = getAndRemoveWaiter();
      Scheduler::wakeUp(wd);
   }
   unlock();
}


class MpiConditionChecker : public ConditionChecker {
	private:
		MPI_Request *_req;
		MPI_Status  *_status;

	public:
		MpiConditionChecker( MPI_Request *req, MPI_Status *status ) :
			_req ( req ), _status( status )
		{}

		MpiConditionChecker( MpiConditionChecker const& other ) :
			_req ( other._req ), _status( other._status )
		{}

		~MpiConditionChecker() {}

		bool checkCondition() {
			int flag;
			MPI_Test( _req, &flag, _status );

			int rank;
			MPI_Comm_rank( MPI_COMM_WORLD, &rank );
			std::cout << "R" << rank << " checking request. Result: " << flag << std::endl;
			return flag == 1;
		}
};

class MpiTestCond : public SingleSyncCond<MpiConditionChecker>
{
	public:
		MpiTestCond( MPI_Request *req, MPI_Status *status ) : nanos::SingleSyncCond<MpiConditionChecker>( MpiConditionChecker( req, status ) )
		{
		}

		/*! \brief SingleSyncCond destructor
		 */
		virtual ~MpiTestCond() { }
};

}


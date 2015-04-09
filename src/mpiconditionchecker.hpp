#include <mpi.h>
#include <cassert>
#include <vector>

#include <nanox-dev/synchronizedcondition_decl.hpp>
#include <nanox-dev/schedule_decl.hpp>

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

namespace mpi {
class SinglePendingCheck : ConditionChecker
{
    private:
        MPI_Request *_request;
        MPI_Status  *_status;
    public:
        SinglePendingCheck ( MPI_Request *request, MPI_Status *status ) :
            _request( request ), _status( status )
        {}

        MPI_Request* getRequests() { return _request; }
        MPI_Status* getStatuses() { return _status; }
        size_t getSize() const { return 1; }
        bool checkCondition () {
            int flag;
            MPI_Test( getRequests(), &flag, getStatuses() );
            return flag == 1;
        }
};

class MultiplePendingCheck : ConditionChecker {
    private:
        std::vector<MPI_Request> _requests;
        std::vector<MPI_Status > _statuses;
    public:
        MultiplePendingCheck ( std::initializer_list<MPI_Request> const& requests, std::initializer_list<MPI_Status> const& statuses ) :
            _requests( requests ), _statuses( statuses)
        {
            assert( requests.size() == statuses.size() );
        }

        MPI_Request* getRequests() { return &_requests[0]; }
        MPI_Status* getStatuses() { return &_statuses[0]; }
        size_t getSize() const { return _requests.size(); }
        bool checkCondition() {
            int flag;
            MPI_Testall( _requests.size(), getRequests(), &flag, getStatuses() );
            return flag == 1;
        }
};

class SingleTestCond : public SingleSyncCond<SinglePendingCheck>
{
    typedef SingleSyncCond<SinglePendingCheck> super;
	public:
        SingleTestCond( MPI_Request *req, MPI_Status *status ) :
            super( SinglePendingCheck( req, status ) )
        {}

		/*! \brief SingleTestCond destructor
		 */
		virtual ~SingleTestCond() { }
};

class MultipleTestCond : public SingleSyncCond<MultiplePendingCheck>
{
    typedef SingleSyncCond<MultiplePendingCheck> super;
	public:
        MultipleTestCond ( std::initializer_list<MPI_Request> const& requests, std::initializer_list<MPI_Status> const& statuses ) :
            super( MultiplePendingCheck( requests, statuses ) )
        {}

		/*! \brief SingleTestCond destructor
		 */
		virtual ~MultipleTestCond() { }
};


}// namespace mpi
}// namespace nanos


#include <mpi.h>

#include <sstream>
#include <nanox-dev/synchronizedcondition.hpp>
#include <nanox-dev/schedule_decl.hpp>

namespace nanos {

namespace mpi {

#if 0
// C++ Version of MPI_Test
static bool test_request( MPI::Request &req, int* flag, MPI::Status &status )
{
    bool ret = req.Test( status );
    *flag = (int) ret;
    return ret;
}

// C++ Version of MPI_Testall
static bool test_all_requests( size_t count, MPI::Request array_of_requests[], int *flag, MPI::Status array_of_statuses[] )
{
    bool ret = MPI::Request::Testall( count, array_of_requests, array_of_statuses );
    *flag = (int) ret;
    return ret;
}
#endif

// C Version of MPI_Test
static int test_request( MPI_Request *req, int *flag, MPI_Status *status )
{
    return MPI_Test( req, flag, status );
}

// C Version of MPI_Testall
static int test_all_requests( size_t count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[] )
{
    return MPI_Testall( count, array_of_requests, flag, array_of_statuses );
}

template <typename Request, typename Status, typename Result>
class SinglePendingCheck : ConditionChecker
{
    private:
	typedef ConditionChecker super;
        Request _request;
        Status  _status;
        Result  _result;
	bool    _satisfied;
    public:
        SinglePendingCheck ( Request request, Status status ) :
            super(), _request( request ), _status( status ), _satisfied( false )
        {}

	SinglePendingCheck ( SinglePendingCheck const& other ) :
	    super( other ), _request(other._request), _status(other._status), 
            _result(other._result), _satisfied( other._satisfied )
        {}

        virtual ~SinglePendingCheck() {}

	SinglePendingCheck &operator=( SinglePendingCheck const& other ) {
	    _request = other._request;
	    _status = other._status;
	    _result = other._result;
            _satisfied = other._satisfied;
	}

        Request getRequest() { return _request; }
        Status getStatus() { return _status; }
        int getResult() const { return _result; }

        virtual bool checkCondition () {
            // We need to store the condition because it may not be safe
            // to use the other members if a task was not finally blocked
            // (i.e. the condition satisfied just before blocking the task)
            if( !_satisfied ) {
                int flag;
                _result = test_request( _request, &flag, _status );
                _satisfied = flag == 1;
            }
            return _satisfied;
        }
};

template <typename Request, typename Status, typename Result>
class MultiplePendingCheck : ConditionChecker {
    private:
        typedef ConditionChecker super;
        size_t   _length;
        Request* _requests;
        Status*  _statuses;
        Result   _result;
	bool     _satisfied;
    public:
        MultiplePendingCheck ( size_t len, Request array_of_requests[], Status array_of_statuses[] ) :
            super(), _length( len ), _requests( array_of_requests ), _statuses( array_of_statuses ),
            _satisfied( false )
        {}

	MultiplePendingCheck ( MultiplePendingCheck const& other ) :
	    super( other ), _length( other._length ), _requests(other._requests), 
            _statuses(other._statuses), _result(other._result), _satisfied( other._satisfied )
        {}

	MultiplePendingCheck &operator=( MultiplePendingCheck const& other ) {
	    _length = other._length;
	    _requests = other._requests;
	    _statuses = other._statuses;
	    _result = other._result;
            _satisfied = other._satisfied;
	}

	virtual ~MultiplePendingCheck() {}

        Request* getRequests() { return _requests; }
        Status* getStatuses() { return _statuses; }
        Result getResult() const { return _result; }
        size_t getSize() const { return _length; }

        virtual bool checkCondition() {
            // We need to store the condition because it may not be safe
            // to use the other members if a task was not finally blocked
            // (i.e. the condition satisfied just before blocking the task)
            if( !_satisfied ) {
                int flag;
                _result = test_all_requests( _length, _requests, &flag, _statuses );
                _satisfied = flag == 1;
            }
            return _satisfied;
        }
};

// Template specifies the type of Request and Status objects
// For C: Request = MPI_Request*, Status = MPI_Status*, Result: int
// For C++: Request = Request&, Status = Status&, Result: bool
template <typename Request, typename Status, typename Result>
class SingleTestCond : public SingleSyncCond<SinglePendingCheck<Request,Status,Result> >
{
    typedef SinglePendingCheck<Request,Status,Result> CheckType;
    typedef SingleSyncCond< CheckType > super;
    public:
        SingleTestCond( Request req, Status status ) :
            super( CheckType( req, status ) )
        {}

        /*! \brief SingleTestCond destructor
         */
        virtual ~SingleTestCond() {}

        Result getResult()
        {
            return super::_conditionChecker.getResult();
        }
};

// Template specifies the type of Request and Status objects
// For C: Request = MPI_Request*, Status = MPI_Status*, Result: int
// For C++: Request = Request&, Status = Status&, Result: bool
template <typename Request, typename Status, typename Result>
class MultipleTestCond : public SingleSyncCond<MultiplePendingCheck<Request,Status,Result> >
{
    typedef MultiplePendingCheck<Request,Status,Result> CheckType;
    typedef SingleSyncCond< CheckType > super;
    public:
        MultipleTestCond ( size_t len, Request array_of_requests[], Status array_of_statuses[] ) :
            super( CheckType( len, array_of_requests, array_of_statuses ) )
        {}

        /*! \brief MultipleTestCond destructor
         */
        virtual ~MultipleTestCond() {}

        Result getResult()
        {
            return super::_conditionChecker.getResult();
        }
};

typedef SingleTestCond<MPI_Request*,MPI_Status*,int> MPI_SingleTest;
typedef MultipleTestCond<MPI_Request,MPI_Status,int> MPI_MultipleTest;
typedef SingleTestCond<MPI::Request&,MPI::Status&,bool> SingleTest;
typedef MultipleTestCond<MPI::Request,MPI::Status,bool> MultipleTest;

}// namespace mpi
}// namespace nanos


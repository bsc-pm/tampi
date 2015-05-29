#include <mpi.h>
#include <iostream>
#include <sstream>
#include <nanox/nanos.h>
#include <nanox-dev/synchronizedcondition.hpp>
#include <nanox-dev/schedule_decl.hpp>

namespace nanos {

namespace print {
    template <typename OStreamType>
    static inline OStreamType &join( OStreamType &&os )
    {
       os << std::endl;
       return os;
    }
    
    template <typename OStreamType, typename T, typename...Ts>
    static inline OStreamType &join( OStreamType &&os, const T &first, const Ts&... rest)
    {
       os << first;
       return join( os, rest... );
    }

    template <typename...Ts>
    inline void dbg( const Ts&... msg )
    {
       std::stringstream ss;
       join( ss, msg... );
       nanos_debug( ss.str().c_str() );
    }

    // FIXME: try to detect at configure time which nanos version is being used (nanos_get_mode api call)
    // and enable define preprocessor flags acordingly.
}

namespace mpi {

static void print_status_error( MPI_Status *status ) {
    switch( status->MPI_ERROR ) {
        case MPI_SUCCESS: std::cerr << "MPI_SUCCESS" << std::endl;break;                   
        case MPI_ERR_BUFFER: std::cerr << "MPI_ERR_BUFFER" << std::endl;break;                
        case MPI_ERR_COUNT: std::cerr << "MPI_ERR_COUNT" << std::endl;break;                 
        case MPI_ERR_TYPE: std::cerr << "MPI_ERR_TYPE" << std::endl;break;                  
        case MPI_ERR_TAG: std::cerr << "MPI_ERR_TAG" << std::endl;break;                   
        case MPI_ERR_COMM: std::cerr << "MPI_ERR_COMM" << std::endl;break;                  
        case MPI_ERR_RANK: std::cerr << "MPI_ERR_RANK" << std::endl;break;                  
        case MPI_ERR_REQUEST: std::cerr << "MPI_ERR_REQUEST" << std::endl;break;               
        case MPI_ERR_ROOT: std::cerr << "MPI_ERR_ROOT" << std::endl;break;                  
        case MPI_ERR_GROUP: std::cerr << "MPI_ERR_GROUP" << std::endl;break;                 
        case MPI_ERR_OP: std::cerr << "MPI_ERR_OP" << std::endl;break;                    
        case MPI_ERR_TOPOLOGY: std::cerr << "MPI_ERR_TOPOLOGY" << std::endl;break;              
        case MPI_ERR_DIMS: std::cerr << "MPI_ERR_DIMS" << std::endl;break;                  
        case MPI_ERR_ARG: std::cerr << "MPI_ERR_ARG" << std::endl;break;                   
        case MPI_ERR_UNKNOWN: std::cerr << "MPI_ERR_UNKNOWN" << std::endl;break;               
        case MPI_ERR_TRUNCATE: std::cerr << "MPI_ERR_TRUNCATE" << std::endl;break;              
        case MPI_ERR_OTHER: std::cerr << "MPI_ERR_OTHER" << std::endl;break;                 
        case MPI_ERR_INTERN: std::cerr << "MPI_ERR_INTERN" << std::endl;break;                
        case MPI_ERR_IN_STATUS: std::cerr << "MPI_ERR_IN_STATUS" << std::endl;break;             
        case MPI_ERR_PENDING: std::cerr << "MPI_ERR_PENDING" << std::endl;break;               
        case MPI_ERR_ACCESS: std::cerr << "MPI_ERR_ACCESS" << std::endl;break;                
        case MPI_ERR_AMODE: std::cerr << "MPI_ERR_AMODE" << std::endl;break;                 
        case MPI_ERR_ASSERT: std::cerr << "MPI_ERR_ASSERT" << std::endl;break;                
        case MPI_ERR_BAD_FILE: std::cerr << "MPI_ERR_BAD_FILE" << std::endl;break;              
        case MPI_ERR_BASE: std::cerr << "MPI_ERR_BASE" << std::endl;break;                  
        case MPI_ERR_CONVERSION: std::cerr << "MPI_ERR_CONVERSION" << std::endl;break;            
        case MPI_ERR_DISP: std::cerr << "MPI_ERR_DISP" << std::endl;break;                  
        case MPI_ERR_DUP_DATAREP: std::cerr << "MPI_ERR_DUP_DATAREP" << std::endl;break;           
        case MPI_ERR_FILE_EXISTS: std::cerr << "MPI_ERR_FILE_EXISTS" << std::endl;break;           
        case MPI_ERR_FILE_IN_USE: std::cerr << "MPI_ERR_FILE_IN_USE" << std::endl;break;           
        case MPI_ERR_FILE: std::cerr << "MPI_ERR_FILE" << std::endl;break;                  
        case MPI_ERR_INFO_KEY: std::cerr << "MPI_ERR_INFO_KEY" << std::endl;break;              
        case MPI_ERR_INFO_NOKEY: std::cerr << "MPI_ERR_INFO_NOKEY" << std::endl;break;            
        case MPI_ERR_INFO_VALUE: std::cerr << "MPI_ERR_INFO_VALUE" << std::endl;break;            
        case MPI_ERR_INFO: std::cerr << "MPI_ERR_INFO" << std::endl;break;                  
        case MPI_ERR_IO: std::cerr << "MPI_ERR_IO" << std::endl;break;                    
        case MPI_ERR_KEYVAL: std::cerr << "MPI_ERR_KEYVAL" << std::endl;break;                
        case MPI_ERR_LOCKTYPE: std::cerr << "MPI_ERR_LOCKTYPE" << std::endl;break;              
        case MPI_ERR_NAME: std::cerr << "MPI_ERR_NAME" << std::endl;break;                  
        case MPI_ERR_NO_MEM: std::cerr << "MPI_ERR_NO_MEM" << std::endl;break;                
        case MPI_ERR_NOT_SAME: std::cerr << "MPI_ERR_NOT_SAME" << std::endl;break;              
        case MPI_ERR_NO_SPACE: std::cerr << "MPI_ERR_NO_SPACE" << std::endl;break;              
        case MPI_ERR_NO_SUCH_FILE: std::cerr << "MPI_ERR_NO_SUCH_FILE" << std::endl;break;          
        case MPI_ERR_PORT: std::cerr << "MPI_ERR_PORT" << std::endl;break;                  
        case MPI_ERR_QUOTA: std::cerr << "MPI_ERR_QUOTA" << std::endl;break;                 
        case MPI_ERR_READ_ONLY: std::cerr << "MPI_ERR_READ_ONLY" << std::endl;break;             
        case MPI_ERR_RMA_CONFLICT: std::cerr << "MPI_ERR_RMA_CONFLICT" << std::endl;break;          
        case MPI_ERR_RMA_SYNC: std::cerr << "MPI_ERR_RMA_SYNC" << std::endl;break;              
        case MPI_ERR_SERVICE: std::cerr << "MPI_ERR_SERVICE" << std::endl;break;               
        case MPI_ERR_SIZE: std::cerr << "MPI_ERR_SIZE" << std::endl;break;                  
        case MPI_ERR_SPAWN: std::cerr << "MPI_ERR_SPAWN" << std::endl;break;                 
        case MPI_ERR_UNSUPPORTED_DATAREP: std::cerr << "MPI_ERR_UNSUPPORTED_DATAREP" << std::endl;break;   
        case MPI_ERR_UNSUPPORTED_OPERATION: std::cerr << "MPI_ERR_UNSUPPORTED_OPERATION" << std::endl;break; 
        case MPI_ERR_WIN: std::cerr << "MPI_ERR_WIN" << std::endl;break;                   
        case MPI_ERR_LASTCODE: std::cerr << "MPI_ERR_LASTCODE" << std::endl;break;              
        default: std::cerr << "unkown status" << std::endl;break;
    }
}

static void print_mpi_errors( int errcode, MPI_Status *status, int count = 1 ) {
    switch( errcode ) {
        case MPI_SUCCESS:
            break;

        case MPI_ERR_IN_STATUS:
            std::cerr << "MPI routine error: ";
           if( status == MPI_STATUS_IGNORE 
            || status == MPI_STATUSES_IGNORE ) {
               std::cerr << "no status object was provided." << std::endl;
            } else {
               for( int i = 0; i < count; i++ ) {
                   std::cerr << "  ";
                   print_status_error( &status[i] );
               }
            }
            break;

        case MPI_ERR_REQUEST:
	    std::cerr << "Invalid MPI_Request. Either null or, in the case of a MPI_Start or MPI_Startall, not a persistent request." << std::endl;
            break;

        case MPI_ERR_ARG:
            std::cerr << "Invalid argument. Some argument is invalid and is not identified by a specific error class (e.g., MPI_ERR_RANK)." << std::endl;
            break;
    }
}

// C Version of MPI_Test
static int test_request( MPI_Request *req, int *flag, MPI_Status *status )
{
    int result = MPI_Test( req, flag, status );
    //print_mpi_errors( result, status );
    return result;
}

// C Version of MPI_Testall
static int test_all_requests( size_t count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[] )
{
    int result =  MPI_Testall( count, array_of_requests, flag, array_of_statuses );
    //print_mpi_errors( result, array_of_statuses, count );
    return result;
}

template <typename Request, typename Status, typename Result>
class SinglePendingCheck : ConditionChecker
{
    private:
	typedef ConditionChecker super;
        Request        _request;
        Status         _status;
        Result         _result;
        Lock           _lock;
	Atomic<bool>   _satisfied;
    public:
        SinglePendingCheck ( Request request, Status status ) :
            super(), _request( request ), _status( status ),
            _satisfied( false ), _lock()
        {}

	SinglePendingCheck ( SinglePendingCheck const& other ) :
	    super( other ), _request(other._request), _status(other._status), 
            _result(other._result), _satisfied( other._satisfied ), _lock()
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
            if( !_satisfied.value() ) {
                _lock.acquire();
                if( !_satisfied.value() ) {
                    int flag;
                    _result = test_request( _request, &flag, _status );
                    _satisfied = flag == 1;
                }
                _lock.release();
            }
            return _satisfied.value();
        }
};

template <typename Request, typename Status, typename Result>
class MultiplePendingCheck : ConditionChecker {
    private:
        typedef ConditionChecker super;
        size_t       _length;
        Request*     _requests;
        Status*      _statuses;
        Result       _result;
        Lock         _lock;
	Atomic<bool> _satisfied;
    public:
        MultiplePendingCheck ( size_t len, Request array_of_requests[], Status array_of_statuses[] ) :
            super(), _length( len ), _requests( array_of_requests ), _statuses( array_of_statuses ),
            _satisfied( false ), _lock()
        {}

	MultiplePendingCheck ( MultiplePendingCheck const& other ) :
	    super( other ), _length( other._length ), _requests(other._requests), 
            _statuses(other._statuses), _result(other._result), _satisfied( other._satisfied ), _lock()
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

        virtual bool checkCondition () {
            // We need to store the condition because it may not be safe
            // to use the other members if a task was not finally blocked
            // (i.e. the condition satisfied just before blocking the task)
            if( !_satisfied.value()) {
                _lock.acquire();
                if( !_satisfied.value() ) {
                    int flag;
                    _result = test_all_requests( _length, _requests, &flag, _statuses );
                    _satisfied = flag == 1;
                }
                _lock.release();
            }
            return _satisfied.value();
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
        /*! \brief MultipleTestCond constructor
         */
        MultipleTestCond ( size_t len, Request array_of_requests[], Status array_of_statuses[] ) :
            super( CheckType( len, array_of_requests, array_of_statuses ) )
        {}

        /*! \brief MultipleTestCond destructor
         */
        virtual ~MultipleTestCond() {}

        /*! \brief returns the return value of the MPI test operation
         */
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


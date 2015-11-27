
#ifndef ERROR_H
#define ERROR_H

#include <mpi.h>

namespace nanos {
namespace mpi {

///* MPI's error classes */
//MPI_SUCCESS          0      /* Successful return code */
///* Communication argument parameters */
//MPI_ERR_BUFFER       1      /* Invalid buffer pointer */
//MPI_ERR_COUNT        2      /* Invalid count argument */
//MPI_ERR_TYPE         3      /* Invalid datatype argument */
//MPI_ERR_TAG          4      /* Invalid tag argument */
//MPI_ERR_COMM         5      /* Invalid communicator */
//MPI_ERR_RANK         6      /* Invalid rank */
//MPI_ERR_ROOT         7      /* Invalid root */
//MPI_ERR_TRUNCATE    14      /* Message truncated on receive */
//
///* MPI Objects (other than COMM) */
//MPI_ERR_GROUP        8      /* Invalid group */
//MPI_ERR_OP           9      /* Invalid operation */
//MPI_ERR_REQUEST     19      /* Invalid mpi_request handle */
//
///* Special topology argument parameters */
//MPI_ERR_TOPOLOGY    10      /* Invalid topology */
//MPI_ERR_DIMS        11      /* Invalid dimension argument */
//
///* All other arguments.  This is a class with many kinds */
//MPI_ERR_ARG         12      /* Invalid argument */
//
///* Other errors that are not simply an invalid argument */
//MPI_ERR_OTHER       15      /* Other error; use Error_string */
//
//MPI_ERR_UNKNOWN     13      /* Unknown error */
//MPI_ERR_INTERN      16      /* Internal error code    */
//
///* Multiple completion has three special error classes */
//MPI_ERR_IN_STATUS           17      /* Look in status for error value */
//MPI_ERR_PENDING             18      /* Pending request */
//
///* New MPI-2 Error classes */
//MPI_ERR_ACCESS      20      /* */
//MPI_ERR_AMODE       21      /* */
//MPI_ERR_BAD_FILE    22      /* */
//MPI_ERR_CONVERSION  23      /* */
//MPI_ERR_DUP_DATAREP 24      /* */
//MPI_ERR_FILE_EXISTS 25      /* */
//MPI_ERR_FILE_IN_USE 26      /* */
//MPI_ERR_FILE        27      /* */
//MPI_ERR_IO          32      /* */
//MPI_ERR_NO_SPACE    36      /* */
//MPI_ERR_NO_SUCH_FILE 37     /* */
//MPI_ERR_READ_ONLY   40      /* */
//MPI_ERR_UNSUPPORTED_DATAREP   43  /* */

template < typename T >
class Error {
	private:
		T _value;
	public:
		Error() :
			_value( MPI_SUCCESS )
		{
		}

		bool success() const
		{
			return _value == MPI_SUCCESS;
		}
};

} // namespace mpi
} // namespace nanos

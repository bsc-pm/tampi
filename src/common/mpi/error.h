
#ifndef ERROR_H
#define ERROR_H

#include <stdexcept>
#include <string>

#include <mpi.h>

namespace nanos {
namespace mpi {

class mpi_error : public std::runtime_error
{
    private:
        static std::string get_error_string( int code )
        {
            int length = 0;
            std::string result( MPI_MAX_ERROR_STRING, '\0' );
            MPI_Error_string( code, &result.front(), &length );
            result.resize( length );
            return result;
        }

    public:
        mpi_error( int code ) :
            std::runtime_error( get_error_string(code) )
        {
        }
};

inline void check_error( int error_code )
{
#ifndef NDEBUG
    if( error_code != MPI_SUCCESS ) {
        throw mpi_error(error_code);
    }
#endif
}

} // namespace mpi
} // namespace nanos

#endif // ERROR_H


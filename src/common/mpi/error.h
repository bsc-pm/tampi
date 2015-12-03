
#ifndef ERROR_H
#define ERROR_H

#include <mpi.h>

#include <string>

namespace nanos {
namespace mpi {

class Error {
	private:
		int _value;

	public:
		Error() :
			_value( MPI_SUCCESS )
		{
		}

		Error( int value ) :
			_value( value )
		{
		}

		Error( Error const& other ) = default;

		Error& operator=( Error const& other ) = default;

		Error& operator=( int other )
		{
			_value = other;
			return *this;
		}

		bool success() const
		{
			return _value == MPI_SUCCESS;
		}

		std::string toString() const
		{
			return Error::toString( _value );
		}

		int getErrorCode() const
		{
			return _value;
		}

		int value() const
		{
			return _value;
		}

		static std::string toString( int code )
		{
			int len;
			char description[MPI_MAX_ERROR_STRING];

			int err = MPI_Error_string( code, description, &len );

			return std::string( description, len );
		}
};

} // namespace mpi
} // namespace nanos

#endif // ERROR_H


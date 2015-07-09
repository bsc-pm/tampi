#ifndef STATUS_H
#define STATUS_H

#include <array>
#include <cstring>
#include <iterator>
#include <vector>

#include "mpicommon.h"
#include "mpi_impl.h"

namespace nanos {
namespace mpi {

inline bool ignore( MPI_Status *status ) {
    return status == MPI_STATUS_IGNORE;
}

inline bool ignore( MPI_Status array_of_statuses[], size_t nelem ) {
    return array_of_statuses == MPI_STATUSES_IGNORE;
}

inline bool ignore( MPI_Fint *status ) {
    return status == MPI_F_STATUS_IGNORE;
}

inline bool ignore( MPI_Fint array_of_statuses[][SIZEOF_MPI_STATUS], size_t nelem ) {
    return ((MPI_Fint*)array_of_statuses) == MPI_F_STATUSES_IGNORE;
}

template < typename T, size_t fixed_size >
class Container : public std::array<T,fixed_size> {
public:
    Container() {}
    Container( size_t len ) {}
    Container( Container<T,fixed_size> const& c ) { std::copy( c.begin(), c.end(), this->begin() ); }
    size_t capacity() const { return fixed_size; }
};

template < typename T >
class Container<T,0> : public std::vector<T> {
    typedef typename std::vector<T> super;
public:
    Container() : super() {}
    Container( size_t len ) : super() { super::reserve(len); }
    Container( Container<T,0> const& c ) : super(c) {}
};

template < typename status_type, size_t fixed_size = 0 >
class Statuses {
    Container<status_type,fixed_size> values;
public:
    Statuses() : values() {}
    Statuses( size_t len ) : values( len ) {}
    Statuses( Statuses const& s ) : values( s.values.size() ) { std::copy( s.values.begin(), s.values.end(), values.begin() ); }
    virtual ~Statuses() {}

    status_type &operator[]( size_t pos ) { return values[pos]; }

    status_type *data() { return values.data(); }

    void copy( status_type *status )
    {
        assert( !values.empty() );
        // copy last element
        if( !ignore( status ) )
            *status = values.back();
    }

    void copy( status_type array_of_statuses[], size_t nelems )
    {
        assert( nelems <= values.size() );
        if( !ignore_all( array_of_statuses, nelems ) )
            std::copy( values.end()-nelems, values.end(), std::begin(*array_of_statuses) );
    }
};

template < size_t fixed_size >
class Statuses< Fortran::status_type, fixed_size > {
    typedef Fortran::status_type status_type;

    Container<status_type,fixed_size> values;
public:
    Statuses() : values() {}
    Statuses( size_t len ) : values(len) {}
    Statuses( Statuses const& s ) : values() { std::copy( s.values.begin(), s.values.end(), values.begin() ); }
    virtual ~Statuses() {}

    status_type &operator[]( size_t pos ) { return values[pos]; }

    status_type *data() { return values.data(); }

    void copy( MPI_Fint *status )
    {
        // copy last element
        if( !ignore( status ) ) {
            assert( !values.empty() );
            std::memcpy(status, &values.back(), sizeof(status_type));
        }
    }

    void copy( status_type array_of_statuses[], size_t nelems )
    {
        if( !ignore( array_of_statuses, nelems ) ) {
            assert( nelems <= values.size() );
            std::copy( values.end()-nelems, values.end(), std::begin(*array_of_statuses) );
        }
    }
};

template < typename request_type, size_t fixed_size = 0 >
using Requests = Container<request_type, fixed_size>;

} // namespace mpi
} // namespace nanos

#endif // STATUS_H

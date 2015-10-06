/*
 * Copyright (c) 2015 Barcelona Supercomputing Center. All rights
 *                    reserved.
 *
 * This file is part of OmpSs-MPI interoperability library.
 *
 * OmpSs-MPI interop. library is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef STATUS_H
#define STATUS_H

#include <array>
#include <cstring>
#include <iterator>
#include <vector>

#include "mpicommon.h"
#include "mpi_impl.h"

/*! \file status.h
  This file provides helper functions to abstract the differencies between
  specification of C and Fortran languages in MPI data structs.
 */

namespace nanos {
namespace mpi {

//! \return A boolean indicating if status pointer must be ignored or not.
inline bool ignore( MPI_Status *status ) {
    return status == MPI_STATUS_IGNORE;
}

//! \return A boolean indicating if status pointer must be ignored or not.
inline bool ignore( MPI_Status array_of_statuses[], size_t nelem ) {
    return array_of_statuses == MPI_STATUSES_IGNORE;
}

//! \return A boolean indicating if status pointer must be ignored or not.
inline bool ignore( MPI_Fint *status ) {
    return status == MPI_F_STATUS_IGNORE;
}

//! \return A boolean indicating if status pointer must be ignored or not.
inline bool ignore( MPI_Fint array_of_statuses[][SIZEOF_MPI_STATUS], size_t nelem ) {
    return ((MPI_Fint*)array_of_statuses) == MPI_F_STATUSES_IGNORE;
}

//! Represents a container class with a given size.
/*!
  Depending if the size of a structure is known in compile time or not,
  this class lets us choose between an array or a vector easily.
  \param T type of the element to be stored
  \param If value is not 0, creates a fixed size container (array) for
         that size. Otherwise it creates a dynamic contiguous container
         (vector).
 */
template < typename T, size_t fixed_size >
class Container : public std::array<T,fixed_size> {
public:
    //! Default constructor.
    Container() {}

    //! Creates a container with an initial size.
    /*!
      This constructor is used to keep the same interface as
      the vector implementation version of Container, to avoid
      errors in a compillation phase.
     */
    Container( size_t len ) { assert( len == fixed_size ); }

    //! Copy constructor.
    Container( Container<T,fixed_size> const& c ) { std::copy( c.begin(), c.end(), this->begin() ); }

    //! Returns the number of elements inside the container.
    size_t capacity() const { return fixed_size; }
};

//! Represents a container class with a given size.
/*!
  Depending if the size of a structure is known in compile time or not,
  this class lets us choose between an array or a vector easily.
  It is also designed for variable size (not known in compile time)
  \param T type of the element to be stored
 */
template < typename T >
class Container<T,0> : public std::vector<T> {
    using std::vector<T> = super;
public:
    //! Default constructor.
    Container() : super() {}

    //! Initializes as many items as requested.
    Container( size_t len ) : super() { super::reserve(len); }
    Container( Container<T,0> const& c ) : super(c) {}
};

//! Container generic specialization for MPI's Status data types.
/*!
  \param status_type MPI Status datatype (e.g., MPI_Status) that will be stored
  into the container.
 */
template < typename status_type, size_t fixed_size = 0 >
class Statuses {
    //! Contains one or many MPI Status.
    Container<status_type,fixed_size> values;
public:
    //! Default constructor.
    Statuses() : values() {}
    //! Initializes container with predefined number of people.
    Statuses( size_t len ) : values( len ) {}

    //! Copy constructor.
    Statuses( Statuses const& s ) : values( s.values.size() ) { std::copy( s.values.begin(), s.values.end(), values.begin() ); }

    //! Destructor.
    virtual ~Statuses() {}

    //! Returns the nth element of the container.
    status_type &operator[]( size_t pos ) { return values[pos]; }

    // Returns a pointer to the Status array.
    status_type *data() { return values.data(); }

    //! Copies the last element of the values array into a memory region.
    void copy( status_type *status )
    {
        assert( !values.empty() );
        if( !ignore( status ) )
            *status = values.back();
    }

    //! Copies N elements of the array into a memory region.
    void copy( status_type array_of_statuses[], size_t nelems )
    {
        assert( nelems <= values.size() );
        if( !ignore_all( array_of_statuses, nelems ) )
            std::copy( values.end()-nelems, values.end(), std::begin(*array_of_statuses) );
    }
};

//! Container specialization for MPI_Status data types.
/*!
  Specialization for Fortran language
 */
template < size_t fixed_size >
class Statuses< Fortran::status_type, fixed_size > {
    using Fortran::status_type = status_type;

    Container<status_type,fixed_size> values;
public:
    //! Default constructor.
    Statuses() : values() {}
    //! Reserves enough space for dynamic memory containers (vector).
    Statuses( size_t len ) : values(len) {}

    Statuses( Statuses const& s ) : values() { std::copy( s.values.begin(), s.values.end(), values.begin() ); }

    //! Default destructor.
    virtual ~Statuses() {}

    //! Index access operator.
    status_type &operator[]( size_t pos ) { return values[pos]; }

    //! Returns the contiguous memory pointer that holds object's value
    status_type *data() { return values.data(); }

    //! Copies the last element of the values array into a memory region.
    void copy( MPI_Fint *status )
    {
        // copy last element
        if( !ignore( status ) ) {
            assert( !values.empty() );
            std::memcpy(status, &values.back(), sizeof(status_type));
        }
    }

    //! Copies N elements of the array into a memory region.
    void copy( status_type array_of_statuses[], size_t nelems )
    {
        if( !ignore( array_of_statuses, nelems ) ) {
            assert( nelems <= values.size() );
            std::copy( values.end()-nelems, values.end(), std::begin(*array_of_statuses) );
        }
    }
};

//! Container specialization for MPI Requests
/*!
  It is actually a 'rename' since MPI requests are usually represented
  by an integer.
 */
template < typename request_type, size_t fixed_size = 0 >
using Requests = Container<request_type, fixed_size>;

} // namespace mpi
} // namespace nanos

#endif // STATUS_H

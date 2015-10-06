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
#ifndef FLAG_H
#define FLAG_H

//! Represents a flag using an integral number
/*!
  Flag provides implicit conversion operators and constructors
  to make conversions between integers and booleans, so that they
  can be used on an if/else construct.
  Possible values are:\n
   - false: 0
   - true: any number but 0
  
  \param IntType an integer type, usually int and MPI_Fint
 */
template <typename IntType>
struct Flag {
   //! Holds the value of the flag
   IntType value;
public:
   //! Default constructor. Constructs a flag with a 'false' value
   Flag() : value(0) {}

   //! Constructs a flag using a reference to an integer
   Flag( IntType const& v ) : value(v) {}

   //! Constructs a flag using a boolean
   /*!
     Stores 1 if boolean is true. Otherwise stores a 0.
    */
   Flag( bool b ) : value( b? 1: 0 ) {}

   //! Copy constructor
   Flag( Flag const& f ) : value(f.value) {}

   //! Implicit conversion to pointer
   /*!
     Allows conversion from a flag to a pointer.
     \return a pointer to the integer value
    */
   operator IntType*() { return &value; }

   //! Implicit conversion to reference
   /*!
     Allows conversion from a flag to an integer reference.
     \return a reference to the integer value
    */
   operator IntType const&() const { return value; }

   //! Implicit conversion to boolean
   /*!
     Allows conversion from a flag to a boolean, so that
     the object can be directly used inside if/else conditionals.
    \return a reference to the integer value
    */
   operator bool() const { return value != 0; }
};

#endif // FLAG_H

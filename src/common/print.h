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
#ifndef PRINT_H
#define PRINT_H

#include <mpi.h>
#include <iostream>
#include <sstream>

#ifdef HAVE_NANOX_NANOS_H
   #include <nanox/nanos.h>
#endif

/*! \file print.h
  Implements several functions that print debug information such as
  human readable description of MPI errors and runtime debug message
  printing helpers.
 */

namespace nanos {
namespace log {
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
   inline void message( const Ts&... msg )
   {
      std::ostream::sentry sentry(std::cout);
      join( std::cout, msg... );
   }

   template <typename...Ts>
   inline void debug( const Ts&... msg )
   {
#ifdef DEBUG_ENABLED
      std::ostream::sentry sentry(std::cout);
      join( std::cout, msg... );
#endif
   }

   inline void fatal( const char* msg ) {
      message("[MPI interoperability] Fatal error: ", msg);
      std::terminate();
   }

   inline void intercepted_call( const char* function_name )
   {
      debug( "[MPI interoperability] Debug: Intercepted ", function_name );
   }

} // namespace log
} // namespace nanos

#endif // PRINT_H

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
   #include <nanos.h>
#endif

/*! \file print.h
  Implements several functions that print debug information such as
  human readable description of MPI errors and runtime debug message
  printing helpers.
 */

namespace print {

#ifdef HAVE_NANOX_NANOS_H
    inline void nanos_debug( const char *message )
    {
        ::nanos_debug( message );
    }
#else
    inline void nanos_debug( const char *message )
    {
    }
#endif

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

    inline void intercepted_call( const char* function_name )
    {
       dbg( "[MPI async. overload library] Intercepted ", function_name );
    }

    /* TODO: try to detect at configure time which nanos version is being used 
       (nanos_get_mode api call) and enable define preprocessor flags acordingly.
     */
}

#endif // PRINT_H

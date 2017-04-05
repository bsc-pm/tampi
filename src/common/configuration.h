/*
 * Copyright (c) 2017 Barcelona Supercomputing Center. All rights
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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "spin_mutex.h"

#include <mutex>

namespace nanos {
namespace mpi {

class Configuration {
private:
   int                  _level;
   mutable spin_mutex   _mutex;

public:
   bool disabled() const { return _level == 0; }
   bool tlsTuneDisabled() const { return disabled() || _level > 2; }

   Configuration() = default; // All enabled by default

   Configuration( int level ) :
      _level( level )
   {
   }

   void reset( int level ) {
      if( level != _level ) {
         std::lock_guard<spin_mutex> guard( _mutex );

         _level = level;
      }
   }
};

extern Configuration config;

} // namespace nanos
} // namespace mpi

#endif


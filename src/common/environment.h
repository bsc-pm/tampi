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
#ifndef NANOS6_ENVIRONMENT_H
#define NANOS6_ENVIRONMENT_H

#include "ticketqueue.h"

#ifdef HAVE_NANOX_NANOS_H
#include <nanox/nanos.h>
#endif

#ifdef HAVE_NANOS6_H
#include <nanos6.h>
#endif

#include <atomic>

namespace nanos {
namespace mpi {

namespace detail {

template < typename Queue >
class environment {
public:
   static void initialize()
   {
      _queue = new Queue();
      nanos_register_polling_service("MPI INTEROPERABILITY", environment::poll, _queue);
   }

   static void finalize()
   {
      nanos_unregister_polling_service("MPI INTEROPERABILITY", environment::poll, _queue);
      delete _queue;
   }

   static int poll( void* data )
   {
      _queue->poll();
      return 0;
   }

   static Queue& getQueue()
   {
      return *_queue;
   }

private:
   static Queue* _queue;
};

} // namespace detail

namespace C {
   typedef detail::environment<TicketQueue<C::Ticket> >       environment;
}

namespace Fortran {
   typedef detail::environment<TicketQueue<Fortran::Ticket> > environment;
}

} // namespace mpi
} // namespace nanos

#endif //NANOS6_ENVIORNMENT_H

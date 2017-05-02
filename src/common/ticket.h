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
#ifndef TICKET_H
#define TICKET_H

#include "ticket_decl.h"

#include "environment.h"

#ifdef HAVE_EXTRAE_H
#include <extrae.h>
#endif

namespace nanos {
namespace mpi {

namespace C {

inline Ticket::Ticket( Request& r, Status* s  ) :
   TicketBase(1),
   _first_status(s)
{
   environment::getQueue().add(*this, r);
}

inline Ticket::Ticket( Request* first, Request* last, Status* first_status ) :
   TicketBase( std::distance(first, last) ),
   _first_status(first_status)
{
   environment::getQueue().add(*this, first, last);
}

inline void Ticket::wait()
{
   if( spinNotYield() ) {
      TicketQueue<Ticket>& queue = environment::getQueue();
      while( !finished() ) {
         queue.poll();
      }
   } else if( !finished() ) {
      blockTask();
   }
}

} // namespace C

namespace Fortran {

inline Ticket::Ticket( Request& r, MPI_Fint* s ) :
   TicketBase(1),
   _first_status(s)
{
   environment::getQueue().add(*this, r);
}

inline Ticket::Ticket( Request* first, Request* last, MPI_Fint* first_status ) :
   TicketBase( std::distance(first, last) ),
   _first_status(first_status)
{
   environment::getQueue().add(*this, first, last);
}

inline void Ticket::wait()
{
   if( spinNotYield() ) {
      TicketQueue<Ticket>& queue = environment::getQueue();
      while( !finished() ) {
         queue.poll();
      }
   } else if( !finished() ) {
      blockTask();
   }
}

} // namespace Fortran

} // namespace mpi
} // namespace nanos

#endif // TICKET_H


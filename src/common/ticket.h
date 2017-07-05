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
#include "print.h"

#ifdef HAVE_EXTRAE_H
#include <extrae.h>
#endif

namespace nanos {
namespace mpi {

namespace C {

inline Ticket::Ticket( Request& r, Status *s  ) :
   TicketBase(),
   _first_status(s)
{
   environment::getQueue().add(*this, r);
}

inline Ticket::Ticket( util::array_view<Request> t_requests, Status *first_status ) :
   TicketBase(),
   _first_status(first_status)
{
   environment::getQueue().add(*this, t_requests);
}

inline void Ticket::wait()
{
   TicketQueue<Ticket>& queue = environment::getQueue();
   while( spinNotYield() && !finished() ) {
      queue.poll();
   }
   if( !finished() ) {
      nanos::log::verbose_output( "blocking task" );
      blockTask();
      nanos::log::verbose_output( "unblocking task" );
   }
}

} // namespace C

namespace Fortran {

inline Ticket::Ticket( Request& r, MPI_Fint* s ) :
   TicketBase(),
   _first_status(s)
{
   environment::getQueue().add(*this, r);
}

inline Ticket::Ticket( util::array_view<Request> t_requests, MPI_Fint* first_status ) :
   TicketBase(),
   _first_status(first_status)
{
   environment::getQueue().add(*this, t_requests);
}

inline void Ticket::wait()
{
   TicketQueue<Ticket>& queue = environment::getQueue();
   while( spinNotYield() && !finished() ) {
      queue.poll();
   }
   if( !finished() ) {
      nanos::log::verbose_output( "blocking task" );
      blockTask();
      nanos::log::verbose_output( "unblocking task" );
   }
}

} // namespace Fortran

} // namespace mpi
} // namespace nanos

#endif // TICKET_H


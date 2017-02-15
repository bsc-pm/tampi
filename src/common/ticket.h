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

inline Ticket::Ticket( Request& r ) :
   TicketBase(r)
{
   environment::getQueue().add(*this, r);
}

inline Ticket::Ticket( Request* first, Request* last ) :
   TicketBase(first, last)
{
   environment::getQueue().add(*this, first, last);
}

} // namespace C

namespace Fortran {

inline Ticket::Ticket( Request& r ) :
   TicketBase(r)
{
   environment::getQueue().add(*this, r);
}

inline Ticket::Ticket( Request* first, Request* last ) :
   TicketBase(first, last)
{
   environment::getQueue().add(*this, first, last);
}

} // namespace Fortran

} // namespace mpi
} // namespace nanos

#endif // TICKET_H


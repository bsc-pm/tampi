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
   TicketBase(r),
   _first_status(s)
{
   environment::getQueue().add(*this, r);
}

inline Ticket::Ticket( Request* first, Request* last, Status* first_status ) :
   TicketBase(first, last),
   _first_status(first_status)
{
   environment::getQueue().add(*this, first, last);
}

} // namespace C

namespace Fortran {

inline Ticket::Ticket( Request& r, MPI_Fint* s ) :
   TicketBase(r),
   _first_status(s)
{
   environment::getQueue().add(*this, r);
}

inline Ticket::Ticket( Request* first, Request* last, MPI_Fint* first_status ) :
   TicketBase(first, last),
   _first_status(first_status)
{
   environment::getQueue().add(*this, first, last);
}

} // namespace Fortran

} // namespace mpi
} // namespace nanos

#endif // TICKET_H


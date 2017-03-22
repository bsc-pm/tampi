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

inline bool Ticket::ignoreStatus() const
{
   return _first_status == MPI_STATUS_IGNORE | _first_status == MPI_STATUSES_IGNORE;
}

inline void Ticket::notifyCompletion( int status_pos, const Status& status )
{
   if( !ignoreStatus() ) {
      const Status* first = &status;
      const Status* last  = std::next(first);
      Status* out   = _first_status;
      std::copy<const Status*,Status*>( first, last, out );
   }
   notifyCompletion();
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

inline bool Ticket::ignoreStatus() const
{
   // MPI fortran API decays status array to a pointer to integer
   return _first_status == MPI_F_STATUS_IGNORE | _first_status == MPI_F_STATUSES_IGNORE;
}

inline void Ticket::notifyCompletion( int status_pos, const Status& status )
{
   if( !ignoreStatus() ) {
      const Status* first = &status;
      const Status* last  = std::next(first);
      Status* out   = reinterpret_cast<Status*>(_first_status);
      std::copy<const Status*,Status*>( first, last, out );
   }
   notifyCompletion();
}

} // namespace Fortran

} // namespace mpi
} // namespace nanos

#endif // TICKET_H


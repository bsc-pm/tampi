#ifndef TICKET_H
#define TICKET_H

#include "ticket_decl.h"

#include "environment.h"

namespace nanos {
namespace mpi {

inline Ticket::Ticket( MPI_Request req ) :
   _task( nanos_get_current_task() ),
   _pending( 0 )
{
   int completed;
   int err = MPI_Test( &req, &completed, MPI_STATUS_IGNORE );
   if( !completed ) {
      _pending = 1;
      environment::getQueue().add(*this, req);
   }

   printf("Create ticket %p, pending: %d\n", this, _pending);
}

// typedef MPI_Request* InputIt
inline Ticket::Ticket( MPI_Request* first, MPI_Request* last ) :
   _task( nanos_get_current_task() ),
   _pending( 0 )
{
   int size = std::distance(first,last);
   int completed = 0;
   int indices[size];
   int err = MPI_Testsome( size, first, indices, &completed, MPI_STATUSES_IGNORE );

   _pending = size - completed;
   if( !finished() ) {
      environment::getQueue().add(*this, first, last);
   }
   printf("Create ticket %p, pending: %d\n", this, _pending);
}

inline void Ticket::wait() {
   environment::getQueue().poll();

   printf("Waiting for ticket %p, pending: %d\n", this, _pending);
   if( !finished() ) { 
      printf("Call nanos_block_current_task() for ticket %p\n", this);
      nanos_block_current_task();
   }
}

} // namespace mpi
} // namespace nanos

#endif // TICKET_H


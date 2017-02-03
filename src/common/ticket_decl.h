#ifndef TICKET_DECL_H
#define TICKET_DECL_H

#include "print.h"

#ifdef HAVE_NANOX_NANOS_H
#include <nanox/nanos.h>
#define nanos_get_current_task nanos_current_wd
#endif

#ifdef HAVE_NANOS6_H
#include <nanos6.h>
typedef void* nanos_wd_t;
#endif

namespace nanos {
namespace mpi {

class Ticket {
private:
   nanos_wd_t _task;
   int        _pending;

public:
   Ticket( MPI_Request req );

   // Only participates in overload resolution if iterators to MPI_Request types are provided.
   // typedef MPI_Request* InputIt
   Ticket( MPI_Request* first, MPI_Request* last );

   Ticket( const Ticket & gt ) = delete;

   Ticket& operator=( const Ticket & gt ) = delete;

   ~Ticket() {
      printf("Destroy ticket %p\n", this);
      if( _pending > 0 )
         log::fatal( "Destroying uncompleted ticket" );
   }

   void notifyCompletion( int num = 1 ) {
      assert( _pending >= num );
      _pending -= num;
      printf("Notify ticket %p, pending: %d\n", this, _pending);
      if( _pending == 0 )
         nanos_unblock_task(_task);
   }

   bool finished() const {
      return _pending == 0;
   }

   void wait();
};

} // namespace mpi
} // namespace nanos

#endif // TICKET_DECL_H


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

class environment {
public:
   static void initialize()
   {
      _queue = new TicketQueue();
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

   static TicketQueue& getQueue()
   {
      return *_queue;
   }

private:
   static TicketQueue* _queue;
};

} // namespace mpi
} // namespace nanos

#endif //NANOS6_ENVIORNMENT_H

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
		TicketQueue::_queue = new TicketQueue();
		nanos_register_polling_service("MPI INTEROPERABILITY", environment::poll, &TicketQueue::get());
	}

	static void finalize()
	{
		nanos_unregister_polling_service("MPI INTEROPERABILITY", environment::poll, &TicketQueue::get());
		delete TicketQueue::_queue;
	}

  private:
	static int poll(void * data)
	{
		return TicketQueue::get().poll();
	}
};

} // namespace mpi
} // namespace nanos

#endif //NANOS6_ENVIORNMENT_H

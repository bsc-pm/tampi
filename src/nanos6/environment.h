#ifndef NANOS6_ENVIRONMENT_H
#define NANOS6_ENVIRONMENT_H

#include "ticketqueue.h"

#include <atomic>

/* Nanos6 runtime API */
extern "C" {
typedef int (*nanos_polling_service_t)(void *service_data);
void nanos_register_polling_service(char const *service_name, nanos_polling_service_t service_function, void *service_data);
void nanos_unregister_polling_service(char const *service_name, nanos_polling_service_t service_function, void *service_data);
} //extern C

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

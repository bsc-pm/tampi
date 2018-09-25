#ifndef RUNTIME_API_H
#define RUNTIME_API_H

extern "C" {
	/* Polling service API */
	typedef int (*nanos6_polling_service_t)(void *data);
	void nanos6_register_polling_service(char const *name, nanos6_polling_service_t service, void *data);
	void nanos6_unregister_polling_service(char const *name, nanos6_polling_service_t service, void *data);
	
	/* Pause/Resume API */
	void *nanos6_get_current_blocking_context();
	void nanos6_block_current_task(void *blocking_context);
	void nanos6_unblock_task(void *blocking_context);
	
	/* Events API */
	void *nanos6_get_current_event_counter();
	void nanos6_increase_current_task_event_counter(void *event_counter, unsigned int increment);
	void nanos6_decrease_task_event_counter(void *event_counter, unsigned int decrement);
	
	/* Additional API routines */
	int nanos6_in_serial_context();
}

#endif // RUNTIME_API_H


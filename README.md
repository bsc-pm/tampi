Interoperability library
========================


Application Programming Interface
---------------------------------

```
void *nanos_get_current_blocking_context();
```

Get a handler that allows to block and unblock the current task

Returns an opaque pointer that is used for blocking and unblocking
the current task.

The underlying implementation may or may not return the same value
for repeated calls to this function. Once the handler has been used once in a call to nanos_block_current_task
and a call to nanos_unblock_task, the handler is discarded and a new
one must be obtained to perform another cycle of blocking and unblocking.

```
void nanos_block_current_task(void *blocking_context);
```

Block the current task

The `blocking_context` parameter is the value returned by a call to nanos_get_current_blocking_context
performed within the task

This function blocks the execution of the current task at least until
a thread calls nanos_unblock_task with its blocking context

The runtime may choose to execute other tasks within the execution scope
of this call.

```
void nanos_unblock_task(void *blocking_context);
```

\brief Unblock a task previously or about to be blocked

Mark as unblocked a task previously or about to be blocked inside a call
to nanos_block_current_task.

While this function can be called before the actual to nanos_block_current_task,
only one call to it may precede its matching call to nanos_block_current_task.

The return of this function does not guarantee that the blocked task has
resumed yet its execution. It only guarantees that it will be resumed.

blocking_context the handler used to block the task



```
typedef int (*nanos_polling_service_t)(void *service_data);
```

Function that the runtime calls periodically

service_data a pointer to data that the function uses and that
also identifies an instance of this service registration

true to signal that the purpose of the function has been
achieved and that the function should not be called again with the given
service_data


```
void nanos_register_polling_service(char const *service_name, nanos_polling_service_t service_function, void *service_data);
```

Register a function and parameter of that function that the runtime
must call periodically to perform operations in a non-blocking fashion

Registers a function that the runtime will called periodically. The  purpose
is to support operations in a non-blocking way. For instance to check for
certain events that are not possible to check in a blocking way, or that it
is not desirable to do so.

The service_data parameter is an opaque pointer that is passed to the function
as is.

The same function can be registered several times with different service_data
parameters, and each combination will be considered as a different
registration. That is, a registered service instance consists of a service
function and a specific service_data.

The service will remain registered as long as the function returns false and
the service has not been explicitly deregistered through a call to
nanos_unregister_polling_service.

   * service_name a string that identifies the kind of service that will
be serviced
   * service the function that the runtime should call periodically
   * service_data an opaque pointer to data that is passed to the service
function

```
void nanos_unregister_polling_service(char const *service_name, nanos_polling_service_t service_function, void *service_data);
```

Unregister a function and parameter of that function previously
registered through a call to nanos_register_polling_service.

Unregister a service instance identified by the service_function and
service_data previously registered through a call to
nanos_register_polling_service. The service_function must not have returned
true when called with the given service_data, since that return value is
equivalent to a call to this function.

   * service_name a string that identifies the kind of service
   * service the function that the runtime should stop calling periodically
     with the given service_data
   * service_data an opaque pointer to the data that was passed to the service
     function


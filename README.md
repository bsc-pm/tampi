Interoperability library
========================


Block/unblock API
-----------------

```c
void *nanos_get_current_blocking_context();
```
Returns an opaque pointer that is used for blocking and unblocking the current
task.

The underlying implementation may or may not return the same value for repeated
calls to this function. Once the handler has been used once in a call to
`nanos_block_current_task()` and a call to `nanos_unblock_task()`, the handler
is discarded and a new one must be obtained to perform another cycle of
blocking and unblocking.


```c
void nanos_block_current_task(void *blocking_context);
```
This function blocks the execution of the current task at least until a thread
calls `nanos_unblock_task()` with its blocking context The runtime may choose
to execute other tasks within the execution scope of this call.

The `blocking_context` parameter is the value returned by a call to
`nanos_get_current_blocking_context()` performed within the task.


```c
void nanos_unblock_task(void *blocking_context);
```
Unblocks a task previously (or about to be) blocked inside a call to
`nanos_block_current_task()`.  While this function can be called before the
actual to `nanos_block_current_task()`, only one call to it may precede its
matching call to `nanos_block_current_task()`.  The return of this function
does not guarantee that the blocked task has resumed yet its execution. It only
guarantees that it will be resumed.

The `blocking_context` parameter is the handler used (or about to be used) to
block the task.


Polling API
-----------

```c
typedef int (* nanos_polling_service_t)(void *data);
```
This is the type signature of the function that the runtime will call
periodically. The function receives a `void *` parameter (i.e. `data`) that
also identifies an instance of this service registration.

The function returns `true` to signal that the purpose of the function has been
achieved and that the function should not be called again with the given `data`


```c
void nanos_register_polling_service(char const *name, nanos_polling_service_t service, void *data);
```

Registers a `service` and its `data` parameter that the runtime must call
periodically to perform operations in a non-blocking fashion.  The  purpose is
to support operations in a non-blocking way. For instance to check for certain
events that are not possible to check in a blocking way, or that it is not
desirable to do so.

The `name` parameter is a string that identifies the kind of service that will
be serviced

The `service` parameter is the function (following the
`nanos_pollint_service_t` type signature) that the runtime should call
periodically

The `data` parameter is an opaque pointer that is passed to the function as is.

Same function can be registered several times with different `data` parameters,
and each combination will be considered as a different registration (i.e. a
`service-data` tuple).  The `service-data` tuple will remain registered as long
as the function returns `false` and the tuple has not been explicitly
deregistered through a call to `nanos_unregister_polling_service()`.


```c
void nanos_unregister_polling_service(char const *name, nanos_polling_service_t service, void *data);
```
Unregisters a `service-data` tuple (this is a `service` and `data` combination)
previously registered  through a call to `nanos_register_polling_service()`.

The `service-data` tuple must not have previously `true` in any of its previous
invocations, since that return value is equivalent to a call to the
`nanos_unregister_polling_service()`.

The `name` parameter is a string that identifies the kind of service.

The `service` parameter is the function that the runtime should stop calling periodically
with the given parameter `data`,

The `data` parameter is an opaque pointer to the data that was passed to the service
function.


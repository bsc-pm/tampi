# Task-Aware MPI Library

Task-Aware MPI is a library which targets hybrid applications (MPI+OmpSs) and
improves the interoperability between these two programming models. Normally,
these applications taskify computation and MPI communication. Thus, computation
and communication can overlap. This mechanism allows the execution of other ready
tasks when a communication task is blocked inside MPI.

This library intercepts each blocking MPI call (e.g. `MPI_Recv`) and converts
it to a call to the non-blocking function of the same operation (e.g. `MPI_Irecv`).
The resulting MPI requests are stored and managed internally. Then, the calling
task is blocked through the block/unblock API. Furthermore, thanks to the polling API,
these pending requests are checked periodically to find out which of them are
complete. When all the requests of a task are complete, the task is resumed,
and finally, it can return from the MPI call.

See the section [API Description](#api-description) for more information about the
required APIs.

## Installation
### Build requirements

To install Task-Aware MPI, the following tools and libraries must be installed:

+ automake, autoconf, libtool, make and a C and C++ compiler
+ [OmpSs](https://pm.bsc.es): Nanox or Nanos6 runtime
+ MPI with multi-threading support

### Optional libraries and tools

In addition to the build requirements, the following libraries and tools are optional:

+ [Extrae](https://tools.bsc.es/extrae)

### Build procedure

Task-Aware MPI uses the standard GNU automake and libtool toolchain.
When cloning from a repository, the building environment must be prepared through the following command:

```sh
$ autoreconf -fiv
```

When the code is distributed through a tarball, it usually does not need that command.

Then execute the following commands:

```sh
$ ./configure --prefix=INSTALLATION_PREFIX ...other options...
$ make
$ make install
```

where `INSTALLATION_PREFIX` is the directory into which to install Nanos6.

The configure script accepts the following options:

+ `--with-ompss=prefix` to specify the prefix of the OmpSs installation
+ `--with-runtime=nanox|nanos6` to specify which OmpSs runtime should be used
+ `--with-extrae=prefix` to specify the prefix of the Extrae installation

**NOTE:** An MPI installation with multi-threading support must be available when
configuring the library. The PATH environment variable should contain the path to
the binaries of MPI (i.e. by executing `export PATH=/path/to/mpi/bin:$PATH`).

## Execution

The user application must be linked to the generated library `libmpiompss-interop`. To
enable the interoperability mechanism, the user have to define the `MPI_TASK_MULTIPLE`
threading level, which is the next level of the `MPI_THREAD_MULTIPLE` (i.e. `MPI_THREAD_MULTIPLE+1`).
In the MPI initialization call (i.e. `MPI_Init_thread`), the user has to request this
new threading level.

For example, a valid initialization of the interoperability mechanism could be the following one:
```c
//...
#include <mpi.h>

#define MPI_TASK_MULTIPLE (MPI_THREAD_MULTIPLE+1)

int main(int argc, char **argv)
{
	int provided;
	MPI_Init_thread(&argc, &argv, MPI_TASK_MULTIPLE, &provided);
	if (provided != MPI_TASK_MULTIPLE) {
		fprintf(stderr, "Error: MPI_TASK_MULTIPLE not supported!");
		return 1;
	}
	//...
}
```

Once added these changes, the application is ready to run with the interoperability
mechanism enabled. The application should be launched with the same commands as in
the original hybrid version.

## API description

This section describes the required APIs that the OmpSs runtime has to provide.

### Block/unblock API

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

* The `blocking_context` parameter is the value returned by a call to
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

* The `blocking_context` parameter is the handler used (or about to be used) to
block the task.


### Register/Unregister Polling Service API

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

* The `name` parameter is a string that identifies the kind of service that will
be serviced

* The `service` parameter is the function (following the
`nanos_pollint_service_t` type signature) that the runtime should call
periodically

* The `data` parameter is an opaque pointer that is passed to the function as is.

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

* The `name` parameter is a string that identifies the kind of service.

* The `service` parameter is the function that the runtime should stop calling periodically
with the given parameter `data`,

* The `data` parameter is an opaque pointer to the data that was passed to the service
function.


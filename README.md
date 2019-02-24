# Task-Aware MPI Library

The Task-Aware MPI or TAMPI library extends the functionality of standard MPI libraries
by providing new mechanisms for improving the interoperability between parallel task-based
programming models, such as OpenMP or OmpSs-2, and both blocking and non-blocking MPI
operations.

By following the MPI Standard, programmers must pay close attention to avoid deadlocks
that may occur in hybrid applications (e.g., MPI+OpenMP) where MPI calls take place inside
tasks. This is given by the out-of-order execution of tasks that consequently alter the
execution order of the enclosed MPI calls. The TAMPI library ensures a deadlock-free
execution of such hybrid applications by implementing a cooperation mechanism between the
MPI library and the parallel task-based runtime system.

TAMPI is compatible with mainstream MPI implementations that support the MPI_THREAD_MULTIPLE
threading level in order to provide its task-aware features. TAMPI provides two main mechanisms:
the blocking mode that targets blocking MPI operations, and the non-blocking mode that focuses
on non-blocking MPI operations.


## Blocking Mode

The blocking mode of TAMPI is enabled by a new level of thread support named MPI_TASK_MULTIPLE,
which is monotonically greater than the MPI_THREAD_MULTIPLE threading level. The blocking mode
prevents the underlying hardware thread, which executes a task that calls blocking MPI
operations, from blocking inside the MPI library. It allows the thread to run other tasks
instead, while the operation does not complete. This mechanism leverages the standard MPI
interception techniques that enable transparent interception of MPI calls performed by
an application.

User applications can activate the TAMPI's blocking mode by requesting the MPI_TASK_MULTIPLE
threading level, which is defined in the `TAMPI.h` header (`TAMPIf.h` for Fortran). A valid
and safe usage of TAMPI's blocking mode is shown in the following example:

```c
#include <mpi.h>
#include <TAMPI.h>

int main(int argc, char **argv)
{
	int provided;
	MPI_Init_thread(&argc, &argv, MPI_TASK_MULTIPLE, &provided);
	if (provided != MPI_TASK_MULTIPLE) {
		fprintf(stderr, "Error: MPI_TASK_MULTIPLE not supported!");
		return 1;
	}
	
	int *data = (int *) malloc(N * sizeof(int));
	//...
	
	if (rank == 0) {
		for (int n = 0; n < N; ++n) {
			#pragma oss task in(data[n]) label(T1)
			MPI_Ssend(&data[n], 1, MPI_INT, 1, n, MPI_COMM_WORLD);
		}
	} else if (rank == 1) {
		for (int n = 0; n < N; ++n) {
			#pragma oss task out(data[n]) label(T2)
			{
				MPI_Status status;
				MPI_Recv(&data[n], 1, MPI_INT, 0, n, MPI_COMM_WORLD, &status);
				check_status(&status);
				fprintf(stdout, "data[%d] = %d\n", n, data[n]);
			}
		}
	}
	#pragma oss taskwait
	
	//...
}
```

In this example, the first MPI rank sends the `data` buffer of integers to the second rank
in messages of one single integer, while the second rank receives the integers and prints
them. On the one hand, the sender rank creates a task (T1) for sending each single-integer
MPI message with an input dependency on the corresponding position of the `data` buffer
(MPI_Ssend reads from `data`). Note that these tasks can run in parallel.

On the other hand, the receiver rank creates a task (T2) for receiving each integer.
Similarly, each task declares an output dependency on the corresponding position of the `data`
buffer (MPI_Recv writes on `data`). After receiving the integer, each task checks the status
of the MPI operation, and finally, it prints the received integer. These tasks can also
run in parallel.

This program would be incorrect when using the standard MPI_THREAD_MULTIPLE since it could
result in a deadlock situation depending on the task scheduling policy. Because of the
out-of-order execution of tasks in each rank, all available hardware threads could end up
blocked inside MPI, hanging the application due to the lack of progress. However, with TAMPI's
MPI_TASK_MULTIPLE threading level, hardware threads are prevented from blocking inside
blocking MPI calls and they can execute other ready tasks while communication tasks are
blocked, guaranteeing application progress.

See the articles listed in the [References](#references) section for more information.


## Non-blocking Mode

Task-Aware MPI also defines a non-blocking mode that consists of two additional functions
which improve the interoperability of non-blocking MPI operations and task-based programming
models. These functions are TAMPI_Iwait and TAMPI_Iwaitall, which have the same parameters
as their standard synchronous counterparts MPI_Wait and MPI_Waitall, respectively. These two
new are non-blocking asynchronous functions that bind the release of the calling task's
dependencies with the completion of the MPI requests passed as parameters. Once the task
finishes its execution, it will wait until all bound MPI requests are completed before
releasing its dependencies. Note that this approach requires both computation and communication
tasks to declare dependencies on the data buffers to guarantee a correct execution order.

This mode is orthogonal to the previous one and it only requires that MPI is initialized with
at least the MPI_THREAD_MULTIPLE threading level. If MPI is not initialized with at least this
standard level, any call to TAMPI_Iwait or TAMPI_Iwaitall will result in undefined behavior.

User applications can activate the TAMPI's non-blocking mode by requesting the standard
MPI_THREAD_MULTIPLE threading level. The `TAMPI.h` header (`TAMPIf.h` for Fortran) declares both
TAMPI_Iwait and TAMPI_Iwaitall functions. The following example shows a valid and safe usage of
this mode:

```c
#include <mpi.h>
#include <TAMPI.h>

int main(int argc, char **argv)
{
	int provided;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
	if (provided != MPI_THREAD_MULTIPLE) {
		fprintf(stderr, "Error: MPI_THREAD_MULTIPLE not supported!");
		return 1;
	}
	
	int *data = (int *) malloc(N * sizeof(int));
	//...
	
	// Must be alive during the execution of the communication tasks
	MPI_Status statuses[N];
	
	if (rank == 0) {
		for (int n = 0; n < N; ++n) {
			#pragma oss task in(data[n]) label(T1)
			{
				MPI_Request request;
				MPI_Issend(&data[n], 1, MPI_INT, 1, n, MPI_COMM_WORLD, &request);
				TAMPI_Iwait(&request, MPI_STATUS_IGNORE);
			}
		}
	} else if (rank == 1) {
		for (int n = 0; n < N; ++n) {
			#pragma oss task out(data[n], statuses[n]) label(T2)
			{
				MPI_Request request;
				MPI_Irecv(&data[n], 1, MPI_INT, 0, n, MPI_COMM_WORLD, &request);
				TAMPI_Iwaitall(1, &request, &statuses[n]);
				// Data buffer and status cannot be accessed yet!
			}
			
			#pragma oss task in(data[n], statuses[n]) label(T3)
			{
				check_status(&statuses[n]);
				fprintf(stdout, "data[%d] = %d\n", n, data[n]);
			}
		}
	}
	#pragma oss taskwait
	
	//...
}
```

The above code does the same as the previous example presented in [Blocking Mode](#blocking-mode)
but exclusively using the non-blocking mode. In this case, the sender rank creates tasks (T1) that
use the immediate version of the MPI_Ssend operation, they bind their release of dependencies to
the completion of the resulting request with TAMPI_Iwait, and they finish their execution "immediately".

The receiver rank instantiates tasks (T2) that use the immediate version of MPI_Recv and bind to themselves
the resulting request. In this case, they use the TAMPI_Iwaitall function to register the request and to
indicate where the status should be saved. Note that the status location must be alive after these tasks
finish their execution, so it cannot be stored in the task's stack. Each task must declare the output dependency
on the status position, in addition to the dependency on the `data` buffer. Since TAMPI_Iwait/TAMPI_Iwaitall
are non-blocking and asynchronous, the buffer and status cannot be consumed inside those tasks.

In order to consume the received data or the resulting status, the receiver can instantiate a task (T3)
with the corresponding dependencies. Once the request registered by a task T2 completes, the corresponding
task T3 will become ready, and it will be able to print the received integer and check the status of the
operation. Moreover, tasks T3 could be removed and the data or statuses could be consumed by the `main`
after the taskwait.

See the articles listed in the [References](#references) section for more information.


## Compatibility of Modes

Both blocking and non-blocking modes are entirely compatible so that they can operate in the same
application. For instance, some tasks could use the blocking mode, while others could use the non-blocking
one. Even a task can mix both modes. For example, it could issue some non-blocking MPI operations followed by a call
to TAMPI_Iwaitall, and then a call to various blocking MPI operations.


## Required APIs

Any parallel task-based programming model can support the TAMPI library by providing the
APIs described in the [src/common/RuntimeAPI.hpp](src/common/RuntimeAPI.hpp) source file.


## References

More information about this work can be found in the articles listed below. Citations to this work should reference these articles.

* Sala, K., Teruel, X., Perez, J. M., Peña, A. J., Beltran, V., & Labarta, J. (2018). **Integrating Blocking and Non-Blocking MPI Primitives with Task-Based Programming Models. Parallel Computing**. [Link to article](https://doi.org/10.1016/j.parco.2018.12.008).
* Sala, K., Bellón, J., Farré, P., Teruel, X., Perez, J. M., Peña, A. J., Holmes, D., Beltran. V., & Labarta, J. (2018, September). **Improving the interoperability between MPI and task-based programming models**. In Proceedings of the 25th European MPI Users' Group Meeting (p. 6). ACM. [Link to article](https://doi.org/10.1145/3236367.3236382).


# Getting Started

This section describes the software requirements of TAMPI, the building and installation process and how the library
can be used from user applications.


## Software Requirements

The Task-Aware MPI library requires the installation of the following tools and libraries:

* Automake, autoconf, libtool, make and a C and C++ compiler.
* An MPI library supporting the MPI_THREAD_MULTIPLE threading level.
* [Boost](http://boost.org) library version 1.59 or greater.
* A parallel task-based runtime system providing the API descrived in [src/common/RuntimeAPI.hpp](src/common/RuntimeAPI.hpp).
It is not required for the building of TAMPI, but it will be required when linking a user application
against this library. [OmpSs-2](https://github.com/bsc-pm/ompss-2-releases) (version 2018.11 or greater)
is one of the task-based programming models that supports TAMPI.


## Building and Installing

TAMPI uses the standard GNU automake and libtool toolchain. When cloning from a repository, the
building environment must be prepared through the following command:

```bash
$ autoreconf -fiv
```

When the code is distributed through a tarball, it usually does not need that command.

Then execute the following commands:

```bash
$ ./configure --prefix=$INSTALLATION_PREFIX --with-boost=$BOOST_HOME ..other options..
$ make
$ make install
```

where `$INSTALLATION_PREFIX` is the directory into which to install TAMPI, and `$BOOST_HOME` is the
prefix of the Boost installation. An MPI installation with multi-threading support must be available
when configuring the library. The MPI frontend compiler for C++ (usually `mpicxx`) must be provided
by either adding the binary's path to the PATH environment variable (i.e., executing `export
PATH=/path/to/mpi/bin:$PATH`) or by setting the MPICXX environment variable.

Other optional configuration flags are:
* `--disable-blocking-mode`: Disables the blocking mode of TAMPI. MPI_TASK_MULTIPLE threading level is
   never provided, so that calls to blocking MPI procedures are directly converted to calls to the same
   blocking procedures of the underlying MPI library. Also, by disabling this mode, TAMPI does not
   require the runtime system to provide the block/unblock API. The blocking mode is **enabled** by
   default.
* `--disable-nonblocking-mode`: Disables the non-blocking mode of TAMPI. Any call to TAMPI_Iwait or
   TAMPI_Iwaitall procedures results in undefined behavior. The non-blocking mode is **enabled** by
   default.
* `--enable-debug-mode`: Adds compiler debug flags and enables additional internal debugging mechanisms.
   Note that this flag can downgrade the overall performance. Debug mode is **disabled** by default.

Once TAMPI is built and installed, e.g, in `$TAMPI_HOME`, the installation folder will contain the libraries
in `$TAMPI_HOME/lib` (or `$TAMPI_HOME/lib64`) and the headers in `$TAMPI_HOME/include`. There are three
different libraries and each one has the static and dynamic variant:

* `libtampi`: The complete TAMPI library supporting both C/C++ and Fortran languages.
* `libtampi-c`: The TAMPI library for C/C++.
* `libtampi-fortran`: The TAMPI library for Fortran.

There are two header files which can be included from user applications that declare all needed
functions and constants. `TAMPI.h` is the header for C/C++ applications and `TAMPIf.h` is the header
for Fortran programs.

## Using TAMPI in Hybrid Applications

User applications should be linked against the MPI library (e.g, using `mpicxx` compiler), the parallel
task-based runtime system and the TAMPI library. For example, a hybrid MPI+OmpSs-2 application in C++
named `app.cpp` could be compiled and linked using the following MPICH command:

```bash
$ MPICH_CXX=mcxx mpicxx --ompss-2 -I${TAMPI_HOME}/include app.cpp -o app.bin -ltampi -L${TAMPI_HOME}/lib
```

Finally, the application could be launched as any traditional hybrid MPI+OmpSs-2 program.

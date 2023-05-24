# Task-Aware MPI Library

The Task-Aware MPI or TAMPI library extends the functionality of standard MPI libraries
by providing new mechanisms for improving the interoperability between parallel task-based
programming models, such as OpenMP and OmpSs-2, and MPI communications. This library allows
the safe and efficient execution of MPI operations from concurrent tasks and guarantees the
transparent management and progress of these communications.

By following the MPI Standard, programmers must pay close attention to avoid deadlocks
that may occur in hybrid applications (e.g., MPI+OpenMP) where MPI calls take place inside
tasks. This is given by the out-of-order execution of tasks that consequently alter the
execution order of the enclosed MPI calls. The TAMPI library ensures a deadlock-free
execution of such hybrid applications by implementing a cooperation mechanism between the
MPI library and the parallel task-based runtime system.

TAMPI provides two main mechanisms: the blocking mode and the non-blocking mode. The blocking
mode targets the efficient and safe execution of blocking MPI operations (e.g., MPI_Recv)
from inside tasks, while the non-blocking mode focuses on the efficient execution of
non-blocking or immediate MPI operations (e.g., MPI_Irecv), also from inside tasks.

On the one hand, TAMPI is currently compatible with two task-based programming model
implementations: a derivative version of the [LLVM OpenMP](https://github.com/bsc-pm/llvm) and
[OmpSs-2](https://github.com/bsc-pm/ompss-2-releases). However, the derivative OpenMP does
not support the full set of features provided by TAMPI. OpenMP programs can only make use of
the non-blocking mode of TAMPI, whereas OmpSs-2 programs can leverage both blocking and
non-blocking modes.

On the other hand, TAMPI is compatible with mainstream MPI implementations that support the
`MPI_THREAD_MULTIPLE` threading level, which is the minimum requirement to provide its task-aware
features. The following sections describe in detail the blocking (OmpSs-2) and non-blocking
(OpenMP & OmpSs-2) modes of TAMPI.


## Blocking Mode (OmpSs-2)

The blocking mode of TAMPI targets the safe and efficient execution of blocking MPI operations
(e.g., MPI_Recv) from inside tasks. This mode virtualizes the execution resources (e.g.,
hardware threads) of the underlying system when tasks call blocking MPI functions. When a
task calls a blocking operation, and it cannot complete immediately, the task is *paused*, and
the underlying execution resource is leveraged to execute other ready tasks while the MPI operation
does not complete. This means that the execution resource is **not blocked inside MPI**. Once
the operation completes, the paused task is resumed, so that it will eventually continue its
execution and it will return from the blocking MPI call.

All this is done transparently to the user, meaning that all blocking MPI functions maintain the
blocking semantics described in the MPI Standard. Also, this TAMPI mode ensures that the user
application can make progress although multiple communication tasks are executing blocking MPI
operations.

This virtualization prevents applications from blocking all execution resources inside MPI
(waiting for the completion of some operations), which could result in a deadlock due to the
lack of progress. Thus, programmers are allowed to instantiate multiple communication tasks
(that call blocking MPI functions) without the need of serializing them with dependencies,
which would be necessary if this TAMPI mode was not enabled. In this way, communication tasks
can run in parallel and their execution can be re-ordered by the task scheduler.

This mode provides support for the following set of blocking MPI operations:

* **Blocking primitives**: MPI_Recv, MPI_Send, MPI_Bsend, MPI_Rsend and MPI_Ssend.
* **Blocking collectives**: MPI_Gather, MPI_Scatter, MPI_Barrier, MPI_Bcast, MPI_Scatterv, etc.
* **Waiters** of a *complete set* of requests: **MPI_Wait** and **MPI_Waitall**.
	* MPI_Waitany and MPI_Waitsome are not supported yet; the standard behavior is applied.

As stated previously, this mode is only supported by [OmpSs-2](https://github.com/bsc-pm/ompss-2-releases) (version 2018.11 or greater).


### Using the blocking mode

This library provides a header named `TAMPI.h` (or `TAMPIf.h` in Fortran). Apart from other
declarations and definitions, this header defines a new MPI level of thread support named
`MPI_TASK_MULTIPLE`, which is monotonically greater than the standard `MPI_THREAD_MULTIPLE`. To
activate this mode from an application, users must:

* Include the `TAMPI.h` header in C or `TAMPIf.h` in Fortran.
* Initialize MPI with `MPI_Init_thread` and requesting the new `MPI_TASK_MULTIPLE` threading
  level.

The blocking TAMPI mode is considered activated once `MPI_Init_thread` successfully returns,
and the provided threading level is `MPI_TASK_MULTIPLE`. A valid and safe usage of TAMPI's blocking
mode is shown in the following OmpSs-2 + MPI example:

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
  // ...
  
  if (rank == 0) {
    for (int n = 0; n < N; ++n) {
      #pragma oss task in(data[n]) // T1
      {
        MPI_Ssend(&data[n], 1, MPI_INT, 1, n, MPI_COMM_WORLD);
        // Data buffer could already be reused
      }
    }
  } else if (rank == 1) {
    for (int n = 0; n < N; ++n) {
      #pragma oss task out(data[n]) // T2
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

This program would be incorrect when using the standard `MPI_THREAD_MULTIPLE` since it could
result in a deadlock situation depending on the task scheduling policy. Because of the
out-of-order execution of tasks in each rank, all available execution resources could end up
blocked inside MPI, hanging the application due to the lack of progress. However, with TAMPI's
`MPI_TASK_MULTIPLE` threading level, execution resources are prevented from blocking inside
blocking MPI calls and they can execute other ready tasks while communications are taking
place, guaranteeing application progress.

See the articles listed in the [References](#references) section for more information.


## Non-Blocking Mode (OpenMP & OmpSs-2)

The non-blocking mode of TAMPI focuses on the execution of non-blocking or immediate MPI
operations from inside tasks. As the blocking TAMPI mode, the objective of this one is to
allow the safe and efficient execution of multiple communication tasks in parallel, but
avoiding the pause of these tasks.

The idea is to allow tasks to bind their completion to the finalization of one or more MPI
requests. Thus, the completion of a task is delayed until (1) it finishes the execution of
its body code and (2) all MPI requests that it bound during its execution complete. Notice
that the completion of a task usually implies the release of its dependencies, the freeing
of its data structures, etc.

For that reason, TAMPI defines two asynchronous and non-blocking functions named **TAMPI_Iwait**
and **TAMPI_Iwaitall**, which have the same parameters as their standard synchronous counterparts
MPI_Wait and MPI_Waitall, respectively. They bind the completion of the calling task to the
finalization of the MPI requests passed as parameters, and they return "immediately" without
blocking the caller. The completion of the calling task will take place once it finishes its
execution and all bound MPI requests complete.

Since they are non-blocking and asynchronous, a task after calling TAMPI_Iwait or TAMPI_Iwaitall
passing some requests cannot assume that the corresponding operations have already finished.
For this reason, the communication buffers related to those requests should not be consumed or
reused inside that task. The proper way is to correctly annotate the communication tasks (the
ones calling TAMPI_Iwait or TAMPI_Iwaitall) with the dependencies on the corresponding
communication buffers, and then, annotating also the tasks that will reuse or consume the data
buffers. In this way, these latter will become ready once the data buffers are safe to be
accessed (i.e., once the communications have been completed). Defining the correct dependencies
of tasks is essential to guarantee a correct execution order.

Calling any of these two functions from outside a task will result in undefined behavior. Note
that the requests passed to these functions could be generated by calling non-blocking MPI
operations (e.g., MPI_Irecv) from the same tasks that call TAMPI_Iwait or TAMPI_Iwaitall, from
calls of other previously executed tasks or even from the main function. Also, notice that all
requests with value `MPI_REQUEST_NULL` will be ignored.

As stated in the introduction, this non-blocking mode is supported by both a derivative version
of [LLVM OpenMP](https://github.com/bsc-pm/llvm) and [OmpSs-2](https://github.com/bsc-pm/ompss-2-releases)
(version 2018.11 or greater).


### Using the non-blocking mode

To activate this mode from applications, users must:

* Include the `TAMPI.h` header in C or `TAMPIf.h` in Fortran.
* Initialize the MPI with `MPI_Init_thread` requesting at least the standard `MPI_THREAD_MULTIPLE`
  threading level.

The non-blocking mode of TAMPI is considered activated once `MPI_Init_thread` returns successfully,
and the provided threading level is at least `MPI_THREAD_MULTIPLE`. If MPI is not initialized in that
way, any call to TAMPI_Iwait or TAMPI_Iwaitall will be ignored.

Notice that this mode is orthogonal to the blocking one, which we presented in the previous section.
An OmpSs-2 program could activate both modes by initializing MPI with `MPI_TASK_MULTIPLE`, so that
both mechanisms could operate at the same time. This does not apply to OpenMP programs since the
aforementioned derivative implementation of OpenMP does not support the blocking mode.

The following OpenMP + MPI example shows a valid and safe usage of this mode:

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
  
  #pragma omp parallel
  #pragma omp single
  {
    if (rank == 0) {
      for (int n = 0; n < N; ++n) {
        #pragma omp task depend(in: data[n]) // T1
        {
          MPI_Request request;
          MPI_Issend(&data[n], 1, MPI_INT, 1, n, MPI_COMM_WORLD, &request);
          TAMPI_Iwait(&request, MPI_STATUS_IGNORE);
          // Data buffer cannot be reused yet!
          // Other unrelated computation...
        }
      }
    } else if (rank == 1) {
      for (int n = 0; n < N; ++n) {
        #pragma omp task depend(out: data[n], statuses[n]) // T2
        {
          MPI_Request request;
          MPI_Irecv(&data[n], 1, MPI_INT, 0, n, MPI_COMM_WORLD, &request);
          TAMPI_Iwaitall(1, &request, &statuses[n]);
          // Data buffer and status cannot be accessed yet!
          // Other unrelated computation...
        }
        
        #pragma omp task depend(in: data[n], statuses[n]) // T3
        {
          check_status(&statuses[n]);
          fprintf(stdout, "data[%d] = %d\n", n, data[n]);
        }
      }
    }
    #pragma omp taskwait
  }
  
  //...
}
```

The above code does the same as the example presented in the previous section but exclusively using
the non-blocking mode. In this case, the sender rank creates tasks (T1) that use the immediate version
of the MPI_Ssend operation, they bind their release of dependencies to the completion of the resulting
request with TAMPI_Iwait, and they finish their execution "immediately".

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


## Wrapper Functions for Code Compatibility

Typically, application developers try to adapt their code to different parallel programming models and
platforms, so that an application can be compiled enabling a specific programming model or even a
combination of various programming models, e.g., hybrid programming. However, developing and maintaining
such type of applications can be difficult and tedious. To facilitate the task of developing this
kind of multi-platform applications, TAMPI provides a set of wrapper functions and macros that behave
differently depending on whether the **non-blocking mode** of TAMPI is enabled or not.

The idea behind those wrapper functions is to allow users to use the same code for pure MPI and hybrid
configurations. In this way, the user does not need to duplicate the code or to use conditional compilation
directives in order to keep correct both variants. For instance, when the application is compiled only
with MPI (i.e., the non-blocking mode of TAMPI cannot be enabled) the wrappers will behave differently
than when when compiling with MPI, OpenMP and enabling the non-blocking mode of TAMPI. In the first case,
the OpenMP pragmas are ignored, and the standard operating of MPI is enforced, whereas in the second case,
the OpenMP pragmas are enabled and the non-blocking mechanisms of TAMPI are used.

The following table shows in the first column the name of each wrapper function. We provide a wrapper
function for each standard non-blocking communication operation (e.g., MPI_Issend), be it primitives
or collectives. The wrapper's name of a standard non-blocking function is composed of the original MPI
name and adding the prefix *TA*. For instance, the corresponding wrapper function for MPI_Ibrecv is
TAMPI_Ibrecv.

The actual behavior of a wrapper function, when the non-blocking TAMPI is disabled (e.g., in pure MPI mode)
is shown in the second column. The third column shows the behavior when the non-blocking mode of TAMPI is
enabled, and therefore, in this case, OpenMP or OmpSs-2 must be enabled.

Regardless of whether TAMPI is enabled or not, the wrapper functions call the corresponding standard
non-blocking MPI function. Additionally, if the non-blocking mode is active, that call is followed by a call
to TAMPI_Iwait passing the request generated by the previous non-blocking function. In this way, the calling
task binds its completion to the finalization of that MPI request, as previously explained in this section.

| Function         | TAMPI Disabled  | TAMPI Enabled                  |
|------------------|-----------------|--------------------------------|
| `TAMPI_Isend`    | `MPI_Isend`     | `MPI_Isend` + `TAMPI_Iwait`    |
| `TAMPI_Irecv`    | `MPI_Irecv`     | `MPI_Irecv` + `TAMPI_Iwait`    |
| `TAMPI_Ibcast`   | `MPI_Ibcast`    | `MPI_Ibcast` + `TAMPI_Iwait`   |
| `TAMPI_Ibarrier` | `MPI_Ibarrier`  | `MPI_Ibarrier` + `TAMPI_Iwait` |
| `TAMPI_I...`     | `MPI_I...`      | `MPI_I...` + `TAMPI_Iwait`     |


The parameters of the wrapper functions are the same as their corresponding standard non-blocking MPI
functions. The exception is TAMPI_Irecv because it requires an additional status parameter, which is
not required in the standard MPI_Irecv. The following snippet shows the C prototype of both TAMPI_Irecv
and TAMPI_Isend.

```c
int TAMPI_Irecv(void *buf, int count, MPI_Datatype datatype, int src,
                int tag, MPI_Comm comm, MPI_Request *request,
                MPI_Status *status);

int TAMPI_Isend(const void *buf, int count, MPI_Datatype datatype, int dst,
                int tag, MPI_Comm comm, MPI_Request *request);
```

With those wrappers, we cover the conventional phase of MPI applications where they start the communications
with multiple calls to non-blocking MPI functions (e.g., MPI_Isend and MPI_Irecv). Usually, after this phase,
MPI applications wait for the completion of all those issued operations by calling MPI_Wait or MPI_Waitall
from the main thread. However, this technique is not optimal in hybrid applications that want to parallelize
both computation and communication phases, since it considerably reduces the parallelism in each communication
phase.

For this reason, TAMPI provides two special wrapper functions that are essential for facilitating the
development of multi-platform applications. These two wrappers are TAMPI_Wait and TAMPI_Waitall, and
their behavior is shown in the following table.

| Function         | TAMPI Disabled  | TAMPI Enabled |
|------------------|-----------------|---------------|
| `TAMPI_Wait`     | `MPI_Wait`      |       -       |
| `TAMPI_Waitall`  | `MPI_Waitall`   |       -       |


As the previously presented wrappers, they behave differently depending on whether the non-blocking
mode of TAMPI is enabled or not. If TAMPI is not active, those wrappers only call directly to MPI_Wait
or MPI_Waitall, respectively. However, if TAMPI is active, they do nothing. With all these wrappers,
multi-platform applications can have a hybrid MPI + OpenMP + TAMPI code that also can correctly work
when compiling only with MPI.

The following MPI + OpenMP example in Fortran tries to demonstrate the idea behind all those wrapper functions:

```fortran
#include "TAMPIf.h"

! ...

nreqs=0
tag=10
do proc=1,nprocs
  if (sendlen(proc) > 0) then
    nreqs=nreqs+1
    len=sendlen(proc)
    
    !$OMP TASK DEFAULT(shared) FIRSTPRIVATE(proc,tag,len,nreqs) &
    !$OMP&     PRIVATE(err) DEPEND(IN: senddata(:,proc))
    call TAMPI_Isend(senddata(:,proc),len,MPI_REAL8,proc-1,tag,MPI_COMM_WORLD, &
          requests(nreqs),err)
    !$OMP END TASK
  end if
  if (recvlen(proc) > 0) then
    nreqs=nreqs+1
    len=recvlen(proc)
    
    !$OMP TASK DEFAULT(shared) FIRSTPRIVATE(proc,tag,len,nreqs) &
    !$OMP&     PRIVATE(err) DEPEND(OUT: recvdata(:,proc))
    call TAMPI_Irecv(recvdata(:,proc),len,MPI_REAL8,proc-1,tag,MPI_COMM_WORLD, &
          requests(nreqs),statuses(nreqs),err)
    !$OMP END TASK
  end if
end do

call TAMPI_Waitall(nreqs,requests(:),statuses(:),err)

! Computation tasks consuming or reusing communications buffers
! ...
```

The previous example sends/receives a message, if needed, to/from the rest of MPI processes. Notice that
this code would be correct for both a pure MPI execution (i.e., ignoring the OpenMP directives) and an
execution with MPI+OpenMP+TAMPI. In the first case, the TAMPI_Isend, TAMPI_Irecv, and TAMPI_Waitall calls
will become calls to MPI_Isend, MPI_Irecv and MPI_Waitall, respectively.

However, if the non-blocking TAMPI mode is active, being OpenMP or OmpSs-2 also enabled obligatorily,
TAMPI_Isend and TAMPI_Irecv will be executed from inside tasks. The TAMPI_Isend call would become an
MPI_Isend call followed by a TAMPI_Iwait call passing the generated request as a parameter. In this way,
the calling task will bind its completion to the finalization of that MPI request. The same will happen
to TAMPI_Irecv. Finally, TAMPI_Waitall would do nothing since each task would already bound its own MPI
request.

It is important to mention that both TAMPI_Irecv and TAMPI_Waitall have a status location as a parameter.
These statuses should be consistent in both calls; a status location passed to a TAMPI_Irecv should be
the same as the status location that corresponds to that request in the TAMPI_Waitall. It could also be
correct to specify the status to be ignored in both calls.


## References

More information about this work can be found in the articles listed below. **Citations to the TAMPI library should reference these articles**:

* Sala, K., Teruel, X., Perez, J. M., Peña, A. J., Beltran, V., & Labarta, J. (2019). **Integrating blocking and non-blocking MPI primitives with task-based programming models**. Parallel Computing, 85, 153-166. [[Article link]](https://doi.org/10.1016/j.parco.2018.12.008)
* Sala, K., Bellón, J., Farré, P., Teruel, X., Perez, J. M., Peña, A. J., Holmes, D., Beltran. V., & Labarta, J. (2018, September). **Improving the interoperability between MPI and task-based programming models**. In Proceedings of the 25th European MPI Users' Group Meeting (p. 6). ACM. [[Article link]](https://doi.org/10.1145/3236367.3236382)

Furthermore, several works have demonstrated the performance and programmability benefits of leveraging the TAMPI library in hybrid applications:

* Ciesko, J., Martínez-Ferrer, P. J., Veigas, R. P., Teruel, X., & Beltran, V. (2020). **HDOT—An approach towards productive programming of hybrid applications**. Journal of Parallel and Distributed Computing, 137, 104-118. [[Article link]](https://doi.org/10.1016/j.jpdc.2019.11.003)
* Sala, K., Rico, A., & Beltran, V. (2020, September). **Towards Data-Flow Parallelization for Adaptive Mesh Refinement Applications**. In 2020 IEEE International Conference on Cluster Computing (CLUSTER) (pp. 314-325). IEEE. [[Article link]](https://doi.org/10.1109/CLUSTER49012.2020.00042)
* Maroñas, M., Teruel, X., Bull, J. M., Ayguadé, E., & Beltran, V. (2020, September). **Evaluating Worksharing Tasks on Distributed Environments**. In 2020 IEEE International Conference on Cluster Computing (CLUSTER) (pp. 69-80). IEEE. [[Article link]](https://doi.org/10.1109/CLUSTER49012.2020.00017)


## Acknowledgments

This work was financially supported by the PRACE and INTERTWinE projects funded in part by the European
Union’s Horizon 2020 Research programme (2014-2020) under grant agreements 823767 and 671602, respectively.
This research has also received funding through The European PILOT project from the European Union's Horizon
2020/EuroHPC research and innovation programme under grant agreement 101034126. Project PCI2021-122090-2A
was funded by MCIN/AEI /10.13039/501100011033 and European Union NextGenerationEU/PRTR. The Departament de
Recerca i Universitats de la Generalitat de Catalunya also funded the Programming Models research group at
BSC-UPC under grant agreement 2021 SGR01007. This research received funding also from Huawei SoW1.


# Getting Started

This section describes the software requirements of TAMPI, the building and installation process and how the library
can be used from user applications.


## Software Requirements

The Task-Aware MPI library requires the installation of the following tools and libraries:

* Automake, autoconf, libtool, make and a C and C++ compiler.
* An MPI library supporting the `MPI_THREAD_MULTIPLE` threading level.
* [Boost](http://boost.org) library version 1.59 or greater.
* One of the following parallel task-based programming models (required when compiling a user application):
	- [OmpSs-2](https://github.com/bsc-pm/ompss-2-releases) (version 2018.11 or greater): Supports both blocking and
	  non-blocking TAMPI modes.
	- A derivative implementation of [LLVM OpenMP](https://github.com/bsc-pm/llvm): Supports the non-blocking TAMPI mode.


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
when configuring by either adding the binary's path to the `PATH` environment variable (i.e.,
executing `export PATH=/path/to/mpi/bin:$PATH`) or by setting the `MPICXX` environment variable.

Other optional configuration flags are:
* `--disable-blocking-mode`: Disables the blocking mode of TAMPI. `MPI_TASK_MULTIPLE` threading level is
   never provided, so that calls to blocking MPI procedures are directly converted to calls to the same
   blocking procedures of the underlying MPI library. Also, by disabling this mode, TAMPI does not
   require the runtime system to provide the block/unblock API. The blocking mode is **enabled** by
   default.
* `--disable-nonblocking-mode`: Disables the non-blocking mode of TAMPI. Any call to TAMPI_Iwait or
   TAMPI_Iwaitall procedures will be ignored. The non-blocking mode is **enabled** by default.
* `--enable-debug-mode`: Adds compiler debug flags and enables additional internal debugging mechanisms.
   Note that this flag can downgrade the overall performance. Debug mode is **disabled** by default.
* `--with-ovni`: Build the instrumentation with [ovni](https://github.com/bsc-pm/ovni) and allow enabling
   ovni tracing at run-time through the `TAMPI_INSTRUMENT` enviornment variable.

Once TAMPI is built and installed, e.g, in `$TAMPI_HOME`, the installation folder will contain the libraries
in `$TAMPI_HOME/lib` (or `$TAMPI_HOME/lib64`) and the headers in `$TAMPI_HOME/include`. There are three
different libraries and each one has the static and dynamic variant:

* `libtampi`: The complete TAMPI library supporting both C/C++ and Fortran languages.
* `libtampi-c`: The TAMPI library for C/C++.
* `libtampi-fortran`: The TAMPI library for Fortran.

There are two header files which can be included from user applications that declare all needed
functions and constants. `TAMPI.h` is the header for C/C++ applications and `TAMPIf.h` is the header
for Fortran programs.


## Run-time Options

The TAMPI library has some run-time options that can be set through environment variables. These are:

* `TAMPI_POLLING_PERIOD` (default `100` us): The polling period in microseconds in which the in-flight MPI
requests are checked by a transparent task. This polling task is scheduled every specific number of
microseconds to check the pending MPI requests that have been generated by the tasks that called the TAMPI
communication services. This envar expects a positive integer number; the value `0` means that the task should
be always running. Notice that the task leverages a CPU while running. By default, this task runs every `100`
microseconds. This value may be decreased by the user in communication-intensive applications or increased in
applications with low communication weights. This envar is only considered when running with OmpSs-2 2020.06
or newer.

* `TAMPI_INSTRUMENT` (default `none`): The TAMPI library leverages [ovni](https://github.com/bsc-pm/ovni) for
instrumenting and generating [Paraver](https://tools.bsc.es/paraver) traces. For builds with the capability of
extracting Paraver traces, the TAMPI library should be configured passing a valid ovni installation through
the `--with-ovni`. Then, at run-time, define the `TAMPI_INSTRUMENT=ovni` environment variable to generate an ovni
trace. After the execution, the ovni trace can be converted to a Paraver trace with the `ovniemu` tool. You can
find more information regarding ovni tracing at <https://github.com/bsc-pm/ovni>.

**IMPORTANT:** The `TAMPI_POLLING_FREQUENCY` envar is **deprecated** and will be removed in future versions.
Please use `TAMPI_POLLING_PERIOD` instead. The deprecated envar is considered only when `TAMPI_POLLING_PERIOD`
is not defined.


## Leveraging TAMPI in Hybrid Applications

User applications should be linked against the MPI library (e.g, using `mpicc` or `mpicxx` compiler), the
parallel task-based runtime system and the TAMPI library. A hybrid OpenMP + MPI application in C++ named
`app.cpp` could be compiled and linked using the following command:

```bash
$ mpicxx -cxx=clang++ -fopenmp -I${TAMPI_HOME}/include app.cpp -o app.bin -ltampi -L${TAMPI_HOME}/lib
```

Similarly, a hybrid OmpSs-2 + MPI application in C++ named `app.cpp` could be compiled and linked using the
following command:

```bash
$ mpicxx -cxx=mcxx --ompss-2 -I${TAMPI_HOME}/include app.cpp -o app.bin -ltampi -L${TAMPI_HOME}/lib
```

Please note that the options passed to `mpicc` or `mpicxx` may differ between different MPI implementations.
The `-cxx` option indicates that `mpicxx` should use the compiler passed as a parameter to compile and link
the source files. This can also be indicated with the environment variables `MPICH_CC` or `MPICH_CXX` for
MPICH and MVAPICH, `I_MPI_CC` or `I_MPI_CXX` for Intel MPI, and `OMPI_CC` or `OMPI_CXX` for OpenMPI. For
instance, the OmpSs-2 application could be compiled and linked using MPICH with the following command:

```bash
$ MPICH_CXX=mcxx mpicxx --ompss-2 -I${TAMPI_HOME}/include app.cpp -o app.bin -ltampi -L${TAMPI_HOME}/lib
```

Finally, both OpenMP and OmpSs-2 applications can be launched as any traditional hybrid program.

### Fortran

The `TAMPIf.h` header for Fortran defines some preprocessor macros. Therefore, to correctly use TAMPI
in Fortran programs, users must include the header with `#include "TAMPIf.h"` at the starting lines of the
program, as shown in the following example. Additionally, the program must be preprocessed when compiling
the code. The preprocessing is enabled by default when the code file has the extension `.F90`, `.F95`, etc.,
by passing the `-cpp` option to `gfortran` compiler or by passing the `--pp` option to the Mercurium compiler.

```fortran
#include "TAMPIf.h"

module test_mod
! ...
end module test_mod

program test_prog
! ...
end program test_prog
```

Finally, due to some technical limitations in the Fortran language, the wrapper functions of the non-blocking
TAMPI mode cannot be written in all combinations of lowercase and uppercase letters. For instance, the only
accepted ways to write in a Fortran program a call to the TAMPI_Issend procedure are:

* `TAMPI_Issend`
* `TAMPI_issend`
* `tampi_issend`

This limitation is applied to all other wrappers presented in the [Wrapper Functions](#wrapper-functions-for-code-compatibility)
section, including both TAMPI_Wait and TAMPI_Waitall.


## Frequently Asked Questions (FAQ)

This section answers some of the most common questions:

**Q1: Why does linking my application to the TAMPI library fail with undefined symbols?**

Some MPI libraries, such as OpenMPI, do not provide the MPI symbols for Fortran by default. In the case you are
getting undefined symbol errors and your application is C/C++, you can link to *libtampi-c* instead of *libtampi*.

**Q2: Why does my application not perform as expected when using TAMPI?**

One of the first aspects to check when your application does not perform as expected is the polling frequency
in which TAMPI background services are working. By default, the TAMPI services check the internal MPI requests
every 100us (see `TAMPI_POLLING_PERIOD` envar). This period should be enough for most applications,
including communication-intensive ones. However, your application may need a higher polling frequency (i.e.,
reducing the envar's value) or a lower frequency (i.e., increasing the envar's value).

**Q3: Why does my application hang or report an error when using TAMPI for point-to-point communication?**

Most MPI or hybrid MPI+OpenMP applications do not use specific MPI tags when sending and receiving messages.
That's quite common because these applications usually issue MPI operations from a single thread and are always in
the same order on both the sender and receiver sides. They rely on the message ordering guarantees of MPI, so they
use an arbitrary MPI tag for multiple or all messages.

That becomes a problem when issuing MPI operations from different concurrent threads or tasks. The order of sending
and receiving messages in the sender and receiver sides may be different, and thus, messages can arrive and match
in any order. To avoid this issue, you should use distinct tags for the different messages that you sending and
receive. For instance, if you are exchanging multiple blocks of data, and you want to send a message per block
(encapsulated in a separate task), you could use the block id as the MPI tag of the corresponding message.

**Q4: Why does my application hang or report an error when using TAMPI for collective communication?**

This issue is quite related to the previous one. The MPI standard does not allow identifying collective operations
with MPI tags. The standard enforces the user to guarantee a specific order when issuing multiple **collectives of
the same type and through the same communicator** in all involved processes. That limits the parallelism that we
can achieve when executing multiple collective operations of the same time. If you want to run multiple collectives
of the same type in parallel (e.g., different concurrent tasks), we recommend using separate MPI communicators.
Notice that having many MPI communicators could damage the application's performance depending on the MPI implementation.

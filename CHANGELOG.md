# Task-Aware MPI Release Notes
All notable changes to this project will be documented in this file.


## Version 4.1, Fri Jun 6, 2025
The 4.1 release of TAMPI reworks the polling mechanism through ALPI's new suspend feature, adds compatibility with ALPI 1.2, and improves several configuration and installation options. Additionally, it introduces several bug fixes and QOL improvements.

### General
- Rework the polling mechanism through ALPI's new suspend feature
- Add compatibility with ALPI v1.2
- Generate a PKGCONFIG file on installation
- Allow specifying the maximum number of CPUs of the system while configuring TAMPI
- Improve the logging of tests

### Bug Fixes
- Fixed passing of lambdas in some boost functions to fix compatibility with v1.87.9
- Removed `cpubind` from tests to avoid unexpected behavior depending on SLURM configuration


## Version 4.0, Fri Nov 15, 2024
The 4.0 release introduces TAMPI-OPT, a newly optimized version of the library that ensures only a single thread accesses the MPI interface through delegation techniques. This approach gives access comparable to the highest MPI performance of single-threaded scenarios. Moreover, it introduces several bug fixes and usability and code improvements. Due to the new optimizations, some features have been ultimately dropped, while others will remain unsupported temporarily. For a detailed list check the bullet points below.

### General
- Major library optimizations by serializing communications
- Bump required MPI standard to 3.0 or later
- Add support to adjust the completion polling task period dynamically
- Improved collective operations constructors
- Improve allocations of Operations through a scalable allocator
- Improved ALPI symbol handling
- Other bug fixes and code/performance improvements

### Unsupported Features
- Temporarily dropped support for Fortran.
- Temporarily dropped support for request-based MPI operations: `MPI_Wait`, `MPI_Waitall`, `TAMPI_Iwait`, `TAMPI_Iwaitall`.
- Dropped support for `MPI_Sendrecv` and `MPI_Sendrecv_replace`. Every point-to-point and collective operation is **supported**.


## Version 3.0.1, Thu Dec 7, 2023
The 3.0.1 release introduces bug fixes.

### General
- Fix memory order in an atomic store
- Update README with information regarding the [OpenMP-V](https://github.com/bsc-pm/llvm) runtime


## Version 3.0, Fri Nov 17, 2023
The 3.0 release introduces the use of the generic ALPI tasking interface, bug fixes, and improved usability and programmability. This version also extends the ovni instrumentation to show more information regarding the TAMPI behavior in Paraver traces.

### General
- Rely on the [ALPI](https://gitlab.bsc.es/alpi/alpi) tasking interface (OmpSs-2 2023.11 or later)
- Drop support for the Nanos6-specific tasking interface
- Drop support for older versions than OmpSs-2 2023.11
- Remove deprecated `TAMPI_POLLING_FREQUENCY` environment variable
- Stop using PMPI interfaces for testing internal requests (e.g., `PMPI_Test`)
- Do not assume the default MPI threading level is `MPI_THREAD_SINGLE`
- Load first occurrence of the ALPI tasking interface symbols (`RTLD_DEFAULT`)
- Add opt-in mechanism to explicitly initialize TAMPI independently from MPI
- Add opt-in mechanism to disable task-awareness for specific threads
- Refactor and simplify symbol loading

### Instrumentation
- Instrument library subsystems with ovni; see the [ovni documentation](https://ovni.readthedocs.io) for more information
- Improve ovni library discovery

### Building
- Add `--enable-debug` configure option replacing `--enable-debug-mode`
- Add `--enable-asan` option to enable address sanitizer flags
- Deprecate `--enable-debug-mode` option, which will be removed in next versions

### Testing
- Improve testing scripts and Makefiles
- Fix CPU binding on SLURM-based tests
- Add testing option `--skip-omp` to skip the execution of OpenMP tests


## Version 2.0, Fri May 26, 2023
The 2.0 release introduces several performance improvements, important bug fixes, and improved usability and programmability. Several environment variables that users can set to change default behavior have been updated. This version also introduces support for the ovni instrumentation to obtain Paraver execution traces.

### General
- Introduce `TAMPI_POLLING_PERIOD` replacing `TAMPI_POLLING_FREQUENCY`
- Deprecate `TAMPI_POLLING_FREQUENCY` and will be removed in next versions
- Drop support for OmpSs-2 2020.06; now requiring OmpSs-2 2020.11 or later
- Leverage C++17 standard, which may require newer GCC (such as GCC 7 or later)
- Extend README with a Frequently Asked Questions (FAQ) section

### Performance
- Set default polling period (`TAMPI_POLLING_PERIOD`) to 100us, which can improve applications' performance
- Fix and improve implementation of custom spinlocks
- Remove use of std::function due to its dynamic memory allocations

### Fixes and Code Improvements
- Reduce code duplication between C/C++ and Fortran support
- Fix Fortran interfaces
- Fix and improve testing infrastructure and test codes
- Compile all libraries with -fPIC

### Instrumentation
- Add [ovni](https://github.com/bsc-pm/ovni) instrumentation to generate Paraver traces for multi-node executions
- Enable ovni instrumentation when `TAMPI_INSTRUMENT=ovni` environment variable
- Drop support for Nanos6-specific instrumentation; use ovni instead

# Task-Aware MPI Release Notes
All notable changes to this project will be documented in this file.


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

#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdlib>
#include <cstdio>
#include <sys/time.h>

#include <mpi.h>

#define CHECK(f...)                                                                     \
    do {                                                                                \
        const int __error = f;                                                          \
        if (__error != MPI_SUCCESS) {                                                   \
            fprintf(stderr, "Error: '%s' [%s:%i]: %i\n",#f,__FILE__,__LINE__, __error); \
            std::abort();                                                               \
        }                                                                               \
    } while(0);

#define ASSERT(e...)                                                       \
    do {                                                                   \
        bool __condition = e;                                              \
        if (!__condition) {                                                \
            fprintf(stderr, "Error: '%s' [%s:%i]\n",#e,__FILE__,__LINE__); \
            std::abort();                                                  \
        }                                                                  \
    } while(0);


double getTime()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return tv.tv_sec + 1e-6 * tv.tv_usec;
}

#endif // UTILS_HPP


#ifndef ERROR_H
#define ERROR_H

#include <cstdlib>
#include <iostream>
#include <string>

namespace Error {
	static inline void fail(const std::string &message)
	{
		std::cerr << message << std::endl;
		abort();
	}
	
	static inline void failIf(bool condition, const std::string &message)
	{
		if (condition) {
			fail(message);
		}
	}
}

#endif // ERROR_H

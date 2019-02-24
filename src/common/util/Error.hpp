/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef ERROR_HPP
#define ERROR_HPP

#include <cstdlib>
#include <iostream>
#include <string>


namespace error {
	static inline void fail(const std::string &message)
	{
		std::cerr << "Error: " << message << std::endl;
		std::abort();
	}
	
	static inline void failIf(bool condition, const std::string &message)
	{
		if (condition) {
			fail(message);
		}
	}
	
	static inline void warn(const std::string &message)
	{
		std::cerr << "Warning: " << message << std::endl;
	}
	
	static inline void warnIf(bool condition, const std::string &message)
	{
		if (condition) {
			warn(message);
		}
	}
} // namespace error

#endif // ERROR_HPP

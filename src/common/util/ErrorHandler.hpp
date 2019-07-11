/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2019 Barcelona Supercomputing Center (BSC)
*/

#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include <cstdlib>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string.h>
#include <unistd.h>

#include "SpinLock.hpp"


class ErrorHandler {
private:
	static SpinLock _lock;
	
	template<typename T, typename... TS>
	static inline void emitReasonParts(std::ostringstream &oss, T const &firstReasonPart, TS... reasonParts)
	{
		oss << firstReasonPart;
		emitReasonParts(oss, reasonParts...);
	}
	
	static inline void emitReasonParts(__attribute__((unused)) std::ostringstream &oss)
	{
	}
	
	static inline void safeEmitPart(__attribute__((unused)) char *buffer, __attribute__((unused)) size_t size, char part)
	{
		write(2, &part, 1);
	}
	
	static inline void safeEmitPart(char *buffer, size_t size, int part)
	{
		int length = snprintf(buffer, size, "%i", part);
		write(2, buffer, length);
	}
	
	static inline void safeEmitPart(char *buffer, size_t size, long part)
	{
		int length = snprintf(buffer, size, "%li", part);
		write(2, buffer, length);
	}
	
	static inline void safeEmitPart(__attribute__((unused)) char *buffer, __attribute__((unused)) size_t size, char const *part)
	{
		write(2, part, strlen(part));
	}
	
	static inline void safeEmitPart(char *buffer, size_t size, float part)
	{
		int length = snprintf(buffer, size, "%f", part);
		write(2, buffer, length);
	}
	
	static inline void safeEmitPart(char *buffer, size_t size, double part)
	{
		int length = snprintf(buffer, size, "%f", part);
		write(2, buffer, length);
	}
	
	template<typename T, typename... TS>
	static inline void safeEmitReasonParts(char *buffer, size_t size, T const &firstReasonPart, TS... reasonParts)
	{
		safeEmitPart(buffer, size, firstReasonPart);
		safeEmitReasonParts(buffer, size, reasonParts...);
	}
	
	static inline void safeEmitReasonParts(__attribute__((unused)) char *buffer, __attribute__((unused)) size_t size)
	{
	}
	
public:
	template<typename... TS>
	static inline void fail(TS... reasonParts)
	{
		std::ostringstream oss;
		oss << "Error: ";
		emitReasonParts(oss, reasonParts...);
		oss << std::endl;
		
		{
			std::lock_guard<SpinLock> guard(_lock);
			std::cerr << oss.str();
		}
		
#ifndef NDEBUG
		abort();
#else
		exit(1);
#endif
	}
	
	template<typename... TS>
	static inline void failIf(bool failure, TS... reasonParts)
	{
		if (__builtin_expect(!failure, 1)) {
			return;
		}
		
		fail(reasonParts...);
	}
	
	template<typename... TS>
	static inline void warn(TS... reasonParts)
	{
		std::ostringstream oss;
		oss << "Warning: ";
		emitReasonParts(oss, reasonParts...);
		oss << std::endl;
		
		{
			std::lock_guard<SpinLock> guard(_lock);
			std::cerr << oss.str();
		}
	}
	
	template<typename... TS>
	static inline void warnIf(bool failure, TS... reasonParts)
	{
		if (__builtin_expect(!failure, 1)) {
			return;
		}
		
		warn(reasonParts...);
	}
};

#endif // ERROR_HANDLER_HPP

/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.
	
	Copyright (C) 2015-2018 Barcelona Supercomputing Center (BSC)
*/

#ifndef POLLING_SERVICES_HPP
#define POLLING_SERVICES_HPP

#include "RuntimeAPI.hpp"

#include <string>

class PollingServices {
public:
	typedef nanos6_polling_service_t polling_function_t;
	
	static inline void registerService(const std::string name, polling_function_t function, void *data = nullptr)
	{
		nanos6_register_polling_service(name.c_str(), function, data);
	}
	
	static inline void unregisterService(const std::string name, polling_function_t function, void *data = nullptr)
	{
		nanos6_unregister_polling_service(name.c_str(), function, data);
	}
};

#endif // POLLING_SERVICES_HPP

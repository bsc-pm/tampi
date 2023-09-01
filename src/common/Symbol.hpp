/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2023 Barcelona Supercomputing Center (BSC)
*/

#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <dlfcn.h>
#include <string>

#include "util/ErrorHandler.hpp"

namespace tampi {

//! Class that allows the dynamic loading of symbols at run-time
class Symbol {
public:
	//! \brief The attributes when loading symbols with Symbol::load
	enum LoadAttr : int {
		//! The default behavior when loading symbols. The default
		//! policy loads the next occurence of the symbol (RTLD_NEXT)
		//! and the loading is mandatory
		DEFAULT = 0x0,
		//! Load the first occurence of the symbol (RTLD_DEFAULT)
		FIRST = 0x1,
		//! Load the symbol as optional; do not abort the execution
		//! if the symbol is not found
		OPTIONAL = 0x2,
	};

	//! \brief Load a symbol from the subsequent libraries
	//!
	//! \param symbolName The name of the symbol to load
	//! \param attr The attributes of the symbol; LoadAttr::DEFAULT by default
	//!
	//! \returns An opaque pointer to the symbol or null if not found
	static inline void *load(
		const std::string &symbolName, int attr = LoadAttr::DEFAULT
	) {
		void *handle = (attr & LoadAttr::FIRST) ? RTLD_DEFAULT : RTLD_NEXT;

		void *symbol = dlsym(handle, symbolName.c_str());
		if (symbol == nullptr && !(attr & LoadAttr::OPTIONAL)) {
			ErrorHandler::fail("Could not find symbol ", symbolName);
		}
		return symbol;
	}
};

} // namespace tampi

#endif // SYMBOL_HPP

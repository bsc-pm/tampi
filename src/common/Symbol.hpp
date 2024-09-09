/*
	This file is part of Task-Aware MPI and is licensed under the terms contained in the COPYING and COPYING.LESSER files.

	Copyright (C) 2015-2024 Barcelona Supercomputing Center (BSC)
*/

#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <cassert>
#include <dlfcn.h>
#include <string_view>

#include "util/ErrorHandler.hpp"

namespace tampi {

//! \brief Utility class to store function prototypes
template <typename Ret = void, typename... Params>
class SymbolDecl {
public:
	//! The return type of the function
	typedef Ret ReturnTy;

	//! The complete function prototype
	typedef Ret SymbolTy(Params...);
};

//! \brief The attributes when loading symbols with Symbol::load
enum class SymbolAttr {
	//! The default behavior when loading symbols. The default
	//! policy loads the next occurence of the symbol (RTLD_NEXT)
	//! and the loading is mandatory
	Next = 0,
	//! Load the first occurence of the symbol (RTLD_DEFAULT)
	First,
};

//! Class that allows the dynamic loading of symbols at run-time
template <typename SymbolDecl>
class Symbol {
	using SymbolTy = typename SymbolDecl::SymbolTy;
	using ReturnTy = typename SymbolDecl::ReturnTy;

	//! The symbol name
	std::string_view _name;

	//! The loaded symbol or nullptr
	SymbolTy *_symbol;

public:
	Symbol(std::string_view name, bool preload = true) :
		_name(name), _symbol(nullptr)
	{
		if (preload)
			load();
	}

	//! \brief Load the symbol if not already loaded
	//!
	//! \param attr The attribute to load the symbol
	//! \param mandatory Whether the symbol is mandatory
	bool load(SymbolAttr attr = SymbolAttr::Next, bool mandatory = true)
	{
		// Do nothing if it was already loaded
		if (_symbol != nullptr)
			return true;

		void *handle = (attr == SymbolAttr::Next) ? RTLD_NEXT : RTLD_DEFAULT;

		_symbol = (SymbolTy *) dlsym(handle, _name.data());
		if (_symbol == nullptr && mandatory)
			ErrorHandler::fail("Could not find symbol ", _name);

		return (_symbol != nullptr);
	}

	//! \brief Indicate whether the symbol is loaded
	bool hasSymbol() const
	{
		return (_symbol != nullptr);
	}

	//! \brief Execute the function
	template <typename... Params>
	ReturnTy operator()(Params &&... params) const
	{
		assert(_symbol != nullptr);
		return (*_symbol)(std::forward<Params>(params)...);
	}
};

} // namespace tampi

#endif // SYMBOL_HPP

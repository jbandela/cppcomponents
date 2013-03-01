//          Copyright John R. Bandela 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstddef>
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <objbase.h>
#include <string>
#include "../../cross_compiler_error_handling.hpp"

// On Windows use stdcall
#define CROSS_CALL_CALLING_CONVENTION __stdcall

namespace jrb_interface{
	inline void* shared_malloc(std::size_t sz){
		void* ret = ::CoTaskMemAlloc(sz);
		if(!ret){
			throw std::bad_alloc();
		}
		return ret;

	}	
	inline void shared_free(void* ptr){
		return ::CoTaskMemFree(ptr);
	}

	template<class F>
	F load_module_function(std::string module, std::string func){
		// add a .dll extension
		module += ".dll";
		auto m = ::LoadLibraryA(module.c_str());
		if(!m){
			throw error_shared_function_not_found();
		}

		auto f = reinterpret_cast<F>(GetProcAddress(m, func.c_str()));
		if(!f){
			throw error_shared_function_not_found();
		}

		return f;
	}

}
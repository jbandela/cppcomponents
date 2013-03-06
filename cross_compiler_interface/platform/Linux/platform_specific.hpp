//          Copyright John R. Bandela 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstddef>
#include <stdlib.h>
#include <string>
#include <dlfcn.h>
#include <stdexcept>

// On Linux don't need explicit calling convention
#define CROSS_CALL_CALLING_CONVENTION 

namespace cross_compiler_interface{

	template<class F>
	F load_module_function(std::string module, std::string func){
			// add current directory to front and add .so extenstion
			module = "./" + module + ".so";
			auto m = dlopen(module.c_str(),RTLD_LAZY);
			if(!m){
				throw error_shared_function_not_found();
			}
			auto f = reinterpret_cast<F>(dlsym(m, func.c_str()));
			if(!f){
				throw error_shared_function_not_found();
			}

			return f;
	}

}
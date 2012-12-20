
#include <cstddef>
#include <stdlib.h>
#include <string>
#include <dlfcn.h>
#include <stdexcept>
#include "../../jrb_interface_error_handling.hpp"
// On Linux don't need explicit calling convention
#define CROSS_CALL_CALLING_CONVENTION 

namespace jrb_interface{
	inline void* shared_malloc(std::size_t sz){
		return ::malloc(sz);
	}	
	inline void shared_free(void* ptr){
		return ::free(ptr);
	}

	template<class F>
	F load_module_function(std::string module, std::string func){
			module = "./" + module;
			auto m = dlopen(module.c_str());
			if(!m){
				throw error_shared_function_not_found();
			}
			auto f = (F) dlsym(m, func.c_str());
			if(!f){
				throw error_shared_function_not_found();
			}

			return f;
	}

}
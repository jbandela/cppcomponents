
#include <cstddef>
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <objbase.h>
#include <string>

// On Windows use stdcall
#define CROSS_CALL_CALLING_CONVENTION __stdcall

namespace jrb_interface{
	inline void* shared_malloc(std::size_t sz){
		return ::CoTaskMemAlloc(sz);
	}	
	inline void shared_free(void* ptr){
		return ::CoTaskMemFree(ptr);
	}

	template<class F>
	F load_module_function(std::string module, std::string func){
		auto m = ::LoadLibraryA(module.c_str());
		if(!m){
			throw error_shared_function_not_found();
		}

		auto f = (F) GetProcAddress(m, func.c_str());
		if(!f){
			throw error_shared_function_not_found();
		}

		return f;
	}

}
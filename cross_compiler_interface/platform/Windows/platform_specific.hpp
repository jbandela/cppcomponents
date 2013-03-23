//          Copyright John R. Bandela 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <cstddef>
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <string>

// On Windows use stdcall
#define CROSS_CALL_CALLING_CONVENTION __stdcall

namespace cross_compiler_interface{

	class module{

		HMODULE m_;

		// Not copyable
		module(const module&);
		module& operator=(const module&);
	public:
		module(std::string m){
			// add a .dll extension
			m += ".dll";
			m_ = ::LoadLibraryA(m.c_str());
			if(!m_){
				throw error_unable_to_load_library();
			}


		};

		template<class F>
		F load_module_function(std::string func)const{
			auto f = reinterpret_cast<F>(::GetProcAddress(m_, func.c_str()));
			if(!f){
				throw error_shared_function_not_found();
			}

			return f;
		}

		~module(){
			::FreeLibrary(m_);
		};
	};

}
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
#define CROSS_CALL_EXPORT_FUNCTION __attribute__ ((visibility ("default")))

namespace cross_compiler_interface{


	class module{

		void* m_;

		// Not copyable
		module(const module&);
		module& operator=(const module&);

	public:
		// Movable
		module(module && other) : m_(other.m_){
			other.release();
		}

		module& operator=(module && other){
			m_ = other.m_;
			other.release();
			return *this;
		}

		module(std::string m):m_(nullptr){
			// if empty string, leave m_ as nullptr;
			if (m.empty()) return; 

            // if there is a / in the string leave it alone
            // otherwise add ./ to front and .so to end
            if(m.find('/')==std::string::npos){
                m =  "./" + m + ".so";
            }
			m_ = ::dlopen(m.c_str(),RTLD_LAZY);
			if(!m_){
				throw error_unable_to_load_library();
			}
		};

		template<class F>
		F load_module_function(std::string func)const{
			auto f = reinterpret_cast<F>(dlsym(m_, func.c_str()));
			if(!f){
				throw error_shared_function_not_found();
			}

			return f;
		}
		bool valid(){
			return m_ != nullptr;
		}
        void release(){
            m_ = nullptr;
        }

		~module(){
			if(m_) ::dlclose(m_);
		};
	};

}


//          Copyright John R. Bandela 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef CROSS_COMPILER_SAFE_STATIC_INITIALIZATION_HPP_06_28_2013
#define CROSS_COMPILER_SAFE_STATIC_INITIALIZATION_HPP_06_28_2013

// MSVC does not have C++11 thread-safe local static initialization (as of MSVC 2013)
// This uses std::call_once to achieve this
// However mingw gcc does not have std::call_once, but does have thread-safe local static initialization
// Idea for this from approach from http://silviuardelean.ro/2012/06/05/few-singleton-approaches/

// Used for std::forward
#include <utility>


#ifdef _MSC_VER

// Used for std::call_once
#include <atomic>


#endif 


namespace cross_compiler_interface{
	namespace detail{

		template<class T, class>
		class safe_static_init{

			
#ifdef _MSC_VER // For MSVC we use call_once because MSVC Meyers singleton not thread safe


			// This will point to the class
			static T* ptr_;
			static std::atomic_flag  setting_ptr_;

			struct functor{
				template<class... Parms>
				void operator()(Parms && ... p){
					static T t_{ std::forward<Parms>(p)... };
					ptr_ = &t_;
				}
			};
			 
		public:

			// Takes arguments and passes them on to T constructor and sets the ptr_
			template<class... P>
			static T& get(P && ... p){
				// Our call once flag
				static std::atomic_flag once = ATOMIC_FLAG_INIT;
				T* ret = nullptr;
				try{
					while (setting_ptr_.test_and_set());
					if (once.test_and_set() == false){
						functor f;
						f(std::forward<P>(p)...);
					}

					ret = ptr_;
					setting_ptr_.clear();
				}
				catch (...){
					setting_ptr_.clear();
					once.clear();
					throw;
				}
				assert(ret != nullptr);
				return *ret;
			}
#else // For other compilers besides MSVC (ie GCC and clang) that have support
		public:
			template<class... P>
			static T& get(P && ... p){
				// Meyers singleton - guaranteed safe by c++11
				static T t_{std::forward<P>(p)...};
				return t_;
			}

#endif
		};

#ifdef _MSC_VER
		template<class T, class U>
		T* safe_static_init<T, U>::ptr_ = nullptr;
		template<class T, class U>
		std::atomic_flag safe_static_init<T, U>::setting_ptr_ = ATOMIC_FLAG_INIT;
#endif

	}
}

#endif
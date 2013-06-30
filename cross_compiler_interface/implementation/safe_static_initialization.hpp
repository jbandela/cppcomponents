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
#include <mutex>


#endif 


namespace cross_compiler_interface{
	namespace detail{

		template<class T, class>
		class safe_static_init{

			
#ifdef _MSC_VER // For MSVC we use call_once because MSVC Meyers singleton not thread safe


			// This will point to the class
			static T* ptr_;

			// This will delete ptr at the end of program execution
			template<class P>
			friend struct safe_static_init_deleter;

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
				static std::once_flag once_;

				std::call_once(once_, functor{}, std::forward<P>(p)...);

				// Ptr is now initalized, return reference to underlying object
				return *ptr_;
			}
#else // For other compilers besides MSVC (ie GCC and clang) that have support
		public:
			template<class... P>
			static T& get(P && ... p){
				// Meyers singleton - guaranteed safe by c++11
				static T t_(std::forward<P>(p)...);
				return t_;
			}

#endif
		};

#ifdef _MSC_VER
		template<class T, class U>
		T* safe_static_init<T, U>::ptr_ = 0;
#endif

	}
}

#endif
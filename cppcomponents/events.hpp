#pragma once

#ifndef CPPCOMPONENTS_EVENTS_HPP_07_17_2013_
#define CPPCOMPONENTS_EVENTS_HPP_07_17_2013_

#include "cppcomponents.hpp"
#include <mutex>


namespace cppcomponents{

	template < class F,
		std::uint32_t d1,
		std::uint16_t d2,
		std::uint16_t d3,
		std::uint8_t d4,
		std::uint8_t d5,
		std::uint8_t d6,
		std::uint8_t d7,
		std::uint8_t d8,
		std::uint8_t d9,
		std::uint8_t d10,
		std::uint8_t d11 >
	struct event_delegate:public define_interface<d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11>{
	
		template<class T>
		struct Interface : public cross_compiler_interface::define_unknown_interface<T, typename event_delegate::uuid>{
			cross_compiler_interface::cross_function<Interface, 0, F> Invoke;

			Interface() : Invoke(this){}
		};
	
	
	};
	
	//template < class R,class... P,
	//	std::uint32_t d1,
	//	std::uint16_t d2,
	//	std::uint16_t d3,
	//	std::uint8_t d4,
	//	std::uint8_t d5,
	//	std::uint8_t d6,
	//	std::uint8_t d7,
	//	std::uint8_t d8,
	//	std::uint8_t d9,
	//	std::uint8_t d10,
	//	std::uint8_t d11 >
	//struct event_delegate<R(P...),d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11>:public define_interface<d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11>{
	//	R Invoke(P...);


	//	template<class T>
	//	struct Interfac

	//	typedef R(P...) signature;

	//	CPPCOMPONENTS_CONSTRUCT(event_delegate, Invoke);

	//};
	
	


	namespace detail{

		template<class Delegate>
		struct event_delegate_implementation : public cross_compiler_interface::implement_unknown_interfaces<event_delegate_implementation<Delegate>, Delegate::template Interface>{


			template<class F>
			event_delegate_implementation(F f){
				this->get_implementation<Delegate::template Interface>()->Invoke = f;
			}
		};
	}


	template<class D,class Delegate,class Add, class Remove>
	struct event{
		cross_compiler_interface::portable_base* p_;
		template<class I>
		event(const I* i) : p_(static_cast<const D*>(i)->get_portable_base()){}

		template<class F>
		std::int64_t operator +=(F f){
			auto t = new detail::event_delegate_implementation<Delegate>(f);
			cppcomponents::use<Delegate> d(t->get_implementation<Delegate::template Interface>()->get_use_interface(), true);
			Add addfunc(p_);
			return addfunc(d);
		}

		void operator -=(std::int64_t i){
			Remove removefunc(p_);
			removefunc(i);
		}

	};

	template<class Delegate>
	struct event_implementation{

		typedef std::vector<use<Delegate>> containter_t;
		containter_t delegates_;
		//std::mutex evt_mutex_;

		std::int64_t add(use<Delegate> d){
			//std::lock_guard<std::mutex> lock(evt_mutex_);
			delegates_.push_back(d);
			return static_cast<std::int64_t>(delegates_.size()-1);

		}
		void remove(std::int64_t token){
			//std::lock_guard<std::mutex> lock(evt_mutex_);
			auto i = static_cast<std::size_t>(token);
			if (i < 0 || i >= delegates_.size()){
				throw cross_compiler_interface::error_out_of_range();
			}
			delegates_.erase(delegates_.begin() + i);
		}
		template<class... P>
		void raise(P&&... p){
			containter_t copy_delegates;
			{
				//std::lock_guard<std::mutex> lock(evt_mutex_);
				copy_delegates = delegates_;
			}

			for (auto& d : copy_delegates){
				d.Invoke(std::forward<P>(p)...);
			}
		}

	};

}

#define CPPCOMPONENTS_EVENT(Delegate,Add,Remove) cppcomponents::event<T,Delegate,decltype(Interface<T>::Add),decltype(Interface<T>::Remove)>



#endif
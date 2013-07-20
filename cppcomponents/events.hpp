#pragma once

#ifndef CPPCOMPONENTS_EVENTS_HPP_07_17_2013_
#define CPPCOMPONENTS_EVENTS_HPP_07_17_2013_

#include "cppcomponents.hpp"

// Some implementations don't have mutex (particularly mingw)
#ifndef CPPCOMPONENTS_NO_MUTEX

#include <mutex>

#endif


namespace cppcomponents{

	template < class F,
		class TUUID >
	struct event_delegate:public define_interface<TUUID>{
	
		template<class T>
		struct Interface : public cross_compiler_interface::define_unknown_interface<T, typename event_delegate::uuid>{
			cross_compiler_interface::cross_function<Interface, 0, F,cross_compiler_interface::detail::dummy_function<F>> Invoke;

			Interface() : Invoke(this){}
		};
	
	
	};
	
	namespace detail{

		template<class Delegate,class F>
		struct event_delegate_implementation{};

		template < class R, class... P,
			class TUUID, class F >
		struct event_delegate_implementation<event_delegate<R(P...),TUUID>,F >
			: public cross_compiler_interface::implement_unknown_interfaces<
			event_delegate_implementation<event_delegate<R(P...), TUUID>, F >, 
			event_delegate<R(P...), TUUID>::template Interface >
		{
			typedef event_delegate<R(P...), TUUID> delegate_t;
			F f_;


			R Invoker(P... p) {
				return f_(p...);
			}

			event_delegate_implementation(F f):f_(f){
				this->template get_implementation<delegate_t::template Interface>()->Invoke.template set_mem_fn < event_delegate_implementation,
					&event_delegate_implementation::Invoker>(this);
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
			std::unique_ptr < detail::event_delegate_implementation<Delegate,F> > t(new detail::event_delegate_implementation<Delegate,F>(f));
			cppcomponents::use<Delegate> d(t->template get_implementation<Delegate::template Interface>()->get_use_interface(), false);
			t.release();
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

#ifndef CPPCOMPONENTS_NO_MUTEX
		std::mutex evt_mutex_;
#endif

		std::int64_t add(use<Delegate> d){

#ifndef CPPCOMPONENTS_NO_MUTEX
			std::lock_guard<std::mutex> lock(evt_mutex_);
#endif

			delegates_.push_back(d);
			return static_cast<std::int64_t>(delegates_.size()-1);

		}
		void remove(std::int64_t token){
#ifndef CPPCOMPONENTS_NO_MUTEX
			std::lock_guard<std::mutex> lock(evt_mutex_);
#endif			
			auto i = static_cast<std::size_t>(token);
			if (i >= delegates_.size()){
				throw cross_compiler_interface::error_out_of_range();
			}
			delegates_[i] = nullptr;
		}
		template<class... P>
		void raise(P&&... p){
			containter_t copy_delegates;
			{
#ifndef CPPCOMPONENTS_NO_MUTEX
				std::lock_guard<std::mutex> lock(evt_mutex_);
#endif
				copy_delegates = delegates_;
			}

			for (auto& d : copy_delegates){
				if (d){
					d.Invoke(std::forward<P>(p)...);

				}
			}
		}

	};

}

#define CPPCOMPONENTS_EVENT(Delegate,Add,Remove) cppcomponents::event<T,Delegate,decltype(Interface<T>::Add),decltype(Interface<T>::Remove)>



#endif
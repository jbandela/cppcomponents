#pragma once

#ifndef INCLUDE_GUARD_CPPCOMPONENTS_EVENTS_HPP_07_17_2013_
#define INCLUDE_GUARD_CPPCOMPONENTS_EVENTS_HPP_07_17_2013_

#include "cppcomponents.hpp"

// Some implementations don't have mutex (particularly mingw)
#ifndef CPPCOMPONENTS_NO_MUTEX

#include <mutex>

#endif


namespace cppcomponents{

	template < class F,
		class TUUID >
	struct idelegate:public define_interface<TUUID>{
	
		template<class T>
		struct Interface : public cross_compiler_interface::define_unknown_interface<T, typename idelegate::uuid_type>{
			cross_compiler_interface::cross_function<Interface, 0, F,cross_compiler_interface::detail::dummy_function<F>> Invoke;

			Interface() : Invoke(this){}
		};
	
	
	};
	
	namespace detail{

		template<class Delegate,class F>
		struct idelegate_implementation{};

		template < class R, class... P,
			class TUUID, class F >
		struct idelegate_implementation<idelegate<R(P...),TUUID>,F >
			: public cross_compiler_interface::implement_unknown_interfaces<
			idelegate_implementation<idelegate<R(P...), TUUID>, F >, 
			idelegate<R(P...), TUUID>::template Interface >
		{
			typedef idelegate<R(P...), TUUID> delegate_t;
			F f_;


			R Invoker(P... p) {
				return f_(p...);
			}

			idelegate_implementation(F f) : f_(std::move(f)){
				this->template get_implementation<delegate_t::template Interface>()->Invoke.template set_mem_fn < idelegate_implementation,
					&idelegate_implementation::Invoker>(this);
			}
		};
	}

	template<class Delegate, class F>
	use<Delegate> make_delegate(F f){
		std::unique_ptr < detail::idelegate_implementation<Delegate, F> > t(new detail::idelegate_implementation<Delegate, F>(f));
		cppcomponents::use<Delegate> d(t->template get_implementation<Delegate::template Interface>()->get_use_interface(), false);
		t.release();
		return d;
	}
		
	template<class D,class Delegate,class Add, class Remove>
	struct event{
		cross_compiler_interface::portable_base* p_;
		template<class I>
		event(const I* i) : p_(static_cast<const D*>(i)->get_portable_base()){}

		template<class F>
		std::int64_t operator +=(F f){
			Add addfunc{p_};
			return addfunc(make_delegate<Delegate>(f));
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

#define CPPCOMPONENTS_EVENT(Delegate,Add,Remove) cppcomponents::event<CppComponentInterfaceExtrasT,Delegate,decltype(Interface<CppComponentInterfaceExtrasT>::Add),decltype(Interface<CppComponentInterfaceExtrasT>::Remove)>



#endif
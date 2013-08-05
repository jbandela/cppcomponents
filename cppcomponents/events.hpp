#pragma once

#ifndef INCLUDE_GUARD_CPPCOMPONENTS_EVENTS_HPP_07_17_2013_
#define INCLUDE_GUARD_CPPCOMPONENTS_EVENTS_HPP_07_17_2013_

#include "cppcomponents.hpp"
#include "implementation/uuid_combiner.hpp"
// Some implementations don't have mutex (particularly mingw)
#ifndef CPPCOMPONENTS_NO_MUTEX

#include <mutex>

#endif


namespace cppcomponents{


	namespace detail{
		typedef uuid <0x63c4cd27, 0x9e5e, 0x4a27, 0x879d, 0x3a303a3ee14c> delegate_uuid;
		template<class F>
		struct delegate_helper;

		template<class R, class... P>
		struct delegate_helper < R(P...)>{
			typedef combine_uuid<delegate_uuid, typename uuid_of<R>::uuid_type,
				typename uuid_of<P>::uuid_type...> uuid_type;

			typedef R return_type;

		};
	}

	template < class F,
		class TUUID = typename detail::delegate_helper<F>::uuid_type>
	struct delegate:public define_interface<TUUID>{
		typedef typename detail::delegate_helper<F>::return_type return_type;
		template<class T>
		struct Interface : public cross_compiler_interface::define_unknown_interface<T, typename delegate::uuid_type>{
			cross_compiler_interface::cross_function<Interface, 0, F,cross_compiler_interface::detail::dummy_function<F>> Invoke;

			Interface() : Invoke(this){}
		};
		template<class CppComponentInterfaceExtrasT> struct InterfaceExtras : delegate::template InterfaceExtrasBase<CppComponentInterfaceExtrasT>{

			template<class... P>
			return_type operator()(P&& ...p){
				return this->get_interface().Invoke(std::forward<P>(p)...);
			}

			InterfaceExtras(){}
		};

	
	};
	
	namespace detail{

		template<class Delegate,class F>
		struct delegate_implementation{};

		template < class R, class... P,
			class TUUID, class F >
		struct delegate_implementation<delegate<R(P...),TUUID>,F >
			: public cross_compiler_interface::implement_unknown_interfaces<
			delegate_implementation<delegate<R(P...), TUUID>, F >, 
			delegate<R(P...), TUUID>::template Interface >
		{
			typedef delegate<R(P...), TUUID> delegate_t;
			F f_;


			R Invoker(P... p) {
				return f_(p...);
			}

			delegate_implementation(F f) : f_(std::move(f)){
				this->template get_implementation<delegate_t::template Interface>()->Invoke.template set_mem_fn < delegate_implementation,
					&delegate_implementation::Invoker>(this);
			}
		};
	}

	template<class Delegate, class F>
	use<Delegate> make_delegate(F f){
		std::unique_ptr < detail::delegate_implementation<Delegate, F> > t(new detail::delegate_implementation<Delegate, F>(f));
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
					d(std::forward<P>(p)...);

				}
			}
		}

	};

}

#define CPPCOMPONENTS_EVENT(Delegate,Add,Remove) cppcomponents::event<CppComponentInterfaceExtrasT,Delegate,decltype(Interface<CppComponentInterfaceExtrasT>::Add),decltype(Interface<CppComponentInterfaceExtrasT>::Remove)>



#endif
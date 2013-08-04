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
		typedef uuid <0x63c4cd27, 0x9e5e, 0x4a27, 0x879d, 0x3a303a3ee14c> ifunction_uuid;
		typedef uuid <0x702af6dd, 0xe273 ,  0x45b3 , 0x939b , 0x0239e1e3a979> ifunction_return_type_uuid;
		template<class F>
		struct ifunction_helper;

		template<class R, class... P>
		struct ifunction_helper < R(P...)>{
			typedef combine_uuid<ifunction_uuid, typename uuid_of<R>::uuid_type, ifunction_return_type_uuid,
				typename uuid_of<P>::uuid_type...> uuid_type;

			typedef R return_type;

		};
	}

	template < class F,
		class TUUID = typename detail::ifunction_helper<F>::uuid_type>
	struct ifunction:public define_interface<TUUID>{
		typedef typename detail::ifunction_helper<F>::return_type return_type;
		template<class T>
		struct Interface : public cross_compiler_interface::define_unknown_interface<T, typename ifunction::uuid_type>{
			cross_compiler_interface::cross_function<Interface, 0, F,cross_compiler_interface::detail::dummy_function<F>> Invoke;

			Interface() : Invoke(this){}
		};
		template<class CppComponentInterfaceExtrasT> struct InterfaceExtras : ifunction::template InterfaceExtrasBase<CppComponentInterfaceExtrasT>{

			template<class... P>
			return_type operator()(P&& ...p){
				return this->get_interface().Invoke(std::forward<P>(p)...);
			}

			InterfaceExtras(){}
		};

	
	};
	
	namespace detail{

		template<class Delegate,class F>
		struct ifunction_implementation{};

		template < class R, class... P,
			class TUUID, class F >
		struct ifunction_implementation<ifunction<R(P...),TUUID>,F >
			: public cross_compiler_interface::implement_unknown_interfaces<
			ifunction_implementation<ifunction<R(P...), TUUID>, F >, 
			ifunction<R(P...), TUUID>::template Interface >
		{
			typedef ifunction<R(P...), TUUID> delegate_t;
			F f_;


			R Invoker(P... p) {
				return f_(p...);
			}

			ifunction_implementation(F f) : f_(std::move(f)){
				this->template get_implementation<delegate_t::template Interface>()->Invoke.template set_mem_fn < ifunction_implementation,
					&ifunction_implementation::Invoker>(this);
			}
		};
	}

	template<class Delegate, class F>
	use<Delegate> make_ifunction(F f){
		std::unique_ptr < detail::ifunction_implementation<Delegate, F> > t(new detail::ifunction_implementation<Delegate, F>(f));
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
			return addfunc(make_ifunction<Delegate>(f));
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
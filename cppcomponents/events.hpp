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


		};
		template<class F>
		struct delegate_return_helper;

		template<class R, class... P>
		struct delegate_return_helper < R(P...)>{

			typedef R return_type;

		};
	}

	template < class F,
		class TUUID = typename detail::delegate_helper<F>::uuid_type>
	struct delegate;
	template<class R, class... P,class TUUID>
	struct delegate<R(P...),TUUID>:public define_interface<TUUID>{
		typedef R return_type;
		R Invoke(P...);
		CPPCOMPONENTS_CONSTRUCT_TEMPLATE(delegate, Invoke);

		CPPCOMPONENTS_INTERFACE_EXTRAS(delegate){
			template<class... Parms>
			return_type operator()(Parms&& ...p)const{
				return this->get_interface().Invoke(std::forward<Parms>(p)...);
			}
		};


	
	};
	
	namespace detail{

		inline std::string delegate_imp_id(){ return "cppcomponents::uuid<0x0fee2970, 0x9586, 0x48d3, 0xaadf, 0x8c2c4cd26abe>"; }
		template<class Delegate,class F>
		struct delegate_implementation{};

		template < class R, class... P,
			class TUUID, class F >
		struct delegate_implementation<delegate<R(P...),TUUID>,F >
			: public implement_runtime_class< 	delegate_implementation<delegate<R(P...), TUUID>, F >, 
			runtime_class < delegate_imp_id, object_interfaces < delegate<R(P...), TUUID>>,factory_interface<NoConstructorFactoryInterface>>>
		{
			typedef delegate<R(P...), TUUID> delegate_t;
			F f_;


			R Invoke(P... p) {
				return f_(p...);
			}

			delegate_implementation(F f) : f_(std::move(f)){

			}
		};
	}

	template<class Delegate, class F>
	use<Delegate> make_delegate(F f){
		return detail::delegate_implementation<Delegate, F>::create(f).template QueryInterface<Delegate>();
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
#pragma once

#ifndef INCLUDE_GUARD_CPPCOMPONENTS_EVENTS_HPP_07_17_2013_
#define INCLUDE_GUARD_CPPCOMPONENTS_EVENTS_HPP_07_17_2013_

#include "cppcomponents.hpp"
#include "implementation/uuid_combiner.hpp"
#include <atomic>



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
		CPPCOMPONENTS_CONSTRUCT_TEMPLATE(delegate, Invoke)

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
				// Allow functions that return values to be used in void delegates
				 return (R) f_(p...);
			}

			delegate_implementation(F f) : f_(std::move(f)){

			}

		private:
			delegate_implementation(){}
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
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4673 4672)
#endif

	template<class Delegate>
	struct event_implementation{

		typedef std::vector<use<Delegate>> containter_t;
		containter_t delegates_;

		std::atomic<bool> evt_lock_;

		event_implementation() :evt_lock_(false){}

		struct spinlocker{
			std::atomic<bool>& v_;

			spinlocker(std::atomic<bool>& v) :v_(v){
				// spinlock
				while (v_.exchange(true));
			}
			~spinlocker(){
				v_.store(false);
			}

		private:
			spinlocker(const spinlocker&);
			spinlocker& operator=(const spinlocker&);
			spinlocker(const spinlocker&&);
			spinlocker& operator=(const spinlocker&&);
		};

		std::int64_t add(use<Delegate> d){

			spinlocker lock{ evt_lock_ };

			delegates_.push_back(d);
			return static_cast<std::int64_t>(delegates_.size()-1);

		}

		void remove(std::int64_t token){
			spinlocker lock{ evt_lock_ };

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
				spinlocker lock{ evt_lock_ };

				copy_delegates = delegates_;
			}

			for (auto& d : copy_delegates){
				if (d){
					d(std::forward<P>(p)...);

				}
			}
		}

	};
#ifdef _MSC_VER
#pragma warning(pop)
#endif
}

#define CPPCOMPONENTS_EVENT(Delegate,Add,Remove) cppcomponents::event<CppComponentInterfaceExtrasT,Delegate,decltype(Interface<CppComponentInterfaceExtrasT>::Add),decltype(Interface<CppComponentInterfaceExtrasT>::Remove)>



#endif
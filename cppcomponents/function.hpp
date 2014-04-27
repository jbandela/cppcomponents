#pragma once
#ifndef INCLUDE_GUARD_CPPCOMPONENTS_FUNCTION_HPP_04_27_2014_
#define INCLUDE_GUARD_CPPCOMPONENTS_FUNCTION_HPP_04_27_2014_
#include "cppcomponents.hpp"
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
	template<class R, class... P, class TUUID>
	struct delegate<R(P...), TUUID>:public define_interface<TUUID>{
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
		template<class Delegate, class F>
		struct delegate_implementation{};

		template < class R, class... P,
		class TUUID, class F >
		struct delegate_implementation<delegate<R(P...), TUUID>, F >
			: public implement_runtime_class< 	delegate_implementation<delegate<R(P...), TUUID>, F >,
			runtime_class < delegate_imp_id, object_interfaces < delegate<R(P...), TUUID>>, factory_interface<NoConstructorFactoryInterface>>>
		{
			typedef delegate<R(P...), TUUID> delegate_t;
			F f_;


			R Invoke(P... p) {
				// Allow functions that return values to be used in void delegates
				return (R)f_(p...);
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
	
	template<class F>
	using function = use<delegate<F>>;

	template<class Function, class F>
	Function make_function(F f){
		typedef typename Function::interface_t Delegate;
		return make_delegate<Delegate>(f);
	}

}

#endif
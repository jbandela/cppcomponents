#pragma once
#ifndef INCLUDE_GUARD_CPPCOMPONENTS_FUNCTION_HPP_04_27_2014_
#define INCLUDE_GUARD_CPPCOMPONENTS_FUNCTION_HPP_04_27_2014_
#include "cppcomponents.hpp"
#include "implementation/uuid_combiner.hpp"
#include <functional>

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

		inline const char* delegate_imp_id(){ return "cppcomponents::uuid<0x0fee2970, 0x9586, 0x48d3, 0xaadf, 0x8c2c4cd26abe>"; }
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
	
	// A std::function like object
	template<class T>
	class function;

	template<class R, class... P>
	class function<R(P...)>{
	public:

		typedef delegate<R(P...)> delegate_interface;
		typedef delegate_interface interface_t;
		typedef use<delegate_interface> delegate_type;
		
		typedef typename delegate_type::uuid_type uuid_type;

		function(){}

		template<class F>
		function(F f) :d_{ make_delegate<delegate_interface>(std::move(f)) }{}

		function(delegate_type d) :d_{ std::move(d) }{}

		function(const function& other) = default;
		function& operator=(const function& other) = default;

		function(function&& other) :d_{ std::move(other.d_) }{}
		function& operator=(const function&& other){
			d_ = std::move(other.d_);
			return *this;
		}

		void swap(function& other){
			delegate_type temp = std::move(d_);
			d_ = std::move(other.d_);
			other.d_ = std::move(temp);
		}

		explicit operator bool()const{
			return static_cast<bool>(d_);
		}

		template<class... T>
		R operator()(T&&... t)const{
			return d_(std::forward<T>(t)...);
		}

		const delegate_type& get_delegate()const{
			return d_;
		}

		delegate_type& get_delegate(){
			return d_;
		}
	private:
		delegate_type d_;

	};

	template<class F>
	bool operator ==(const function<F>& f, std::nullptr_t){
		return static_cast<bool>(f);
	}

	template<class F>
	bool operator == (std::nullptr_t, const function<F>& f){
		return static_cast<bool>(f);
	}

	template<class F>
	bool operator !=(const function<F>& f, std::nullptr_t){
		return !static_cast<bool>(f);
	}

	template<class F>
	bool operator != (std::nullptr_t, const function<F>& f){
		return !static_cast<bool>(f);
	}

	template<class Function, class F>
	Function make_function(F f){
		typedef typename Function::interface_t Delegate;
		return make_delegate<Delegate>(f);
	}


	template<class F>
	struct uuid_of<function<F>>{
		typedef typename function<F>::uuid_type uuid_type;
	};

	// also allow std::function
	template<class F>
	struct uuid_of<std::function<F>>{
		typedef typename function<F>::uuid_type uuid_type;
	};
}


namespace cppcomponents{


	template<class R, class... P>
	struct cross_conversion<cppcomponents::function<R(P...)>>{

		typedef cppcomponents::use < cppcomponents::delegate<R(P...)>> delegate_type;
		typedef cross_conversion<delegate_type> cc;
		// Required typedefs
		typedef cppcomponents::function<R(P...)>  original_type;
		typedef typename cc::converted_type converted_type;

		// static functions to do the conversion

		static converted_type to_converted_type(const original_type& o){
			return cc::to_converted_type(o.get_delegate());
		}
		static original_type to_original_type(const converted_type& c){
			return original_type{ cc::to_original_type(c) };
		}
	};


	// We specialize the default cross_conversion_return 
	// because by doing this we can avoid an AddRef/Release pair
	template<class R, class... P>
	struct cross_conversion_return<cppcomponents::function<R(P...)>>{
		typedef cppcomponents::function<R(P...)> original_type;
		typedef cross_conversion <original_type> cc;

		typedef cross_conversion_return<cppcomponents::use<cppcomponents::delegate<R(P...)>>> cc_ret;
		
		typedef original_type return_type;
		typedef typename cc_ret::converted_type converted_type;

		static void initialize_return(return_type&, converted_type&){}

		static void do_return(return_type && r, converted_type& c){
			cc_ret::do_return(std::move(r.get_delegate()), c);
		}
		static void finalize_return(return_type& r, converted_type& c){
			cc_ret::finalize_return(r.get_delegate(), c);
		}

	};

}
namespace std{

	template<class F>
	void swap(cppcomponents::function<F>& a, cppcomponents::function<F>& b){
		a.swap(b);
	}


}

#endif
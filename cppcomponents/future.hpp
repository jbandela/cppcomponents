#pragma once
#ifndef INCLUDE_GUARD_CPPCOMPONENTS_FUTURE_HPP_08_01_2013_
#define INCLUDE_GUARD_CPPCOMPONENTS_FUTURE_HPP_08_01_2013_

#include "events.hpp"

#include <future>
#include "implementation/uuid_combiner.hpp"
#include <cstdio>

namespace cppcomponents{



	namespace detail{
		template<typename F, typename W, typename R>
		struct helper
		{
			F f_;
			W w_;

			// By having the resulting future,
			// avoid having destructor block
			helper(F f, W w)
				: f_(f)
				, w_(w)
			{
			}


			R operator()()
			{
				f_.get();
				return w_(f_);
			}
		};
		typedef uuid < 0x59de594e , 0xb773 , 0x4e65 , 0xbaaf , 0x104b58c32abe> ifuture_uuid_base_t;

	}

	template<typename F, typename W>
	auto then(F f, W w) -> std::shared_future<decltype(w(f))>
	{
		// Emulation of future.then
		// Can change to return f.then(w) when available
		typedef decltype(w(f)) T;
		std::shared_future < decltype(w(f))> ret = std::async(std::launch::async, detail::helper<F, W, decltype(w(f))>(f, w));
		return ret;
	}



	template < class T >
	struct IFuture : public define_interface<combine_uuid<detail::ifuture_uuid_base_t,typename uuid_of<T>::uuid_type>>{
		typedef IFuture<T> IFutureTemplate;
		typedef delegate<void(use<IFuture<T>>)> delegate_type;
		typedef T value_type;
		T Get();
		void SetCompleted(use<delegate_type>);
		CPPCOMPONENTS_CONSTRUCT_TEMPLATE(IFuture, Get, SetCompleted);



		CPPCOMPONENTS_INTERFACE_EXTRAS(IFuture){

			CPPCOMPONENTS_W_PROPERTY(SetCompleted) Completed;

			CPPCOMPONENTS_INTERFACE_EXTRAS_CONSTRUCTOR(Completed);
		};

	};

	template<class T>
	std::shared_future<T> to_future(use < IFuture < T >> fut){
		auto shared_promise = std::make_shared < std::promise<T>>();
		auto f = [shared_promise](use < IFuture < T >> result)mutable{

			try{
				shared_promise->set_value(result.Get());
				shared_promise = nullptr;
			}
			catch (std::exception&){
				shared_promise->set_exception(std::current_exception());
			}
		};
		fut.Completed = make_delegate < delegate < void(use < IFuture < T >> result) >> (f);
		return shared_promise->get_future();
	}
	inline std::shared_future<void> to_future(use < IFuture < void >> fut){
		auto shared_promise = std::make_shared < std::promise<void>>();
		auto f = [shared_promise](use < IFuture < void >> result)mutable{

			try{
				result.Get();
				shared_promise->set_value();
				shared_promise = nullptr;
			}
			catch (std::exception&){
				shared_promise->set_exception(std::current_exception());
			}
		};
		fut.Completed = make_delegate < delegate < void(use < IFuture < void >> result) >> (f);
		return shared_promise->get_future();
	}

	template<class TIFuture, class T>
	use<TIFuture> make_ifuture(std::shared_future<T> f);

	namespace detail{

		template<class TIFuture, class TFuture>

		struct ifuture_implementation
			: public cross_compiler_interface::implement_unknown_interfaces <
			ifuture_implementation<TIFuture, TFuture >,
			TIFuture::template Interface 
			>
		{
			typedef TIFuture ifuture_t;
			typedef typename ifuture_t::value_type value_type;
			typedef typename ifuture_t::delegate_type delegate_type;
			TFuture f_;
			std::shared_future<void> resulting_;

			value_type get(){
				return f_.get();
			}
			void set_completed(use<delegate_type> d){
				auto func = [d](TFuture sfuture)mutable{

					d(make_ifuture<TIFuture>(sfuture));
				};
				resulting_ = then(f_, func);
			}


			ifuture_implementation(TFuture f) : f_(f){
				this->template get_implementation<ifuture_t::template Interface>()->Get.template set_mem_fn < ifuture_implementation,
					&ifuture_implementation::get>(this);
				this->template get_implementation<ifuture_t::template Interface>()->SetCompleted.template set_mem_fn < ifuture_implementation,
					&ifuture_implementation::set_completed>(this);
			}

			~ifuture_implementation(){

			}
		};
	}


	template<class TIFuture, class T>
	use<TIFuture> make_ifuture(std::shared_future<T> f){
		std::unique_ptr < detail::ifuture_implementation < TIFuture, std::shared_future<T >> > t(new  detail::ifuture_implementation < TIFuture, std::shared_future < T >> (f));
		cppcomponents::use<TIFuture> fut(t->template get_implementation<TIFuture::template Interface>()->get_use_interface(), false);
		t.release();
		return fut;
	}

};


#endif
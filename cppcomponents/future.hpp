//          Copyright John R. Bandela 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef INCLUDE_GUARD_0x0ccb9cdb_0x88a7_0x43fb_0xab23_0x8f06c85c1513
#define INCLUDE_GUARD_0x0ccb9cdb_0x88a7_0x43fb_0xab23_0x8f06c85c1513

#include "events.hpp"
#include <atomic>
#include <thread>
#include <chrono>
#include <iterator>

namespace cppcomponents{


	// Support for executors - See http://isocpp.org/files/papers/n3562.pdf for inspiration

	struct IExecutor
		: public define_interface<cppcomponents::uuid<0xd8036de3, 0x9266, 0x47c6, 0xa84b, 0x316f85290045>>
	{
		typedef cppcomponents::delegate < void() > ClosureType;

		void AddDelegate(use<ClosureType>);
		std::size_t NumPendingClosures();

		CPPCOMPONENTS_CONSTRUCT(IExecutor, AddDelegate, NumPendingClosures);
		CPPCOMPONENTS_INTERFACE_EXTRAS(IExecutor){

			template<class F>
			void Add(F f){
				this->get_interface().AddDelegate(make_delegate<ClosureType>(f));
			}

		};

	};

	struct IScheduledExecutor
		: define_interface < cppcomponents::uuid<0xeb1e8988, 0x22a0, 0x4ef9, 0x81d2, 0xb39122af1b50>>
	{
		typedef cppcomponents::delegate < void() > ClosureType;

		void AddDelegateAt(std::chrono::system_clock::time_point abs_time, ClosureType closure);
		void AddDelegateAfter(std::chrono::system_clock::duration rel_time, ClosureType closure);

		CPPCOMPONENTS_CONSTRUCT(IScheduledExecutor, AddDelegateAt, AddDelegateAfter);

		CPPCOMPONENTS_INTERFACE_EXTRAS(IScheduledExecutor){
			template<class F>
			void AddDelegateAt(std::chrono::system_clock::time_point abs_time, F closure){
				this->get_interface().AddDelegateAt(abs_time, make_delegate<ClosureType>(closure));
			}
			template<class F>
			void AddDelegateAfter(std::chrono::system_clock::duration rel_time, F closure){
				this->get_interface().AddDelegateAfter(rel_time, make_delegate<ClosureType>(closure));
			}
		};
	};
	struct executor_holder{
		typedef cppcomponents::delegate < void() > delegate_type;

		use<IExecutor> default_executor_;
		void add(use<delegate_type> d){
			if (default_executor_){
				default_executor_.Add(d);
			}
			else{
				d();
			}
		}
		void set_executor(use<IExecutor> e){
			default_executor_ = e;
		}

		executor_holder(use<IExecutor> e)
			: default_executor_{ e }
		{}

	};
	namespace detail{

		struct spinlocker{
			std::atomic<bool>& b_;

			spinlocker(std::atomic<bool>& b) :b_( b ){
				while (b_.exchange(true));
			}
			~spinlocker(){
				b_.store(false);
			}
		};

	}

	template<class T>
	struct storage_error_continuation{
		typedef cppcomponents::delegate < void()> Delegate;

		executor_holder executor_;

		error_code error_;

		typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type storage_;
		std::atomic<bool> storage_initialized_;
		std::atomic<bool> finished_;
		use<Delegate> continuation_;
		std::atomic<bool> has_continuation_;
		std::atomic<bool> continuation_run_;
		std::atomic<bool> set_called_;
		std::atomic<bool> set_continuation_called_;



		storage_error_continuation(use<IExecutor> e = nullptr)
			: executor_{ e },
			error_(0),
			storage_initialized_(false),
			finished_(false),
			has_continuation_(false),
			continuation_run_{ false },
			set_called_{ false },
			set_continuation_called_{ false }

		{	}



		bool finished()const{
			return finished_.load();
		}
		void set_error(cppcomponents::error_code ec){
			// Can only be called once
			if (set_called_.exchange(true))return;

			error_ = ec;
			finished_.store(true);
			run_continuation_once_if_ready();
		}

		void set(T t){
			// Can only be called once
			if (set_called_.exchange(true))return;

			try{
				void* data = &storage_;
				new(data) T(std::move(t));
				storage_initialized_.store(true);

			}
			catch (std::exception& e){
				error_ = cppcomponents::error_mapper::error_code_from_exception(e);
			}
			finished_.store(true);

			run_continuation_once_if_ready();
		}

		template<class F>
		void set_result_of(F& f){
			set(f());
		}

		void set_continuation(use<Delegate> c){
			{
				// Spin lock on set continuation
				detail::spinlocker locker{ set_continuation_called_ };

				bool continuation_run = continuation_run_.exchange(false);
				if (continuation_ && continuation_run == false){
					auto prev_continuation = continuation_;
					continuation_ = make_delegate<Delegate>([prev_continuation, c](){
						prev_continuation();
						c();
					});
				}
				else{
					continuation_ = c;
				}
			}
			has_continuation_.store(true);
			run_continuation_once_if_ready();
		}
		void set_continuation_and_executor(use<IExecutor> e, use<Delegate> c){
			{
				// Spin lock on set continuation
				detail::spinlocker locker{ set_continuation_called_ };
				bool continuation_run = continuation_run_.exchange(false);
				executor_.set_executor(e);
				if (continuation_ && continuation_run == false){
					auto prev_continuation = continuation_;
					continuation_ = make_delegate<Delegate>([prev_continuation, c](){
						prev_continuation();
						c();
					});
				}
				else{
					continuation_ = c;
				}
			}
			has_continuation_.store(true);
			run_continuation_once_if_ready();
		}

		template<class F>
		void set_continuation(F f){
			set_continuation(make_delegate<Delegate>(f));
		}
		template<class F>
		void set_continuation_and_executor(use<IExecutor> e, F f){
			set_continuation_and_executor(e, make_delegate<Delegate>(f));
		}

		void check_get()const {
			if (!finished_.load()){
				throw error_pending();;
			}
			cppcomponents::throw_if_error(error_);
			if (!storage_initialized_.load()){
				throw error_fail();
			}
		}
		error_code get_error_code(){
			if (!finished_.load()){
				throw error_pending();;
			}
			return error_;
		}

		T& get(){
			check_get();

			void* data = &storage_;
			auto& ret = *static_cast<T*>(data);
			return ret;
		}
		const T& get()const{
			check_get();
			const void* data = &storage_;
			return *static_cast<const T*>(data);
		}

		T && move(){
			return std::move(get());
		}


		void run_continuation_once_if_ready(){
			// If we are not finished, then return;
			if (finished_.load() == false) return;


			// Check if we have a continuation
			if (has_continuation_.load()){
				// Spin lock on set continuation
				detail::spinlocker locker{ set_continuation_called_ };

				// Check if it has been run already, and if not, run it
				if (continuation_run_.exchange(true) == false){
					executor_.add(continuation_);

					// Clear out the continuation,
					// That way we do not have to worry about circular
					// references in the continuation to us
					continuation_ = nullptr;
				}
			}
		}


		~storage_error_continuation(){

			// Destroy the storage if present
			if (storage_initialized_.load()){
				void* data = &storage_;
				static_cast<const T*>(data)->~T();
			}
		}
	};
	template<>
	struct storage_error_continuation<void>{
		typedef cppcomponents::delegate < void()> Delegate;
		executor_holder executor_;
		error_code error_;

		std::atomic<bool> finished_;
		use<Delegate> continuation_;
		std::atomic<bool> has_continuation_;
		std::atomic<bool> continuation_run_;
		std::atomic<bool> set_called_;
		std::atomic<bool> set_continuation_called_;

		storage_error_continuation(use<IExecutor> e = nullptr)
			: executor_{ e },
			error_(0),
			finished_(false),
			has_continuation_(false),
			continuation_run_{ false },
			set_called_{ false },
			set_continuation_called_{ false }
		{	}

		bool finished()const{
			return finished_.load();
		}
		void set_error(cppcomponents::error_code ec){
			// Can only be called once
			if (set_called_.exchange(true))return;

			error_ = ec;
			finished_.store(true);
			run_continuation_once_if_ready();
		}

		void set(){
			// Can only be called once
			if (set_called_.exchange(true))return;

			finished_.store(true);

			run_continuation_once_if_ready();
		}

		template<class F>
		void set_result_of(F& f){
			f();
			set();
		}

		void set_continuation(use<Delegate> c){
			{
			// Spin lock on set continuation
			detail::spinlocker locker{ set_continuation_called_ };
			bool continuation_run = continuation_run_.exchange(false);
			if (continuation_ && continuation_run == false){
				auto prev_continuation = continuation_;
				continuation_ = make_delegate<Delegate>([prev_continuation, c](){
					prev_continuation();
					c();
				});
			}
			else{
				continuation_ = c;
			}

			}

			has_continuation_.store(true);
			run_continuation_once_if_ready();
		}
		void set_continuation_and_executor(use<IExecutor> e, use<Delegate> c){
			{
				// Spin lock on set continuation
				detail::spinlocker locker{ set_continuation_called_ };

				bool continuation_run = continuation_run_.exchange(false);
				executor_.set_executor(e);
				if (continuation_ && continuation_run == false){
					auto prev_continuation = continuation_;
					continuation_ = make_delegate<Delegate>([prev_continuation, c](){
						prev_continuation();
						c();
					});
				}
				else{
					continuation_ = c;
				}

			}
			has_continuation_.store(true);
			run_continuation_once_if_ready();
		}

		template<class F>
		void set_continuation(F f){
			set_continuation(make_delegate<Delegate>(f));
		}
		template<class F>
		void set_continuation_and_executor(use<IExecutor> e, F f){
			set_continuation_and_executor(e, make_delegate<Delegate>(f));
		}
		void check_get()const {
			if (!finished_.load()){
				throw error_pending();;
			}
			cppcomponents::throw_if_error(error_);
		}
		error_code get_error_code(){
			if (!finished_.load()){
				throw error_pending();;
			}
			return error_;
		}

		void get()const{
			check_get();
		}



		void run_continuation_once_if_ready(){
			// If we are not finished, then return;
			if (finished_.load() == false) return;

			// Check if we have a continuation
			if (has_continuation_.load()){
				// Spin lock on set continuation
				detail::spinlocker locker{ set_continuation_called_ };


				// Check if it has been run already, and if not, run it
				if (continuation_run_.exchange(true) == false){
					executor_.add(continuation_);
					continuation_ = nullptr;
				}
			}
		}

	};









	template<class T>
	struct implement_future_promise;

	typedef cppcomponents::uuid<0xb23a22d4, 0xbafb, 0x4744, 0x8cf1, 0xc85c2d916358> uuid_base_t_IPromise;

	template<class T>
	struct IPromise : public cppcomponents::define_interface <combine_uuid<uuid_base_t_IPromise, typename uuid_of<T>::uuid_type>>{

		void Set(T t);
		void SetError(cppcomponents::error_code);
		typedef IPromise<T> IPromiseT;
		CPPCOMPONENTS_CONSTRUCT_TEMPLATE(IPromise, Set, SetError);

		CPPCOMPONENTS_INTERFACE_EXTRAS(IPromise){

			template<class F>
			void SetResultOf(F f){
				try{
					this->get_interface().Set(f());
				}
				catch (std::exception& e){
					auto ec = error_mapper::error_code_from_exception(e);
					this->get_interface().SetError(ec);
				}
			}

		};

	};
	template<>
	struct IPromise<void>:public cppcomponents::define_interface < cppcomponents::uuid<0x26c7caeb, 0x47d6, 0x42de, 0xa93c, 0x66f2ada2122b>>{

		void Set();
		void SetError(cppcomponents::error_code);

		CPPCOMPONENTS_CONSTRUCT(IPromise, Set, SetError);

		CPPCOMPONENTS_INTERFACE_EXTRAS(IPromise){

			template<class F>
			void SetResultOf(F f){
				try{
					f();
					this->get_interface().Set();
				}
				catch (std::exception& e){
					auto ec = error_mapper::error_code_from_exception(e);
					this->get_interface().SetError(ec);
				}
			}

		};

	};
	template<class T>
	struct IFuture;

	namespace detail{

		template<class R, class F, class Fut>
		void set_promise_result(use < IPromise < R >> p, F f, Fut fut){
			try{
				p.Set(f(fut));

			}
			catch (std::exception& e){
				auto ec = error_mapper::error_code_from_exception(e);
				p.SetError(ec);
			}
		}
		template<class F, class Fut>
		void set_promise_result(use < IPromise < void >> p, F f, Fut fut){
			try{
				f(fut);
				p.Set();

			}
			catch (std::exception& e){
				auto ec = error_mapper::error_code_from_exception(e);
				p.SetError(ec);
			}
		}
		template<class R, class Fut>
		void set_promise_result_from_future(use < IPromise < R >> p, Fut fut){
			try{
				p.Set(fut.Get());

			}
			catch (std::exception& e){
				auto ec = error_mapper::error_code_from_exception(e);
				p.SetError(ec);
			}
		}
		template<class Fut>
		void set_promise_result_from_future(use < IPromise < void >> p, Fut fut){
			try{
				fut.Get();
				p.Set();

			}
			catch (std::exception& e){
				auto ec = error_mapper::error_code_from_exception(e);
				p.SetError(ec);
			}
		}
		template<class T>
		use<IFuture<T>> unwrap(use < IFuture < T >> fut){
			return  fut;
		}
		template<class T>
		use<IFuture<T>> unwrap(use < IFuture < use < IFuture < T >> >> fut){
			auto p = implement_future_promise<T>::create().template QueryInterface<IPromise<T>>();
			fut.Then([p](use < IFuture < use < IFuture<T >> > > fut){
				fut.Get().Then([p](use < IFuture < T >> fut){
					detail::set_promise_result_from_future(p, fut);
				});
			});
			return p.template QueryInterface<IFuture<T>>();
		}

		template<class T>
		struct unwrap_helper{
			typedef use<IFuture<T>> type;
		};
		template<class T>
		struct unwrap_helper < use < IFuture<T >> >{
			typedef use<IFuture<T>> type;
		};

	}
	typedef cppcomponents::uuid<0xf2ff083f, 0xa305, 0x4f11, 0x98dc, 0x0b41344d1292> uuid_base_t_IFuture;
	template<class T>
	struct IFuture : public cppcomponents::define_interface < combine_uuid<uuid_base_t_IFuture, typename uuid_of<T>::uuid_type>>{

		typedef combine_uuid<detail::delegate_uuid, typename uuid_of<void>::uuid_type, uuid_base_t_IFuture, typename uuid_of<T>::uuid_type> u_t;

		typedef cppcomponents::delegate < void(use < IFuture<T >> ), u_t> CompletionHandler;

		T Get();
		bool Ready();
		cppcomponents::error_code ErrorCode();
		void SetCompletionHandlerRaw(use<CompletionHandler>);
		void SetCompletionHandlerAndExecutorRaw(use<IExecutor>, use<CompletionHandler>);

		CPPCOMPONENTS_CONSTRUCT_TEMPLATE(IFuture, Get, Ready, ErrorCode, SetCompletionHandlerRaw, SetCompletionHandlerAndExecutorRaw);

		CPPCOMPONENTS_INTERFACE_EXTRAS(IFuture){
			template<class F>
			void SetCompletionHandler(F f){
				this->get_interface().SetCompletionHandlerRaw(cppcomponents::make_delegate<CompletionHandler>(f));
			}
			template<class F>
			void SetCompletionHandlerAndExecutor(use<IExecutor> e, F f){
				this->get_interface().SetCompletionHandlerAndExecutorRaw(e, cppcomponents::make_delegate<CompletionHandler>(f));
			}

			template<class F>
			use < IFuture < typename std::result_of < F(use < IFuture<T >> )>::type >> Then(F f) {
				typedef typename std::result_of < F(use < IFuture<T >> )>::type R;
				auto iu = implement_future_promise<R>::create();
				auto p = iu.template QueryInterface<IPromise<R>>();
				this->get_interface().SetCompletionHandler([p, f](use < IFuture < T >> res)mutable{
					detail::set_promise_result(p, f, res);
				});
				return iu.template QueryInterface<IFuture<R>>();
			}
			template<class F>
			use < IFuture < typename std::result_of < F(use < IFuture<T >> )>::type >> Then(use<IExecutor> e, F f) {
				typedef typename std::result_of < F(use < IFuture<T >> )>::type R;
				auto iu = implement_future_promise<R>::create();
				auto p = iu.template QueryInterface<IPromise<R>>();
				this->get_interface().SetCompletionHandlerAndExecutor(e, [p, f](use < IFuture < T >> res)mutable{
					detail::set_promise_result(p, f, res);
				});
				return iu.template QueryInterface<IFuture<R>>();
			}

			typename detail::unwrap_helper<T>::type Unwrap(){
				return detail::unwrap(this->get_interface());
			}
		};



	};
	template<class T>
	using Future = use<IFuture<T>>;

	template<class T>
	using Promise = use<IPromise<T>>;


	// This is just a dummy function
	inline std::string implement_future_promise_id(){ return "cppcomponents::uuid<0x5373e27f, 0x84a7, 0x477a, 0x9486, 0x3c38371fb556>"; }

	template<class T>
	struct implement_future_promise
		: public cppcomponents::implement_runtime_class < implement_future_promise<T>, cppcomponents::runtime_class < implement_future_promise_id,
		object_interfaces<IFuture<T>, IPromise<T>>, factory_interface < NoConstructorFactoryInterface >> >
	{
		storage_error_continuation<T> sec_;
		typedef typename IFuture<T>::CompletionHandler CompletionHandler;

		error_code ErrorCode(){ return sec_.get_error_code(); }
		T Get(){ return sec_.get(); }
		bool Ready(){ return sec_.finished(); }
		void SetCompletionHandlerRaw(use<CompletionHandler> h){
			auto self = this->template QueryInterface<IFuture<T>>();
			sec_.set_continuation([self, h](){h(self); });
		}

		void SetCompletionHandlerAndExecutorRaw(use<IExecutor> e, use<CompletionHandler> h){
			auto self = this->template QueryInterface<IFuture<T>>();
			sec_.set_continuation_and_executor(e, [self, h](){h(self); });
		}


		void IPromise_Set(T t){ sec_.set(t); }
		void IPromise_SetError(cppcomponents::error_code e){ sec_.set_error(e); }

		implement_future_promise(){}

		implement_future_promise(use<IExecutor> e) : sec_{ e }
		{}

	};

	template<>
	struct implement_future_promise<void>
		: public cppcomponents::implement_runtime_class < implement_future_promise<void>, cppcomponents::runtime_class < implement_future_promise_id,
		object_interfaces<IFuture<void>, IPromise<void>>, factory_interface < NoConstructorFactoryInterface >> >
	{
		storage_error_continuation<void> sec_;
		typedef IFuture<void>::CompletionHandler CompletionHandler;
		error_code ErrorCode(){ return sec_.get_error_code(); }
		void Get(){ return sec_.get(); }
		bool Ready(){ return sec_.finished(); }
		void SetCompletionHandlerRaw(use<CompletionHandler> h){
			auto self = this->QueryInterface<IFuture<void>>();
			sec_.set_continuation([self, h](){h.Invoke(self); });
		}
		void SetCompletionHandlerAndExecutorRaw(use<IExecutor> e, use<CompletionHandler> h){
			auto self = this->QueryInterface<IFuture<void>>();
			sec_.set_continuation_and_executor(e, [self, h](){h(self); });
		}

		void IPromise_Set(){ sec_.set(); }
		void IPromise_SetError(cppcomponents::error_code e){ sec_.set_error(e); }

	};

	template<class T>
	use<IPromise<T>> make_promise(){
		return implement_future_promise<T>::create().template QueryInterface < IPromise < T > >();
	}

	template<class F>
	use<IFuture<typename std::result_of<F()>::type>> async(use<IExecutor> e, F f){
		typedef typename std::result_of<F()>::type R;

		auto iu = implement_future_promise<R>::create(e);
		auto p = iu.template QueryInterface < IPromise < R >>();

		e.Add([f, p]()mutable{
			try{
				p.SetResultOf(f);
			}
			catch (...){
				// No exceptions allowed to escape
			}
		});

		return p.template QueryInterface<IFuture<R>>();


	}
	template<class F>
	use<IFuture<typename std::result_of<F()>::type>> async(use<IExecutor> e, use<IExecutor> then_executor, F f){
		typedef typename std::result_of<F()>::type R;

		auto iu = implement_future_promise<R>::create(then_executor);
		auto p = iu.template QueryInterface < IPromise < R >>();

		e.Add([f, p]()mutable{
			try{
				p.SetResultOf(f);
			}
			catch (...){
				// No exceptions allowed to escape
			}
		});

		return p.template QueryInterface<IFuture<R>>();
	}

	template<class T>
	use<IFuture<T>> make_ready_future(T t){

		auto iu = implement_future_promise<T>::create();
		auto p = iu.template QueryInterface < IPromise < T >>();

		p.Set(t);
		return p.template QueryInterface<IFuture<T>>();
	}

	inline	use<IFuture<void>> make_ready_future(){

		auto iu = implement_future_promise<void>::create();
		auto p = iu.QueryInterface < IPromise < void >>();

		p.Set();
		return p.QueryInterface<IFuture<void>>();
	}

	template<class T>
	use<IFuture<T>> make_error_future(cppcomponents::error_code e){

		auto iu = implement_future_promise<T>::create();
		auto p = iu.template QueryInterface < IPromise < T >>();

		p.SetError(e);
		return p.template QueryInterface<IFuture<T>>();
	}

	namespace detail{
		template<class A, class B>
		use < IFuture <void> >
			when_all_imp(use < IFuture < A >> fa, use < IFuture < B >> fb){

				auto p = implement_future_promise<void>::create().QueryInterface<IPromise<void>>();

				auto fut = fa.Then(nullptr, [p, fb](use < IFuture < A >> )mutable{
					fb.Then(nullptr, [p](use < IFuture < B >> )mutable{
						p.Set();
					});
				});
				return p.QueryInterface<IFuture<void>>();

			}
	}
	template<class A, class B>
	use < IFuture < std::tuple < use < IFuture<A >> , use<IFuture<B >> > > >
		when_all(use < IFuture < A >> fa, use < IFuture < B >> fb){

			typedef std::tuple < use < IFuture < A >> , use < IFuture<B >> > tup_t;
			tup_t ret{ fa, fb };

			auto f = detail::when_all_imp(fa, fb);
			return f.Then(nullptr, [ret](use < IFuture < void >> )mutable{
				tup_t r{ std::move(ret) };
				return r;
			});

		}

	namespace detail{

		struct empty_then_functor{
			template<class T>
			void operator()(T &&)const{}
		};
	}

	template<class InputIterator>
	Future<void>
		when_all_range(InputIterator first, InputIterator last){



			if (first == last){
				return make_ready_future();
			}

			auto fut = first->Then(nullptr, detail::empty_then_functor{});
			++first;
			for (; first != last; ++first){
				auto f = when_all(fut, *first);
				fut = f.Then(nullptr, detail::empty_then_functor{});
			}

			return fut;
		}
	namespace detail{
		template<class F0>
		use < IFuture < void >> when_all_imp(F0 f0){
			return f0.Then(nullptr, [](F0){});
		}

		template<class F0, class F1, class... Futures>
		use < IFuture <void> >when_all_imp(F0 f0, F1 f1, Futures... futures){
			auto p = implement_future_promise<void>::create().QueryInterface<IPromise<void>>();

			auto fut0 = when_all_imp(f0, f1);
			auto fut1 = when_all_imp(futures...);
			auto fut2 = when_all_imp(fut0, fut1);
			return fut2;
		}
	}

	template<class... Futures>
	Future<void> when_all(Futures... futures){


		auto f = detail::when_all_imp(futures...);

		return f;

	}
	inline Future<void> when_all(){

		return make_ready_future();
	}


	namespace detail{
		template<class T>
		use<IFuture<T>> fork_future(use<IFuture<T>>& f){

			auto p1 = make_promise<T>();
			auto p2 = make_promise<T>();

			auto f1 = f;
			f1.Then([p1, p2](use<IFuture<T>> f){

				try{
					p1.Set(f.Get());
					p2.Set(f.Get());
				}
				catch (std::exception& e){
					p1.SetError(cross_compiler_interface::general_error_mapper::error_code_from_exception(e));
					p2.SetError(cross_compiler_interface::general_error_mapper::error_code_from_exception(e));
				}
			});

			f = p1.template QueryInterface<IFuture<T>>();
			return p2.template QueryInterface<IFuture<T>>();
		}
		struct set_promise_then_functor{
			use<IPromise<void>> p_;

			set_promise_then_functor(use < IPromise < void >> p) : p_(p){}
			template<class T>
			void operator()(T &&)const{
				p_.Set();
			}
		};

		inline void when_any_imp(use < IPromise < void >> p){
			p.Set();
		}


		template<class F>
		void when_any_imp(use < IPromise < void >> p, F f){
			f.Then(nullptr, set_promise_then_functor{ p });
		}
		template<class F, class... Futures>
		void when_any_imp(use < IPromise < void >> p, F f, Futures... futures){
			f.Then(nullptr, set_promise_then_functor{ p });
			when_any_imp(p, futures...);
		}


		template<class T>
		using decay_t = typename std::decay<T>::type;

	
	}


	// Note changed to return future void
	template<class... Futures>
	Future<void> when_any(Futures... f){
		auto p = implement_future_promise<void>::create().QueryInterface<IPromise<void>>();
		detail::when_any_imp(p, f...);
		auto fut = p.QueryInterface<IFuture<void>>();
		return fut;

	}
	// Note changed to return future void
	inline	Future<void> when_any(){
		auto p = implement_future_promise<void>::create().QueryInterface<IPromise<void>>();
		return make_ready_future();

	}
	template<class InputIterator>
	Future<void>
		when_any_range(InputIterator first, InputIterator last){

			if (first == last){
				return make_ready_future();
			}

			auto p = implement_future_promise<void>::create().QueryInterface<IPromise<void>>();

			for (; first != last; ++first){

				first->Then(nullptr, detail::set_promise_then_functor{ p });
			}
			auto fut = p.QueryInterface<IFuture<void>>();

			return fut;

		}

	template<class T>
	struct uuid_of<IFuture<T>>{
		typedef combine_uuid<uuid_base_t_IFuture, typename uuid_of<T>::uuid_type> uuid_type;
	};


}



#endif

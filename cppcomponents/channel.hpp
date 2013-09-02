//          Copyright John R. Bandela 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef INCLUDE_GUARD_CPPCOMPONENTS_CHANNEL_HPP_05_29_2013_
#define INCLUDE_GUARD_CPPCOMPONENTS_CHANNEL_HPP_05_29_2013_

#include "future.hpp"
#include "implementation/queue.hpp"

namespace cppcomponents{

	typedef cppcomponents::uuid<0x4d327f1c, 0x7515, 0x4dc5, 0xb1e4, 0x4ff5051d2beb> channel_base_uuid_t;
	template<class T>
	struct IChannel:define_interface<combine_uuid<channel_base_uuid_t,typename uuid_of<T>::uuid_type>{
		IFuture<void> Write(T);
		IFuture<void> WriteError(cppcomponents::error_code);
		IFuture<T> Read();

		CPPCOMPONENTS_CONSTRUCT_TEMPLATE(IChannel, Write, WriteError, Read);
	};


	inline std::string ChannelDummyId(){ return "cppcomponents::uuid<0x158d0d7c, 0xcaf2, 0x4301, 0xa192, 0x0e2509204e93>"; }

	template<class T, unsigned int size>
	struct implement_channel{
		std::array<T, size> buffer_;
		std::atomic<unsigned int> writer_pos_;
		std::atomic<unsigned int> reader_pos_;
		typedef IPromise<void> ipromise_void_t;
		low_lock_queue<use<ipromise_void_t>> reader_promise_queue_;
		low_lock_queue<use<ipromise_void_t>> writer_promise_queue_;

		implement_channel() : buffer_{ 0 },reader_pos_{0}{}

		IFuture<void> Write(T t){
			auto pos = writer_pos_.fetch_add(1);
			if (pos < buffer_.size()){
				pos = (reader_pos_.load() + pos) % buffer_.size();
				buffer_[pos] = std::move(t);
				use<ipromise_void_t> p;
				if (reader_promise_queue_.consume(p)){
					if (p){
						p.Set();
					}
				}
				return make_ready_future();
			}
			auto p = implement_future_promise<void>::create().QueryInterface<ipromise_void_t>();
			writer_promise_queue_.produce(p);
			auto f = p.QueryInterface<IFuture<void>>();
			auto self = this->template QueryInterface<IChannel<T>>();
			return f.Then([t, self](use < IFuture < void >> f)mutable{
				f.Get();
				use<ipromise_t> p;
				assert(self.reader_promise_queue_.consume(p));
				self = nullptr;
				assert(p);
				p.Set(t);
			});
		}
		use<IFuture<void>> WriteError(cppcomponents::error_code e){
			auto p = implement_future_promise<void>::create().QueryInterface<ipromise_void_t>();
			writer_promise_queue_.produce(p);
			auto f = p.QueryInterface<IFuture<void>>();
			auto self = this->template QueryInterface<IChannel<T>>();
			return f.Then([t, self](use < IFuture < void >> f)mutable{
				f.Get();
				use<ipromise_t> p;
				assert(self.reader_promise_queue_.consume(p));
				self = nullptr;
				assert(p);
				p.SetError(t);
			});
		}

		use<IFuture<T>> Read(){
			auto p = implement_future_promise<T>::create().template QueryInterface<ipromise_t>();
			reader_promise_queue_.produce(p);
			use<ipromise_void_t> p;
			if (write_promise_queue_.consume(p)){
				if (p){
					p.Set();
				}
			}
			auto f = p. template QueryInterface<IFuture<T>>();
			return f.Then([](use < IFuture < T >> f){
				return f.Get();
			});
		}


	};
	
	template<class T>
	struct implement_channel<T,0>:implement_runtime_class<
		implement_channel<T,0>,runtime_class<ChannelDummyId,object_interfaces<IChannel<T>>
		>>
	{
		typedef IPromise<T> ipromise_t;
		typedef IPromise<void> ipromise_void_t;
		low_lock_queue<use<ipromise_t>> reader_promise_queue_;
		low_lock_queue<use<ipromise_void_t>> writer_promise_queue_;

		IFuture<void> Write(T t){
			auto p = implement_future_promise<void>::create().QueryInterface<ipromise_void_t>();
			writer_promise_queue_.produce(p);
			auto f = p.QueryInterface<IFuture<void>>();
			auto self = this->template QueryInterface<IChannel<T>>();
			return f.Then([t,self](use < IFuture < void >> f)mutable{
				f.Get();
				use<ipromise_t> p;
				assert(self.reader_promise_queue_.consume(p));
				self = nullptr;
				assert(p);
				p.Set(t);
			});
		}
		use<IFuture<void>> WriteError(cppcomponents::error_code e){
			auto p = implement_future_promise<void>::create().QueryInterface<ipromise_void_t>();
			writer_promise_queue_.produce(p);
			auto f = p.QueryInterface<IFuture<void>>();
			auto self = this->template QueryInterface<IChannel<T>>();
			return f.Then([t, self](use < IFuture < void >> f)mutable{
				f.Get();
				use<ipromise_t> p;
				assert(self.reader_promise_queue_.consume(p));
				self = nullptr;
				assert(p);
				p.SetError(t);
			});
		}

		use<IFuture<T>> Read(){
			auto p = implement_future_promise<T>::create().template QueryInterface<ipromise_t>();
			reader_promise_queue_.produce(p);
			use<ipromise_void_t> p;
			if (write_promise_queue_.consume(p)){
				if (p){
					p.Set();
				}
			}
			auto f = p. template QueryInterface<IFuture<T>>();
			return f.Then([](use < IFuture < T >> f){
				return f.Get();
			});
		}




	};

	template<class T, unsigned int size>
	use<IChannel<T, size>> make_channel(){
		return implement_channel<T, size>::create().template QueryInterface<IChannel<T>>();
	}
}



#endif
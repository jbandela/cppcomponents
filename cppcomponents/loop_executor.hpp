//          Copyright John R. Bandela 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef INCLUDE_GUARD_CPPCOMPONENTS_LOOP_EXECUTOR_HPP_05_29_2013_
#define INCLUDE_GUARD_CPPCOMPONENTS_LOOP_EXECUTOR_HPP_05_29_2013_

// Loop executor from http://isocpp.org/files/papers/n3562.pdf

#include "implementation/queue.hpp"
#include "cppcomponents.hpp"
#include "events.hpp"
#include <thread>

namespace cppcomponents{
	struct ILoopExecutor : define_interface < cppcomponents::uuid<0x55d69d88, 0xadfd, 0x401f, 0x9b49, 0x4fe919700193>, IExecutor>
	{
		void Loop();
		void RunQueuedClosures();
		bool TryOneClosure();
		void MakeLoopExit();

		CPPCOMPONENTS_CONSTRUCT(ILoopExecutor, Loop, RunQueuedClosures, TryOneClosure, MakeLoopExit)
	};

	inline std::string LoopExecutorId(){ return "cppcomponents_loop_executor"; }
	
	typedef runtime_class<LoopExecutorId, object_interfaces<ILoopExecutor>> LoopExecutor_t;
	typedef use_runtime_class<LoopExecutor_t> LoopExecutor;

	struct implement_loop_executor : implement_runtime_class<implement_loop_executor,
		LoopExecutor_t>{
			std::atomic<bool> stopped_;
			typedef delegate < void()> ClosureType;

			low_lock_queue<use<ClosureType>> closures_;
			std::atomic<std::size_t> count_;

			implement_loop_executor() : stopped_{ false }, count_{ 0 }
			{}

			void AddDelegate(use<ClosureType> closure){
				closures_.produce(closure);
				count_.fetch_add(1);
			}
			std::size_t NumPendingClosures(){
				return count_.load();
			}

			void Loop(){
				stopped_.store(false);
				while (stopped_.load() == false){
					use<ClosureType> closure;
					if (closures_.consume(closure)){
						closure();
						count_.fetch_sub(1);
					}
					else{
						std::this_thread::yield();
					}
				}
			}
			void RunQueuedClosures(){
				stopped_.store(false);
				auto count = count_.load();
				use<ClosureType> closure;
				while (stopped_.load()==false && count && closures_.consume(closure)){
					closure();
					count_.fetch_sub(1);
					--count;
				}
			}
			bool TryOneClosure(){
				use<ClosureType> closure;
				if (closures_.consume(closure)){
					closure();
					count_.fetch_sub(1);
					return true;
				}
				return false;
			}
			void MakeLoopExit(){
				stopped_.store(true);
			}



	};

	CPPCOMPONENTS_REGISTER(implement_loop_executor)


}



#endif
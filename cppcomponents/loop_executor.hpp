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

	struct ILoopFactory :define_interface<cppcomponents::uuid<0x1544a71e, 0xcf66, 0x4483, 0xb3a1, 0x9d755a942b49>>
	{
		use<InterfaceUnknown> Create();
		use<InterfaceUnknown> CreateWithFunction(use < delegate<bool()> > func);

		CPPCOMPONENTS_CONSTRUCT(ILoopFactory,Create,CreateWithFunction)

			CPPCOMPONENTS_INTERFACE_EXTRAS(ILoopFactory){
				template<class F>
				use<InterfaceUnknown> TemplatedConstructor(F f){
					return this->get_interface().CreateWithFunction(make_delegate<delegate<bool()>>(f));
				}
			};
	};

	inline std::string LoopExecutorId(){ return "cppcomponents_loop_executor"; }
	
	typedef runtime_class<LoopExecutorId, object_interfaces<ILoopExecutor>, factory_interface<ILoopFactory>> LoopExecutor_t;
	typedef use_runtime_class<LoopExecutor_t> LoopExecutor;

	struct implement_loop_executor : implement_runtime_class<implement_loop_executor,
		LoopExecutor_t>{
			std::atomic<bool> stopped_;
			typedef delegate < void()> ClosureType;

			low_lock_queue<use<ClosureType>> closures_;
			std::atomic<std::size_t> count_;

			use<delegate<bool()>> func_;

			implement_loop_executor() : stopped_{ false }, count_{ 0 }
			{}

			implement_loop_executor(use<delegate<bool()>> f) :stopped_{ false }, count_{ 0 }, func_{ f }
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
						if (func_){
							if (func_() == false){
								MakeLoopExit();
								return;
							}
						}
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
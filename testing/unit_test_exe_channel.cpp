#include "unit_test_interface.h"
#include <gtest/gtest.h>
#include "../cppcomponents/channel.hpp"
#include "../cppcomponents/loop_executor.hpp"



void test_channel_1(){
	auto chan = cppcomponents::make_channel<int>();
	using namespace cppcomponents;
	chan.Write(1);
	chan.Write(2);
	chan.Write(3);

	chan.Read().Then([](use < IFuture < int >> f){
		EXPECT_EQ(1, f.Get());
	});
	chan.Read().Then([](use < IFuture < int >> f){
			EXPECT_EQ(2, f.Get());
	});
	chan.Read().Then([](use < IFuture < int >> f){
			EXPECT_EQ(3, f.Get());
	});
}

void test_channel_2(){
	using namespace cppcomponents;
	auto chan = make_channel<int>();

	chan.WriteError(error_fail::ec);

	chan.Read().Then([](use < IFuture < int >> f){
		EXPECT_THROW(f.Get(), error_fail);
	});


}


void test_channel_on_closed(){
	using namespace cppcomponents;
	int i = 0;
	{
	auto chan = make_channel<int>();
	chan.SetOnClosed([&i](){i = 1; });
	}


	EXPECT_EQ(1, i);

}


void test_channel_completed1(){

	using namespace cppcomponents;

	auto chan = make_channel<int>();

	chan.Write(1);
	chan.Complete();
	int value = 0;
	
	chan.Read().Then([&](Future<int> f){
		value = f.Get();

		EXPECT_EQ(true, chan.IsComplete());
	});

	EXPECT_EQ(1, value);


}

void test_channel_write_after_complete_throws(){

	using namespace cppcomponents;

	auto chan = make_channel<int>();

	chan.Complete();
	EXPECT_THROW(chan.Write(1).Get(), error_abort);
}
void test_channel_read_pending_writes_then_throw(){

	using namespace cppcomponents;

	auto chan = make_channel<int>();
	chan.Write(1);
	chan.Complete();

	auto fut = chan.Read().Then([&](Future<int> f)
	{
		EXPECT_EQ(1, f.Get());
		return chan.Read();
	}).Unwrap();

	EXPECT_EQ(true, fut.Ready());
	EXPECT_EQ(error_abort::ec, fut.ErrorCode());
}

void test_loop_with_function(){
	int i = 0;

	cppcomponents::LoopExecutor loop{ [&](){
		if (++i == 3){
			return false;
		}
		else{
			return true;
		}
	}
	};

	loop.Loop();

	EXPECT_EQ(i, 3);
}


void test_loop_executor_1(){

	using namespace cppcomponents;
	LoopExecutor executor;

	int i = 0;

	auto f = [&](){
		i++;
		executor.Add([&](){i++; });
	};
	executor.Add(f);
	executor.Add(f);
	executor.Add(f);

	executor.RunQueuedClosures();
	EXPECT_EQ(3, i);



	executor.Add([&](){executor.MakeLoopExit(); });

	executor.MakeLoopExit();

	executor.Loop();

	EXPECT_EQ(6, i);

	EXPECT_FALSE(executor.TryOneClosure());



}
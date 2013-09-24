#include "unit_test_interface.h"
#include <gtest/gtest.h>

void test_when_all_tuple(){
	auto e = launch_on_new_thread_executor::create().QueryInterface<cppcomponents::IExecutor>();

	std::atomic<bool> done(false);
	int t0 = 0;
	double t1 = 0.0;
	std::string t2;
	char t3 = 0;


	auto f1 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::microseconds(500));
		return 5;
	});
	auto f2 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::microseconds(500));
		return 6.0;
	});
	auto f3 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::microseconds(500));
		return std::string("Hello");
	});
	auto f4 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::microseconds(500));
		return 'a';
	});
	using namespace cppcomponents;


	auto fut = when_all(f1, f2, f3, f4).Then(
		[&](Future<void> )mutable{

			t0 = f1.Get();
			t1 = f2.Get();
			t2 = f3.Get();
			t3 = f4.Get();
			done.store(true);
	});



	while (done.load() == false);

	EXPECT_EQ(t0, 5);
	EXPECT_EQ(t1, 6.0);
	EXPECT_EQ(t2, std::string("Hello"));
	EXPECT_EQ(t3, 'a');
}

void test_when_all_tuple_empty(){

	auto f = cppcomponents::when_all();
	int i = 0;

	std::atomic<bool> done{ false };
	f.Then([&](cppcomponents::Future<void>)mutable{
		i = 5;
		done.store(true);
	});
	while (done.load() == false);

	EXPECT_EQ(i, 5);
}


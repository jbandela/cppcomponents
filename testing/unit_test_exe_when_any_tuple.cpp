#include "unit_test_interface.h"
#include <gtest/gtest.h>


void test_when_any_tuple(){
	auto e = launch_on_new_thread_executor::create().QueryInterface<cppcomponents::IExecutor>();

	std::atomic<bool> done(false);
	int t0 = 0;
	double t1 = 0.0;
	std::string t2;
	char t3 = 0;


	auto f1 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		return 5;
	});
	auto f2 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		return 6.0;
	});
	auto f3 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::microseconds(500));
		return std::string("Hello");
	});
	auto f4 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		return 'a';
	});
	using namespace cppcomponents;


	auto fut = when_any(f1, f2, f3, f4).Then(
		[&](Future<void>)mutable{


			if (f1.Ready()){
				t0 = f1.Get();
			}
			if (f2.Ready()){
				t1 = f2.Get();
			}
			if (f3.Ready()){
				t2 = f3.Get();
			}
			if (f4.Ready()){
				t3 = f4.Get();
			}
			done.store(true);
	});



	while (done.load() == false);

	EXPECT_EQ(t0, 0);
	EXPECT_EQ(t1, 0.0);
	EXPECT_EQ(t2, std::string("Hello"));
	EXPECT_EQ(t3, 0);

	std::atomic<bool> rest_done{ false };

	f1.Then([&](cppcomponents::Future<int> f){
		f2.Then([&](cppcomponents::Future<double> f){
			f4.Then([&](cppcomponents::Future<char> f){
				rest_done.store(true);
			});
		});
	});

	while (rest_done.load() == false);
}

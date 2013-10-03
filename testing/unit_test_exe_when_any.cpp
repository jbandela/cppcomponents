#include "unit_test_interface.h"
#include <gtest/gtest.h>


void test_when_any_vector(){
	auto e = launch_on_new_thread_executor::create().QueryInterface<cppcomponents::IExecutor>();

	std::atomic<bool> done(false);
	int t0 = 0;
	int t1 = 0;
	int t2 = 0;
	int t3 = 0;


	auto f1 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		return 5;
	});
	auto f2 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		return 6;
	});
	auto f3 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::microseconds(500));
		return 7;
	});
	auto f4 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		return 8;
	});
	using namespace cppcomponents;
	std::vector < use < IFuture < int >> > vec;
	vec.push_back(f1);
	vec.push_back(f2);
	vec.push_back(f3);
	vec.push_back(f4);

	auto fut = when_any(vec).Then(
		[&](Future<void>)mutable{

			auto& v = vec;
			if (v.at(0).Ready()){
				t0 = v.at(0).Get();
			}
			if (v.at(1).Ready()){
				t1 = v.at(1).Get();
			}
			if (v.at(2).Ready()){
				t2 = v.at(2).Get();
			}
			if (v.at(3).Ready()){
				t3 = v.at(3).Get();
			}

			done.store(true);
	});



	while (done.load() == false);

	EXPECT_EQ(t0, 0);
	EXPECT_EQ(t1, 0);
	EXPECT_EQ(t2, 7);
	EXPECT_EQ(t3, 0);

	std::atomic<bool> rest_done{ false };

	vec.at(0).Then([&](cppcomponents::Future<int> f){
		vec.at(1).Then([&](cppcomponents::Future<int> f){
			vec.at(3).Then([&](cppcomponents::Future<int> f){
				rest_done.store(true);
			});
		}); 
	});

	while (rest_done.load() == false);

}


void test_when_any_tuple_empty(){
	using namespace cppcomponents;
	auto f = cppcomponents::when_any();
	int i = 0;

	std::atomic<bool> done{ false };
	f.Then([&](Future<void>)mutable{
		i = 5;
		done.store(true);
	});
	while (done.load() == false);

	EXPECT_EQ(i, 5);
}
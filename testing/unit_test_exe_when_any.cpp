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
		std::this_thread::sleep_for(std::chrono::seconds(50));
		return 5;
	});
	auto f2 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::seconds(50));
		return 6.0;
	});
	auto f3 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::microseconds(500));
		return std::string("Hello");
	});
	auto f4 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::seconds(50));
		return 'a';
	});
	using namespace cppcomponents;


	auto fut = when_any(f1, f2, f3, f4).Then(
		[&](use < IFuture < std::tuple < use < IFuture < int >> , use < IFuture < double >> ,
		use < IFuture < std::string >>, use < IFuture < char >> >> > res)mutable{

			auto t = res.Get();
			
			if (std::get<0>(t).Ready()){
				t0 = std::get<0>(t).Get();
			}
			if (std::get<1>(t).Ready()){
				t1 = std::get<1>(t).Get();
			}
			if (std::get<2>(t).Ready()){
				t2 = std::get<2>(t).Get();
			}
			if (std::get<3>(t).Ready()){
				t3 = std::get<3>(t).Get();
			}
			done.store(true);
	});



	while (done.load() == false);

	EXPECT_EQ(t0, 0);
	EXPECT_EQ(t1, 0.0);
	EXPECT_EQ(t2, std::string("Hello"));
	EXPECT_EQ(t3, 0);
}


void test_when_any_vector(){
	auto e = launch_on_new_thread_executor::create().QueryInterface<cppcomponents::IExecutor>();

	std::atomic<bool> done(false);
	int t0 = 0;
	int t1 = 0;
	int t2 = 0;
	int t3 = 0;


	auto f1 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::seconds(50));
		return 5;
	});
	auto f2 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::seconds(50));
		return 6;
	});
	auto f3 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::microseconds(500));
		return 7;
	});
	auto f4 = cppcomponents::async(e, nullptr, [](){
		std::this_thread::sleep_for(std::chrono::seconds(50));
		return 8;
	});
	using namespace cppcomponents;
	std::vector < use < IFuture < int >> > vec;
	vec.push_back(f1);
	vec.push_back(f2);
	vec.push_back(f3);
	vec.push_back(f4);

	auto fut = when_any(vec.begin(), vec.end()).Then(
		[&](use < IFuture < std::vector < use < IFuture < int >> >> > res)mutable{

			auto v = res.Get();
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
}


void test_when_any_tuple_empty(){

	auto f = cppcomponents::when_any();
	int i = 0;

	std::atomic<bool> done{ false };
	f.Then([&](cppcomponents::use < cppcomponents::IFuture < std::tuple< >> >)mutable{
		i = 5;
		done.store(true);
	});
	while (done.load() == false);

	EXPECT_EQ(i, 5);
}
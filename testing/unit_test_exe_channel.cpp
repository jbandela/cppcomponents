#include "unit_test_interface.h"
#include <gtest/gtest.h>
#include "../cppcomponents/channel.hpp"



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
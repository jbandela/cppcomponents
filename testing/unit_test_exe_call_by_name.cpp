
void test_call_by_name(){}
#if 0
#include "unit_test_interface.h"
#include <gtest/gtest.h>

#include "../cppcomponents/call_by_name.hpp"
#include <sstream>




typedef cppcomponents::uuid<0x0a15fdee, 0xfdc8, 0x4055, 0xb0ed, 0x2c5cccc60058> MyUuid;

namespace cppcomponents{
	template<>
	struct call_by_name_conversion<MyUuid,std::string, int>{
		static int convert_from_any(const std::string& s){
			std::istringstream ss(s);
			int i = 0;
			ss >> i;
			return i;
		}
		static std::string convert_to_any(int i){
			std::ostringstream ss;
			ss << i;
			return ss.str();
		}
	};


	template<>
	struct call_by_name_conversion<MyUuid,std::string, std::string>{
		static std::string convert_from_any(const std::string& s){
			return s;
		}
		static std::string convert_to_any(const std::string&  s){
			return s;
		}
	};
}

void test_call_by_name(){
	Person p;
	typedef cppcomponents::ICallInterfaceByName<MyUuid, std::string> CBNInterface;
	auto ic = cppcomponents::make_call_by_name<CBNInterface>(p);

	auto v = ic.GetMethodNames();

	std::vector<std::string> parms;
	parms.push_back("104");

	auto res = ic.Call("SetAge", parms);

	parms.clear();
	res = ic.Call("GetAge", parms);

	EXPECT_EQ("104",res);

}

#endif
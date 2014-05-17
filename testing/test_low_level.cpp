#include "../cppcomponents/implementation/low_lowel.hpp"
#include <cppcomponents/cppcomponents.hpp>

cppcomponents::use<cppcomponents::InterfaceUnknown> iface;

struct test{

	int Test(){ return 5; }
	static int Test2(){ return 5; }
};

static_assert(std::is_same<int, cppcomponents::detail::return_type<decltype(&test::Test)>>::value, "error");
static_assert(std::is_same<int, cppcomponents::detail::return_type<decltype(&test::Test2)>>::value, "error");
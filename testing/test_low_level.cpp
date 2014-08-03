#include "../cppcomponents/implementation/low_level.hpp"
#include <cppcomponents/cppcomponents.hpp>

cppcomponents::use<cppcomponents::InterfaceUnknown> iface;

struct test{

	int Test(){ return 5; }
	static int Test2(){ return 5; }
};

static_assert(std::is_same<int, cppcomponents::detail::return_type<decltype(&test::Test)>>::value, "error");
static_assert(std::is_same<int, cppcomponents::detail::return_type<decltype(&test::Test2)>>::value, "error");



struct IPerson : cppcomponents::define_interface < cppcomponents::uuid<0x716fdc9a, 0xdb9c, 0x4bb3, 0x8a9b,0xd577415da686>>{

	int GetAge();
	void SetAge(int a);

	std::string GetName();
	void SetName(std::string);

	CPPCOMPONENTS_CONSTRUCT(IPerson, GetAge, SetAge, GetName, SetName)

	CPPCOMPONENTS_INTERFACE_EXTRAS(IPerson){

		//CPPCOMPONENTS_R_PROPERTY(GetAge) AgeReadOnly;
		//CPPCOMPONENTS_RW_PROPERTY(GetName,SetName) Name;
		//CPPCOMPONENTS_W_PROPERTY(SetAge) AgeWriteOnly;

		//CPPCOMPONENTS_INITIALIZE_PROPERTIES_EVENTS(AgeReadOnly, Name, AgeWriteOnly)


	};

};
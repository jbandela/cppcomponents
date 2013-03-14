//          Copyright John R. Bandela 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "DemoInterface.hpp"
#include <iostream>
using namespace std;

void test(cross_compiler_interface::use_interface<DemoInterface> iDemo){
		// Test out base
	cout<< "Hello from base = " << iDemo.hello_from_base() << endl;

	cout << "plus_5(5) = " << iDemo.plus_5(5) << endl;
	
	cout << "times_2point5(4) = " << iDemo.times_2point5(4) << endl;

	int i = 4;
	iDemo.double_referenced_int(i);
	cout << "double_referenced_int(i) where i=4 = " << i << endl;;

	cout << "count_characters(\"Hello World\") = " << iDemo.count_characters("Hello World") << endl;

	cout << "say_hello(\"John\") = " << iDemo.say_hello("John") << endl;
	
	cout << "Split \"This is a test\" into worlds " << endl;
	for(auto s:iDemo.split_into_words("This is a test")){
		cout << s << endl;
	}	
	
	try{
		iDemo.use_at_out_of_range("Test");

	}

	catch(std::out_of_range&){
		cout << "Caught out_of_range exception for use_at_out_of_range " << endl;
	}


	try{
		iDemo.not_implemented();

	}
	catch(cross_compiler_interface::error_not_implemented&){
		cout << "Caught cross_compiler_interface::error_not_implemented exception for not_implemented() " << endl;
	}



	cout << "Use passed in IGetName that returns \"My name is IGetName\"" << endl;
	cross_compiler_interface::implement_interface<IGetName> ign;
	ign.get_name = [](){return "My name is IGetName";};

	cout << "say_hello2 = " << iDemo.say_hello2(ign.get_use_interface()) << endl;
	
	std::vector<std::string> v;
	v.push_back("This");
	v.push_back("is");
	v.push_back("a");
	v.push_back("test");

	auto p = iDemo.get_string_at(v,3);

	cout << "String at " << p.first << " " << p.second << endl;

}


int main(){

	cross_compiler_interface::use_interface<DemoInterface> iDemo(cross_compiler_interface::create<DemoInterface>("DemoDLL","CreateDemoInterface"));


	test(iDemo);
	// Now test inheritance
	cross_compiler_interface::implement_interface<DemoInterface> imp_demo;
	imp_demo.hello_from_base = [](){return std::string("Hello from imp_demo");};

	imp_demo.set_runtime_parent(iDemo);

	test(imp_demo.get_use_interface());


}
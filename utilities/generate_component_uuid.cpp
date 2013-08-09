#include <iostream>
#include <iomanip>
#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>





int main(){


	boost::uuids::random_generator r;
	auto u = r();

	auto s = "0x" + boost::uuids::to_string(u);

	boost::algorithm::replace_all(s, "-", ", 0x");

	std::cout << "cppcomponents::uuid<" << s << ">";




}
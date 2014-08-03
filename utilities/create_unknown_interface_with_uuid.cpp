#include <iostream>
#include <iomanip>
#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>



int main(int argc, char** argv){
	std::string name;
	std::string parent;
	if(argc > 1){
		name = argv[1];
	}
	if(argc > 2){
		parent = argv[2];
	}
	boost::uuids::random_generator r;
	auto u = r();

	if(argc > 1){
		std::cout << "template<class T>\n";
		std::cout << "struct " << name << "\n";
		std::cout << "\t:public cppcomponents::define_unknown_interface<T,\n";
	}
	std::cout << "\t// {" << std::uppercase << u << "}" << std::endl;

	std::cout << "\tcppcomponents::uuid<\n";
	std::cout << "\t";



	int counter = 0;
	for(auto k:u){
		if(counter==0)
			std::cout << "0x";
		if(counter==4)
			std::cout << ",0x";
		if(counter==6)
			std::cout << ",0x";
		if(counter==8)
			std::cout << ",0x";
		if(counter>8)
			std::cout << ",0x";
		++counter;
		std::cout<< std::setw(2) << std::setfill('0') << std::hex << (unsigned int)k;

	}
	std::cout << std::endl;
	std::cout << "\t>";
	if(argc > 1){
		std::cout << "\n\t";
		if(parent.size()){
			std::cout << "," << parent;
		}
		std::cout << ">\n";
		std::cout << "{\n\n\n\n\n";
		std::cout << "\t" << name << "()\n\n\t{}\n};\n";
	}


}
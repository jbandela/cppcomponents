#include <iostream>
#include <iomanip>
#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>


void output_interface(const std::string& name){
    boost::uuids::random_generator r;
	auto u = r();

		std::cout << "struct " << name << "{\n";
	std::cout << "\t// {" << std::uppercase << u << "}" << std::endl;

	std::cout << "\t typedef cppcomponents::uuid<\n";
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
	std::cout << "\t> uuid;";
		std::cout << "\n\t";
		std::cout << "\n\n\n\n\n";
        std::cout << "\t" << "CPPCOMPONENTS_CONSTRUCT(" << name << ",);\n\n\n};\n";



}


int main(int argc, char** argv){
	std::string name;
	std::string options;
	if(argc > 1){
		name = argv[1];
	}
	if(argc > 2){
		options = argv[2];
	}
    if(name.size() == 0) name="TestComponent";

    output_interface("Interface" + name);
    if(options.find('f')==std::string::npos){
        std::cout << "typedef cppcomponents::DefaultFactoryInterface " << name << "FactoryInterface;\n\n";
    }else{
        output_interface(name + "FactoryInterface");

    }
    if(options.find('s')==std::string::npos){
        std::cout << "typedef cppcomponents::static_interfaces<> " << name << "StaticInterface;\n\n";
    }else{
        output_interface(name + "StaticInterface");

    }

    std::cout << "inline std::string " << name << "ComponentName(){return \"" << name << "\";}\n\n";

	std::cout << "typedef cppcomponents::runtime_class<" << name << "ComponentName" << "," << "Interface"
		<< name << "," << name << "FactoryInterface," << name << "StaticInterface>\n" << name << "_t;" << "\n";


    std::cout << "typedef cppcomponents::use_runtime_class<" << name << "_t> " << name << ";\n";





}
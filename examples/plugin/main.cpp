#include "plugin.hpp"
#include <iostream>
#include <map>
#include <sstream>
#include <iterator>

struct ImplementOutput:cppcomponents::implement_runtime_class<ImplementOutput,Output_t>
{
 	static void OutputString(cppcomponents::cr_string s){
		std::cout.write(s.data(),s.size());
	}
	static void OutputWString(cppcomponents::cr_wstring s){
		std::wcout.write(s.data(),s.size());
	}

};

CPPCOMPONENTS_REGISTER(ImplementOutput);

std::map<std::wstring, cppcomponents::use<ICommand>>& GetCommandMap(){
	static std::map<std::wstring, cppcomponents::use<ICommand>> m;
	return m;

}

bool LoadPlugin(std::string module){
	try{
		auto vec = CommandProvider::static_interface(module,CommandProviderId()).GetCommands();
		for(auto& ic:vec){
			GetCommandMap()[ic.GetCommandName()] = ic;
		}
		return true;
	}
	catch(std::exception&){
		return false;
	}
}

bool RunCommand(const std::wstring& command, const std::vector<std::wstring>& parms){
	auto iter =  GetCommandMap().find(command);
	if(iter == GetCommandMap().end()){
		return false;
	}
	return iter->second.Execute(parms);
}

bool Menu(){
	int i = 0;
	std::cout << "\nEnter -1 to quit, 1 to load a plugin, 2 to run a command, 3 to list commands\n";
	{
		std::string iline;
		std::getline(std::cin,iline);

		std::istringstream is{iline};
		is >> i;

	}
	
	if(i == -1){
		return false;
	}
	if(i == 1){
		std::cout << "\nEnter name of plugin\n";
		std::string p;
		std::getline(std::cin,p);
		if(LoadPlugin(p)){
			std::cout << "\n" << "Load plugin succeeded\n";
		}
		else{
			std::cout << "\n" << "Load plugin failed\n";
		}
		return true;
	}
	if(i == 2){
		std::cout << "\nEnter command to run followed by parameters\n";
		std::string line;
		std::getline(std::cin,line);
		// Really poor way to convert from string to wstring
		// Done here only because this is a simple example
		std::wstring wline{line.begin(),line.end()};
		std::wistringstream iss{wline};
		std::wstring command;
		iss >> command;
		std::vector<std::wstring> parms{std::istream_iterator<std::wstring,wchar_t>(iss),
				std::istream_iterator<std::wstring,wchar_t>{}};
		if(RunCommand(command,parms)){
			std::cout << "\nCommand run successfully\n";
		}
		else{
			std::cout << "\nCommand run unsuccessfully\n";
		}
		return true;



	}
	if(i == 3){
		for(auto& p:GetCommandMap()){
			std::wcout << L"\n" << p.first;
		}
		std::cout << "\n";
		return true;
	}
	else{
		return true;
	}

}

int main(){
	while(Menu());

}

#include "plugin.hpp"


inline std::string Command1Id(){return "Command1";}
typedef cppcomponents::runtime_class<Command1Id,cppcomponents::object_interfaces<ICommand>> Command1_t;


struct ImplementCommand1:cppcomponents::implement_runtime_class<ImplementCommand1,Command1_t>{

	std::wstring GetCommandName(){return L"Command1";}
	std::wstring GetCommandGUID(){return L"16e701c0-58fc-49c8-83cc-cd9975e7d341";}
	bool Execute(std::vector<std::wstring> args){

		std::wstring out = L"Command1 in plugin1 called with ";
		for(auto& a:args){
			out  = out + L" " + a;
		}
		Output::OutputWString(out);
		return true;
	}

};
inline std::string Command2Id(){return "Command2";}
typedef cppcomponents::runtime_class<Command2Id,cppcomponents::object_interfaces<ICommand>> Command2_t;



struct ImplementCommand2:cppcomponents::implement_runtime_class<ImplementCommand2,Command2_t>{

	std::wstring GetCommandName(){return L"Command2";}
	std::wstring GetCommandGUID(){return L"16e701c0-58fc-49c8-83cc-cd9975e7d342";}
	bool Execute(std::vector<std::wstring> args){

		std::wstring out = L"Command2 in plugin1 called with ";
		for(auto& a:args){
			out  = out + L" " + a;
		}
		Output::OutputWString(out);
		return true;

	}
	
};

struct ImplementCommandProvider
:cppcomponents::implement_runtime_class<ImplementCommandProvider,CommandProvider_t>
{

	static std::vector<cppcomponents::use<ICommand>> GetCommands(){
		std::vector<cppcomponents::use<ICommand>> ret;
		ret.push_back(ImplementCommand1::create().QueryInterface<ICommand>());
		ret.push_back(ImplementCommand2::create().QueryInterface<ICommand>());

		return ret;
	}


};

CPPCOMPONENTS_REGISTER(ImplementCommandProvider);

CPPCOMPONENTS_DEFINE_FACTORY()

#ifndef PLUGIN_HPP_04_01_2014_
#define PLUGIN_HPP_04_01_2014_
#include <cppcomponents/cppcomponents.hpp>

struct ICommand: cppcomponents::define_interface<cppcomponents::uuid<0xd064d789, 0x9f66, 0x4a6f, 0x8998, 0x655d7aa1551c>>
{
	std::wstring GetCommandName();
	std::wstring GetCommandGUID();
	bool Execute(std::vector<std::wstring> args);

	CPPCOMPONENTS_CONSTRUCT(ICommand, GetCommandName, GetCommandGUID,Execute);
};

struct ICommandProvider: cppcomponents::define_interface<cppcomponents::uuid<0x7af3cea5, 0x6fc7, 0x4a11, 0x8d63, 0x60aa963fd8a4>>
{
	std::vector<cppcomponents::use<ICommand>> GetCommands();
	CPPCOMPONENTS_CONSTRUCT(ICommandProvider, GetCommands);
};

inline std::string CommandProviderId(){return "CommandProvider";}

typedef cppcomponents::runtime_class<CommandProviderId,cppcomponents::factory_interface<cppcomponents::NoConstructorFactoryInterface>
,cppcomponents::static_interfaces<ICommandProvider>> CommandProvider_t;

typedef cppcomponents::use_runtime_class<CommandProvider_t> CommandProvider;


// We use this to make sure that the output all goes to the same place and we do not run into problems with multiple cout
struct IOutput:cppcomponents::define_interface<cppcomponents::uuid<0x0481f4cb, 0x33c8, 0x4b8a, 0x95d6, 0x4a45edc6ed3c>>
{
	void OutputString(cppcomponents::cr_string s);
	void OutputWString(cppcomponents::cr_wstring ws);

	CPPCOMPONENTS_CONSTRUCT(IOutput,OutputString,OutputWString);

};
inline std::string OutputId(){return "Output";}

typedef cppcomponents::runtime_class<OutputId,cppcomponents::factory_interface<cppcomponents::NoConstructorFactoryInterface>,
	cppcomponents::static_interfaces<IOutput>> Output_t;

typedef cppcomponents::use_runtime_class<Output_t> Output;


#endif

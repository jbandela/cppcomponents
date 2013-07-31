#include "library.h"

struct PersonImplementation:cppcomponents::implement_runtime_class<PersonImplementation,Person_t>
{

    std::string SayHello(){return "Hello World\n";}

    // Make sure you have at least 1 constructor
    // If not, the class will not be automatically registered
    // With the class factory

    PersonImplementation(){}

};

CPPCOMPONENTS_DEFINE_FACTORY();

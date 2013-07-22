#include "library.h"

struct PersonImplementation:cppcomponents::implement_runtime_class<PersonImplementation,Person_t>
{

    std::string SayHello(){return "Hello World\n";}

};

CPPCOMPONENTS_DEFINE_FACTORY(PersonImplementation);

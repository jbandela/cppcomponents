#include <cppcomponents/cppcomponents.hpp>

struct IPerson
:public cppcomponents::define_interface<cppcomponents::uuid<0xc618fd04,0xaa62,0x46e0,0xaeb8,0x6605eb4a1e64>>
{

    std::string SayHello();

    CPPCOMPONENTS_CONSTRUCT(IPerson,SayHello);



};

inline std::string PersonId(){return "library!Person";}

typedef cppcomponents::runtime_class<PersonId,cppcomponents::object_interfaces<IPerson>> Person_t;
typedef cppcomponents::use_runtime_class<Person_t> Person;

#include "simple_demo.h"
#include <iostream>


void test_externc(const cross_compiler_interface::module& m){

    auto Create_KVStore = m.load_module_function<HKVStore (CALLING_CONVENTION*)()>("Create_KVStore");
    auto Destroy_KVStore = m.load_module_function<void (CALLING_CONVENTION*) (HKVStore )>("Destroy_KVStore");
    auto Put = m.load_module_function<error_code (CALLING_CONVENTION*)(HKVStore,const char*, int32_t ,const char* , int32_t )>("Put");
    auto Get = m.load_module_function<error_code (CALLING_CONVENTION*)(HKVStore, const char*, int32_t,const char**,int32_t*,char*)>("Get");
    auto Delete = m.load_module_function<error_code (CALLING_CONVENTION*)(HKVStore, const char*, int32_t,char*)>("Delete");


    auto kv = Create_KVStore();
    std::string key = "key";
    std::string value = "value";
    Put(kv,key.data(),key.size(),value.data(),value.size());
    
    const char* pvalue = nullptr;
    int32_t count = 0;
    char b = 0;

    Get(kv,key.data(),key.size(),&pvalue,&count,&b);
    
    std::cout << "Value is " << std::string(pvalue,count) << "\n";

    Delete(kv,key.data(),key.size(),&b);

    Destroy_KVStore(kv);


    delete [] pvalue;



}


void test_compilervtables(const cross_compiler_interface::module& m){
    auto Create_KVStoreImplementation = m.load_module_function<IKVStore* (CALLING_CONVENTION*)()>("Create_KVStoreImplementation");

    auto ikv = Create_KVStoreImplementation();
    std::string key = "key";
    std::string value = "value";
    ikv->Put(key.data(),key.size(),value.data(),value.size());
    
    const char* pvalue = nullptr;
    int32_t count = 0;
    char b = 0;

    ikv->Get(key.data(),key.size(),&pvalue,&count,&b);
    
    std::cout << "Value is " << std::string(pvalue,count) << "\n";

    ikv->Delete(key.data(),key.size(),&b);

    ikv->Destroy();

    delete [] pvalue;

}

void test_programmervtables(const cross_compiler_interface::module& m){
    auto Create_KVStore2Implementation = m.load_module_function<IKVStore2* (CALLING_CONVENTION*)()>("Create_KVStore2Implementation");

    auto ikv = Create_KVStore2Implementation();
    std::string key = "key";
    std::string value = "value";
    ikv->vtable->Put(ikv,key.data(),key.size(),value.data(),value.size());
    
    const char* pvalue = nullptr;
    int32_t count = 0;
    char b = 0;

    ikv->vtable->Get(ikv,key.data(),key.size(),&pvalue,&count,&b);
    
    std::cout << "Value is " << std::string(pvalue,count) << "\n";

    ikv->vtable->Delete(ikv,key.data(),key.size(),&b);

    ikv->vtable->Destroy(ikv);

    delete [] pvalue;

}
void test_programmervtables2(const cross_compiler_interface::module& m){
    auto Create_KVStore2Implementation2 = m.load_module_function<IKVStore2* (CALLING_CONVENTION*)()>("Create_KVStore2Implementation2");
    
    IKVStore2UsageWrapper ikv(Create_KVStore2Implementation2());

    ikv.Put("key","value");

    std::cout << "testvtables2\n";
}

void test_interface(const cross_compiler_interface::module& m){

    auto ikv = cross_compiler_interface::create<InterfaceKVStore>(m,"Create_ImplementKVStore");

    std::string key = "key";
    std::string value = "value";
    ikv.Put(key,value);

    std::string value2;
    ikv.Get(key,&value2);

    std::cout << "Value is " << value2 << "\n";

    ikv.Delete(key);

    ikv.Destroy();




}
void test_unknown_interface(const cross_compiler_interface::module& m){

    auto ikv = cross_compiler_interface::create_unknown(m,"Create_ImplementKVStore2").QueryInterface<InterfaceKVStore2>();

    std::string key = "key";
    std::string value = "value";
    ikv.Put(key,value);

    std::string value2;
    ikv.Get(key,&value2);

    std::cout << "Value is " << value2 << "\n";

    ikv.Delete(key);





}

int main(){


    cross_compiler_interface::module m("simple_demo_dll");

    test_externc(m);
    test_compilervtables(m);
    test_programmervtables(m);
    test_interface(m);
    test_unknown_interface(m);

    test_programmervtables2(m);

}
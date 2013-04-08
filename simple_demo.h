#include "cross_compiler_interface/interface_unknown.hpp"

#if 0


struct KVStore{
    KVStore();
    ~KVStore();
    void Put(const std::string& key, const std::string& value);
    bool Get(const std::string& key,std::string* value);
    bool Delete(const std::string& key);
};

#endif


struct KVStore;
typedef KVStore* HKVStore;
using std::int32_t;
typedef std::int32_t error_code;
#define CALLING_CONVENTION __stdcall

extern "C"{
    HKVStore CALLING_CONVENTION Create_KVStore();
    void CALLING_CONVENTION Destroy_KVStore(HKVStore h);
    error_code CALLING_CONVENTION Put (HKVStore h,const char* key, int32_t key_count,const char* value, int32_t value_count);
    error_code CALLING_CONVENTION Get(HKVStore h, const char* key, int32_t key_count,const char** pvalue,int32_t* pvalue_count,char* breturn);
    error_code CALLING_CONVENTION Delete(HKVStore h, const char* key, int32_t key_count,char* breturn);
}

struct IKVStore{
    virtual error_code CALLING_CONVENTION Put (const char* key, int32_t key_count,const char* value, int32_t value_count) = 0;
    virtual error_code CALLING_CONVENTION Get(const char* key, int32_t key_count,const char** pvalue,int32_t* pvalue_count,char* breturn) = 0;
    virtual error_code CALLING_CONVENTION Delete(const char* key, int32_t key_count,char* breturn)=0;
    virtual void CALLING_CONVENTION Destroy() = 0;
};

struct IKVStore2;

struct IKVStoreVtable{
    error_code (CALLING_CONVENTION * Put) (IKVStore2* ikv, const char* key, int32_t key_count,const char* value, int32_t value_count);
    error_code (CALLING_CONVENTION *Get)(IKVStore2* ikv, const char* key, int32_t key_count,const char** pvalue,int32_t* pvalue_count,char* breturn);
    error_code (CALLING_CONVENTION *Delete)(IKVStore2* ikv, const char* key, int32_t key_count,char* breturn);
    void (CALLING_CONVENTION *Destroy)(IKVStore2* ikv);
};

struct IKVStore2{
    IKVStoreVtable* vtable;
};


struct simple_cross_function1_usage{
    IKVStore2* ikv;

    void operator()(std::string key, std::string value){
        auto ret = ikv->vtable->Put(ikv,key.data(),key.size(),value.data(),value.size());
        if(ret){
            throw std::runtime_error("Error in Get");
        }
    };

    simple_cross_function1_usage(IKVStore2* i):ikv(i){}
};

struct IKVStore2UsageWrapper{
    simple_cross_function1_usage Put;

    IKVStore2UsageWrapper(IKVStore2* ikv):Put(ikv){}
};

struct IKVStore2Derived:public IKVStore2{
    void* pput;

};

struct simple_cross_function1_implementation{
    std::function<void(std::string,std::string)> put;
    
    static error_code CALLING_CONVENTION Put_ (IKVStore2* ikv, const char* key,
        int32_t key_count,const char* value, int32_t value_count){
        try{
            std::string key(key,key_count);
            std::string value(value,value_count);
            auto ikvd = static_cast<IKVStore2Derived*>(ikv);
            auto& f = *static_cast<std::function<void(std::string,
                std::string)>*>(ikvd->pput);
            f(key,value);
            return 0;
        }
        catch(std::exception&){
            return -1;
        }
    }

    template<class F>
    void operator=(F f){
        put = f;
    }

    simple_cross_function1_implementation(IKVStore2Derived* ikvd){
        ikvd->pput = &put;
        ikvd->vtable->Put = &Put_;
    }
};

struct IKV2DerivedImplementationBase:public IKVStore2Derived{
    IKVStoreVtable vt;
    IKV2DerivedImplementationBase(){
        vtable = &vt;
    }
};

struct IKVStore2DerivedImplementation:public IKV2DerivedImplementationBase{
    simple_cross_function1_implementation Put;

    IKVStore2DerivedImplementation():Put(this){}
};



using cross_compiler_interface::cross_function;

template<class T>
struct InterfaceKVStore:public cross_compiler_interface::define_interface<T>{
    cross_function<InterfaceKVStore,0,void(std::string,std::string)> Put;
    cross_function<InterfaceKVStore,1,bool(std::string,cross_compiler_interface::out<std::string>)> Get;
    cross_function<InterfaceKVStore,2,bool(std::string)> Delete;
    cross_function<InterfaceKVStore,3,void()> Destroy;
    
    InterfaceKVStore()
        :Put(this),Get(this),Delete(this),Destroy(this)
    {}
};

template<class T>
struct InterfaceKVStore2
    :public cross_compiler_interface::define_interface_unknown<T,
    // {B781B4FF-995D-4122-842C-E14A4C0348CC}
    cross_compiler_interface::uuid<
    0xB781B4FF,0x995D,0x4122,0x84,0x2C,0xE1,0x4A,0x4C,0x03,0x48,0xCC
    >
    >
{

    typedef cross_compiler_interface::cr_string cr_string;
    cross_function<InterfaceKVStore2,0,void(cr_string,cr_string)> Put;
    cross_function<InterfaceKVStore2,1,bool(cr_string,cross_compiler_interface::out<std::string>)> Get;
    cross_function<InterfaceKVStore2,2,bool(cr_string)> Delete;
    
    InterfaceKVStore2()
        :Put(this),Get(this),Delete(this)
    {}


};

struct FunctionInterface{
    std::function<std::string()> SayHello;
    std::function<std::string(int)> SayMultipleHellos;
};
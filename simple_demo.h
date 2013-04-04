#include "cross_compiler_interface/interface_unknown.hpp"

#if 0


struct KVStore{
	KVStore(const std::string& store);
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
	HKVStore CALLING_CONVENTION Create_KVStore(const char* store);
	void CALLING_CONVENTION Destroy_KVStore(HKVStore h);

	error_code CALLING_CONVENTION Put (HKVStore h,const char* key, int32_t key_count,const char* value, int32_t value_count);
	error_code CALLING_CONVENTION Get(HKVStore h, const char* key, int32_t key_count,const char** pvalue,int32_t* pvalue_count,char* breturn);
	error_code CALLING_CONVENTION Delete(HKVStore h, const char* key, int32_t key_count,char* breturn);

};

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
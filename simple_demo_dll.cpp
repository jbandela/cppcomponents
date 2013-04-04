#include "simple_demo.h"
#include <map>
#include <algorithm>


// Extern C
struct KVStore{
	std::map<std::string,std::string> m_;
};


extern "C"{
	HKVStore CALLING_CONVENTION Create_KVStore(const char* store){
		try{
			return new KVStore;
		}
		catch(std::exception&){
			return nullptr;
		}

	}
	void CALLING_CONVENTION Destroy_KVStore(HKVStore h){
		delete h;
	}

	error_code CALLING_CONVENTION Put (HKVStore h,const char* key, int32_t key_count,const char* value, int32_t value_count){
		try{
			std::string key(key,key_count);
			std::string value(value,value_count);
			h->m_[key] = value;
			return 0;

		}
		catch(std::exception&){
			return -1;
		}
	}
	error_code CALLING_CONVENTION Get(HKVStore h, const char* key, int32_t key_count,const char** pvalue,int32_t* pvalue_count,char* breturn){
		try{
			std::string key(key,key_count);
			auto iter = h->m_.find(key);
			if(iter == h->m_.end()){
				*breturn = 0;
			}
			else{
				std::string value = iter->second;
				auto pc = new char[value.size()];
				std::copy(value.begin(),value.end(),pc);
				*pvalue_count = value.size();
				*pvalue = pc;

				return 0;
			}

		}
		catch(std::exception&){
			return -1;
		}

	}
	error_code CALLING_CONVENTION Delete(HKVStore h, const char* key, int32_t key_count,char* breturn){
		try{
			std::string key(key,key_count);
			auto iter = h->m_.find(key);
			if(iter == h->m_.end()){
				*breturn = 0;
			}
			else{
				h->m_.erase(iter);

				return 0;
			}

		}
		catch(std::exception&){
			return -1;
		}
	}

}

// Compiler Vtable

struct KVStoreImplementation:public IKVStore{
	std::map<std::string,std::string> m_;
	virtual error_code CALLING_CONVENTION Put (const char* key, int32_t key_count,const char* value, int32_t value_count) override{
		try{
			std::string key(key,key_count);
			std::string value(value,value_count);
			m_[key] = value;
			return 0;
		}
		catch(std::exception&){
			return -1;
		}
	}
	virtual error_code CALLING_CONVENTION Get(const char* key, int32_t key_count,const char** pvalue,int32_t* pvalue_count,char* breturn) override{
		try{
			std::string key(key,key_count);
			auto iter = m_.find(key);
			if(iter == m_.end()){
				*breturn = 0;
			}
			else{
				std::string value = iter->second;
				auto pc = new char[value.size()];
				std::copy(value.begin(),value.end(),pc);
				*pvalue_count = value.size();
				*pvalue = pc;

				return 0;
			}

		}
		catch(std::exception&){
			return -1;
		}

	}


	
	virtual error_code CALLING_CONVENTION Delete(const char* key, int32_t key_count,char* breturn)override{
		try{
			std::string key(key,key_count);
			auto iter = m_.find(key);
			if(iter == m_.end()){
				*breturn = 0;
			}
			else{
				m_.erase(iter);

				return 0;
			}

		}
		catch(std::exception&){
			return -1;
		}
	}

	virtual void CALLING_CONVENTION Destroy() override {
		delete this;
	}
	


};

extern "C"{
	IKVStore* CALLING_CONVENTION Create_KVStoreImplementation(const char* store){
		try{
			return new KVStoreImplementation;
		}
		catch(std::exception&){
			return nullptr;
		}

	}

}

// Programmer generated vtable

struct KVStore2Implementation:public IKVStore2{
	std::map<std::string,std::string> m_;

	static void CALLING_CONVENTION Destroy_(IKVStore2* ikv ){
		delete static_cast<KVStore2Implementation*>(ikv);
	}

	static error_code CALLING_CONVENTION Put_ (IKVStore2* ikv, const char* key, int32_t key_count,const char* value, int32_t value_count){
		try{
			std::string key(key,key_count);
			std::string value(value,value_count);
			static_cast<KVStore2Implementation*>(ikv)->m_[key] = value;
			return 0;

		}
		catch(std::exception&){
			return -1;
		}
	}
	static error_code CALLING_CONVENTION Get_(IKVStore2* ikv,  const char* key, int32_t key_count,const char** pvalue,int32_t* pvalue_count,char* breturn){
		try{
			std::string key(key,key_count);
			auto iter = static_cast<KVStore2Implementation*>(ikv)->m_.find(key);
			if(iter == static_cast<KVStore2Implementation*>(ikv)->m_.end()){
				*breturn = 0;
			}
			else{
				std::string value = iter->second;
				auto pc = new char[value.size()];
				std::copy(value.begin(),value.end(),pc);
				*pvalue_count = value.size();
				*pvalue = pc;

				return 0;
			}

		}
		catch(std::exception&){
			return -1;
		}

	}
	static error_code CALLING_CONVENTION Delete_(IKVStore2* ikv,  const char* key, int32_t key_count,char* breturn){
		try{
			std::string key(key,key_count);
			auto iter = static_cast<KVStore2Implementation*>(ikv)->m_.find(key);
			if(iter == static_cast<KVStore2Implementation*>(ikv)->m_.end()){
				*breturn = 0;
			}
			else{
				static_cast<KVStore2Implementation*>(ikv)->m_.erase(iter);

				return 0;
			}

		}
		catch(std::exception&){
			return -1;
		}
	}

	KVStore2Implementation(){
		vtable->Put = &Put_;
		vtable->Get = &Get_;
		vtable->Delete = &Delete_;
		vtable->Destroy = &Destroy_;
	}

};

extern "C"{
	IKVStore2* CALLING_CONVENTION Create_KVStore2Implementation(const char* store){
		try{
			return new KVStore2Implementation;
		}
		catch(std::exception&){
			return nullptr;
		}

	}

}
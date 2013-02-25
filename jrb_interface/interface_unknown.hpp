#include "jrb_interface.hpp"
#include "custom_cross_function.hpp"
namespace jrb_interface{
	struct uuid_base{
		std::uint32_t Data1;
		std::uint16_t Data2;
		std::uint16_t Data3;
		std::uint8_t Data4[8];
	};

	template<
		std::uint32_t d1,
		std::uint16_t d2,
		std::uint16_t d3,
		std::uint8_t d4,
		std::uint8_t d5,
		std::uint8_t d6,
		std::uint8_t d7,
		std::uint8_t d8,
		std::uint8_t d9,
		std::uint8_t d10,
		std::uint8_t d11>

	struct uuid{
		static bool compare(const uuid_base& u){

			return ( d1 == u.Data1 &&
				d2 == u.Data2 &&
				d3 == u.Data3 &&
				d4 == u.Data4[0] &&
				d5 == u.Data4[1] &&
				d6 == u.Data4[2] &&
				d7 == u.Data4[3] &&
				d8 == u.Data4[4] &&
				d9 == u.Data4[5] &&
				d10 == u.Data4[6] &&
				d11 == u.Data4[7] );



		}

		static uuid_base& get(){
			static uuid_base b = {d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11};

			return b;


		}
#ifdef _WIN32
		static bool compare(const GUID& u){

			return ( d1 == u.Data1 &&
				d2 == u.Data2 &&
				d3 == u.Data3 &&
				d4 == u.Data4[0] &&
				d5 == u.Data4[1] &&
				d6 == u.Data4[2] &&
				d7 == u.Data4[3] &&
				d8 == u.Data4[4] &&
				d9 == u.Data4[5] &&
				d10 == u.Data4[6] &&
				d11 == u.Data4[7] );



		}

		static GUID& get_windows_guid(){
			static GUID g = {d1,d2,d3,d4,d5,d6,d7,d8,d9,d10,d11};

			return g;


		}




#endif


	};


	template<class Iface, int Id>
	struct query_interface_cross_function
		:public custom_cross_function<Iface,Id,portable_base*(uuid_base*),error_code(portable_base*,uuid_base*,portable_base**),
		query_interface_cross_function<Iface,Id>>{
			typedef custom_cross_function<Iface,Id,portable_base*(uuid_base*),error_code(portable_base*,uuid_base*,portable_base**),
				query_interface_cross_function<Iface,Id>> base_t;
			typedef typename base_t::helper helper;

			portable_base* call_vtable_function(uuid_base* u){
				portable_base* r = 0;
				auto ret = this->get_vtable_fn()(this->get_portable_base(),u,&r);
				if(ret){
					//this->exception_from_error_code(ret);
					// if QI fails will return 0, don't throw exception
				}
				return r;
			}
			template<class C,class MF, MF mf>
			static error_code CROSS_CALL_CALLING_CONVENTION vtable_func_mem_fn(jrb_interface::portable_base* v,uuid_base* u,portable_base** r){
				helper h(v);
				try{
					*r = 0;
					C* f = h.template get_mem_fn_object<C>();
					*r = (f->*mf)(u);
					return 0;
				} catch(std::exception& e){
					return h.error_code_from_exception(e);
				}
			}
			static error_code CROSS_CALL_CALLING_CONVENTION vtable_func_mem_fn(jrb_interface::portable_base* v,uuid_base* u,portable_base** r){
				helper h(v);
				try{
					*r = 0;
					auto& f = h.get_function();
					if(!f){
						return h.forward_to_runtime_parent(u,r);
					}
					*r = f(u);
					return 0;
				} catch(std::exception& e){
					return h.error_code_from_exception(e);
				}
			}

			template<class F>
			void operator=(F f){
				this->set_function(f);

			}

			template<class T>
			query_interface_cross_function(T t):base_t(t){}

	};

		template<class Iface, int Id>
	struct addref_release_cross_function
		:public custom_cross_function<Iface,Id,std::uint32_t(),std::uint32_t(portable_base*),
		query_interface_cross_function<Iface,Id>>{
			typedef custom_cross_function<Iface,Id,std::uint32_t(),std::uint32_t(portable_base*),
				query_interface_cross_function<Iface,Id>> base_t;
			typedef typename base_t::helper helper;

			std::uint32_t call_vtable_function(){
				return this->get_vtable_fn()(this->get_portable_base());
			}
			template<class C,class MF, MF mf>
			static std::uint32_t CROSS_CALL_CALLING_CONVENTION vtable_func_mem_fn(jrb_interface::portable_base* v){
				helper h(v);
				try{
					C* f = h.template get_mem_fn_object<C>();
					return (f->*mf)();
				} catch(std::exception& e){
					return h.error_code_from_exception(e);
				}
			}
			static std::uint32_t CROSS_CALL_CALLING_CONVENTION vtable_func_mem_fn(jrb_interface::portable_base* v){
				helper h(v);
				try{
					auto& f = h.get_function();
					if(!f){
						return h.forward_to_runtime_parent();
					}

					return f();
				} catch(std::exception& e){
					return h.error_code_from_exception(e);
				}
			}

			template<class F>
			void operator=(F f){
				this->set_function(f);

			}

			template<class T>
			addref_release_cross_function(T t):base_t(t){}

	};

	//	IUnknown
	typedef uuid<0x00000000,0x0000,0x0000,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46> Unknown_uuid_t;
	template<bool b>
	struct InterfaceUnknown:public define_interface<b,3>{
		query_interface_cross_function<InterfaceUnknown,0> QueryInterfaceRaw;
		addref_release_cross_function<InterfaceUnknown,1> AddRef;
		addref_release_cross_function<InterfaceUnknown,2> Release;


		typedef Unknown_uuid_t uuid;

		template<class T>
		InterfaceUnknown(T t):
			QueryInterfaceRaw(t),AddRef(t),Release(t),InterfaceUnknown::base_t(t){} // Do nothing

	};

	template<bool b,int num_functions, class uuid_type, class Base = InterfaceUnknown<b> >
	struct define_interface_unknown:public Base{
		enum{base_sz = Base::sz};

		enum{sz = num_functions + base_sz};
		typedef define_interface_unknown base_t;

		typedef Base base_interface_t;

		typedef uuid_type uuid;

		template<class T>
		define_interface_unknown(T t):Base(t){}
	};



	template<class T>
	struct QIHelper{
		static portable_base* QueryInterfaceImp(uuid_base* u,portable_base* p){
			typedef typename T::uuid uuid_t;
			if(uuid_t::compare(*u)){
				return p;
			}
			else{
				typedef typename T::base_interface_t base_t;
				return QIHelper<base_t>::QueryInterfaceImp(u,p);
			}
		}


	};

	template<>
	struct QIHelper<InterfaceUnknown<true>>{
		static portable_base* QueryInterfaceImp(uuid_base* u,portable_base* p){
			typedef InterfaceUnknown<true>::uuid uuid_t;
			if(uuid_t::compare(*u)){
				return p;
			}
			else{
				return nullptr;
			}
		}

	};


}

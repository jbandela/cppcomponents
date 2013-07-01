//          Copyright John R. Bandela 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef CROSS_COMPILER_COMPONENT_HPP_05_29_2013
#define CROSS_COMPILER_COMPONENT_HPP_05_29_2013


#include "cross_compiler_introspection.hpp"
#include "implementation/safe_static_initialization.hpp"

#define CPPCOMPONENTS_CONSTRUCT CROSS_COMPILER_INTERFACE_CONSTRUCT_UNKNOWN_INTERFACE
#define CPPCOMPONENTS_CONSTRUCT_NO_METHODS CROSS_COMPILER_INTERFACE_CONSTRUCT_UNKNOWN_INTERFACE_NO_METHODS

namespace cross_compiler_interface{

    	template<class Iface>
	struct use:private portable_base_holder, public Iface::template Interface<use<Iface> >{ // Usage

		use(std::nullptr_t p = nullptr ):portable_base_holder(nullptr){}

		use(detail::reinterpret_portable_base_t<Iface::template Interface> r,bool bAddRef):portable_base_holder(r.get()){
			if(*this && bAddRef){
				this->AddRef();
			}
		}

		use(use_interface<Iface::template Interface> u,bool bAddRef):portable_base_holder(u.get_portable_base()){
			if(*this && bAddRef){
				this->AddRef();
			}

		}

		use(implement_interface<Iface::template Interface>& i,bool bAddRef):portable_base_holder(i.get_portable_base()){
			if(*this && bAddRef){
				this->AddRef();
			}

		}

		use(const use<Iface>& other):portable_base_holder(other.get_portable_base()){
			if(*this){
				this->AddRef();
			}
		}

		// Move constructor
		use(use<Iface>&& other):portable_base_holder(other.get_portable_base()){
			other.reset_portable_base();
		}

        // Construct from use_unkown
		use(const use_unknown<Iface::template Interface>& other):portable_base_holder(other.get_portable_base()){
			if(*this){
				this->AddRef();
			}
		}

		// Move constructor
 		use(use_unknown<Iface::template Interface>&& other):portable_base_holder(other.get_portable_base()){
			other.reset_portable_base();
		}

		use& operator=(const use<Iface>& other){
			// Note - order will deal with self assignment as we increment the refcount before decrementing
			if(other){
				other.AddRef();
			}
			if(*this){
				this->Release();
			}
			this->p_ = other.get_portable_base();
			static_cast<typename Iface::template Interface<use<Iface>>&>(*this) =  other;
			return *this;
		}
		// Move constructor
		use& operator=(use<Iface>&& other){
			// can't move to ourself
			assert(this != &other);
			if(*this){
				this->Release();
			}

			this->p_ = other.get_portable_base();
			static_cast<typename Iface::template Interface<use<Iface>>&>(*this) =  other;

			other.reset_portable_base();
			return *this;
		}
		template<class OtherIface>
		use<OtherIface> QueryInterface(){
			if(!*this){
				throw error_pointer();
			}
			typedef typename OtherIface::template Interface<use<OtherIface>>::uuid uuid_t;
			portable_base* r = this->QueryInterfaceRaw(&uuid_t::get());
			if(!r){
				throw error_no_interface();
			}

			// AddRef already called by QueryInterfaceRaw
			return use<OtherIface>(reinterpret_portable_base<OtherIface::template Interface>(r),false);

		}

		template<class OtherIface>
		use<OtherIface> QueryInterfaceNoThrow(){
			if(!*this){
				return nullptr;
			}
			typedef typename OtherIface::template Interface<use<OtherIface>>::uuid uuid_t;
			portable_base* r = this->QueryInterfaceRaw(&uuid_t::get());

			// AddRef already called by QueryInterfaceRaw
			return use<OtherIface>(reinterpret_portable_base<OtherIface::template Interface>(r),false);

		}

		~use(){
			if(*this){
				this->Release();
			}
		}


		portable_base* get_portable_base()const {
			return this->p_;
		}


		portable_base* get_portable_base_addref()const {
			auto r = get_portable_base();
			if(r){
				this->AddRef();
			}
			return r;
		}

		explicit operator bool()const{
			return get_portable_base()!=nullptr;
		}

		use_interface<Iface::template Interface> get_use_interface(){
			return use_interface<Iface::template Interface>(unsafe_portable_base_holder(get_portable_base()));
		}

		void reset_portable_base(){
			// This line prevents a release
			this->p_ = nullptr;


			use empty;
			*this = empty;
		}
		enum{num_functions = sizeof(typename Iface::template Interface<size_only>)/sizeof(cross_function<typename Iface::template Interface<size_only>,0,void()>)};

	private:


		// Simple checksum that takes advantage of the fact that 1+2+3+4...n = n(n+1)/2
		enum{checksum = sizeof(typename Iface::template Interface<checksum_only>)/sizeof(cross_function<InterfaceBase<checksum_only>,0,void()>)};

		// Simple check to catch simple errors where the Id is misnumbered uses sum of squares
		static_assert(checksum==(num_functions * (num_functions +1)*(2*num_functions + 1 ))/6,"The Id's for a cross_function need to be ascending order from 0, you have possibly repeated a number");

        // Hide AddRef and Release
        // We make this const because lifetime management would be 
        // analogous to delete in that you can delete a const
        uint32_t AddRef()const{
           return Iface::template Interface<use<Iface>>::AddRef();
        }

        uint32_t Release()const{
           return Iface::template Interface<use<Iface>>::Release();
        }


	};


	template<class T>
	struct cross_conversion<use<T>>{
		typedef use<T> original_type;
		typedef portable_base* converted_type;
		static converted_type to_converted_type(const original_type& s){
			return s.get_portable_base();
		}
		static  original_type to_original_type(converted_type c){
			return use<T>(reinterpret_portable_base<T::template Interface>(c),true);
		}

	};
	template<class T>
	struct cross_conversion_return<use<T>>{
		typedef cross_conversion<use<T>> cc;
		typedef typename cc::original_type return_type;
		typedef typename cc::converted_type converted_type;

		static void initialize_return(return_type& r, converted_type& c){
			
		}

		static void do_return(return_type&& r,converted_type& c){
            c = r.get_portable_base();
            r.reset_portable_base();
		}
		static void finalize_return(return_type& r,converted_type& c){
            r = use<T>(cross_compiler_interface::reinterpret_portable_base<T::template Interface>(c),false);
		}

	};

        template<class Iface > 
    struct type_information<use<Iface>>{
        enum{is_interface = 1}; 
        enum{is_unknown_interface = 0}; 
        static std::string name(){return type_name_getter< use_unknown<Iface::template Interface> >::get_type_name() ;} 
        enum{names_size = 1+use_interface<Iface::template Interface>::num_functions - use_interface<Iface::template Interface>::base_sz};
        static const char* (&names())[names_size]{return  type_name_getter< use_unknown<Iface::template Interface> >:: template get_type_names<names_size>();}
        typedef typename type_name_getter<use_unknown<Iface::template Interface>>::functions functions; 
        static_assert(functions::size == names_size-1,"Functions defined and functions specified to CROSS_COMPILER_INTERFACE_DEFINE_INTERFACE_INFORMATION macro does not match");
        typedef typename type_name_getter<use_unknown<Iface::template Interface>>::functions_ptrs_to_members_t functions_ptrs_to_members_t;
        static functions_ptrs_to_members_t& get_ptrs_to_members(){
            return type_name_getter<use_unknown<Iface::template Interface>>::get_ptrs_to_members();
        }

        template<class CF>
        static CF& get_cross_function(use<Iface> iface){
            return iface.*get_ptrs_to_members().template get<CF>();
        };
    };  
}
namespace cppcomponents{
    using cross_compiler_interface::use;
    using cross_compiler_interface::portable_base;
    using cross_compiler_interface::error_code;
    using cross_compiler_interface::type_list;
    using cross_compiler_interface::uuid;
    using cross_compiler_interface::uuid_base;
   
    struct InterfaceUnknown{

        template<class T>
        struct Interface:public cross_compiler_interface::InterfaceUnknown<T>{};
    };

    // Define a runtime_class
    template<std::string(*pfun_runtime_class_name)(),
        class DefaultInterface,
        class FactoryInterface,
        class StaticInterface, 
        class... OtherInterfaces>
    struct runtime_class{
        static  std::string get_runtime_class_name(){
            return pfun_runtime_class_name();
        }
    };

    // Enables implementing runtime class
    template<class Derived, class RC>
    struct implement_runtime_class{};

    namespace detail{

        template<class Iface, class T>
        void caster(use<Iface>& r, T& t){
            r = t.template QueryInterface<Iface>();
        }


        // Helpers for using constructors to automatically map interface
        template<class CF, class F>
        struct factory_to_constructor_helper{};

        template<class CF, class R, class... Parms>
        struct factory_to_constructor_helper<CF,R(Parms...)>{
            template<class ImpFactHelper, class MPS,class Interface>

            static void set(ImpFactHelper& helper, MPS& m,Interface& i){
                auto ptm = m.template get<CF>();
                (i.*ptm) = [&helper](Parms... p)->R{
                    R ret;
                    auto t = helper.activate_instance_parms(p...);
                    caster(ret,t);
                    return ret;
                };

            }
        };  

        template<class... CF>
        struct factory_to_constructor{};

        template<class First>
        struct factory_to_constructor<First>{
            template<class ImpFactHelper, class MPS,class Interface>
            static void set(ImpFactHelper& helper, MPS& m,Interface& i){
                factory_to_constructor_helper<First, typename First::function_signature>::set(helper,m,i);
            }
        };

        template<class First, class... Rest>
        struct factory_to_constructor<First,Rest...>{
            template<class ImpFactHelper, class MPS,class Interface>
            static void set(ImpFactHelper& helper, MPS& m,Interface& i){
                factory_to_constructor_helper<First, typename First::function_signature>::set(helper,m,i);
                factory_to_constructor<Rest...>::set(helper,m,i);
            }


        };

        template<class TypesList>
        struct forward_to_factory_to_constructor{};

        template<class... T>
        struct forward_to_factory_to_constructor<cross_compiler_interface::type_list<T...>>{
            typedef factory_to_constructor<T...> type;
        };


        template<class T>
        struct others_helper{
            template<class U>
            struct Interface:public T::template Interface<U>{};

        };


        // Copied from interface_unknown
		template<class Derived, class FirstInterface,  class... Interfaces>
		struct implement_unknown_interfaces_helper:public cross_compiler_interface::implement_interface<FirstInterface::template Interface>,public implement_unknown_interfaces_helper<Derived,Interfaces...>
		{

		};

		// An extra InterfaceUnknown is added by implement_unknown_interfaces to 
		// work around an MSVC bug, filter it out - it is extraneous since all these interfaces
		// inherit from InterfaceUnknown
		template<class Derived,  class FirstInterface>
		struct implement_unknown_interfaces_helper<Derived,FirstInterface,InterfaceUnknown> :public cross_compiler_interface::implement_interface<FirstInterface::template Interface>{

		};



       //  Copied from interface_unknown
        	template<class Derived, class... Interfaces>
	struct implement_unknown_interfaces{
	private:
		 //Adding an extra IUnknown fixes an internal compiler error when compiling with MSVC Milan
		 //When there is only 1 interface specified
		detail::implement_unknown_interfaces_helper<Derived,Interfaces...,InterfaceUnknown> i_;
	public:

		template<class ImpInterface>
		cross_compiler_interface::implement_interface<ImpInterface::template Interface>* get_implementation(){
			return &i_;
		}

	private:


		template<class First, class... Rest>
		struct helper{
			template<class T>
			static portable_base* qihelper(uuid_base* u,T* t){
				if(cross_compiler_interface::detail::qi_helper<cross_compiler_interface::implement_interface<First::template Interface>>::compare(u)){
					return static_cast<cross_compiler_interface::implement_interface<First::template Interface>*>(t)->get_portable_base();
				}
				else{
					return helper<Rest...>::qihelper(u,t);
				}
			}

			template<class T>
			static void set_mem_functions(T* t){
				auto p = t->template get_implementation<First>();
				p->QueryInterfaceRaw.template set_mem_fn<implement_unknown_interfaces,
					&implement_unknown_interfaces::QueryInterfaceRaw>(t);
				p->AddRef.template set_mem_fn<implement_unknown_interfaces,
					&implement_unknown_interfaces::AddRef>(t);
				p->Release.template set_mem_fn<implement_unknown_interfaces,
					&implement_unknown_interfaces::Release>(t);

				helper<Rest...>::set_mem_functions(t);
			}

		};
		template<class First>
		struct helper<First>{
			template<class T>
			static portable_base* qihelper(uuid_base* u,T* t){
				if(cross_compiler_interface::detail::qi_helper<cross_compiler_interface::implement_interface<First::template Interface>>::compare(u)){
					return static_cast<cross_compiler_interface::implement_interface<First::template Interface>*>(t)->get_portable_base();
				}
				else{
					return nullptr;
				}
			}
			template<class T>
			static void set_mem_functions(T* t){
				auto p = t->template get_implementation<First>();
				p->QueryInterfaceRaw.template set_mem_fn<implement_unknown_interfaces,
					&implement_unknown_interfaces::QueryInterfaceRaw>(t);
				p->AddRef.template set_mem_fn<implement_unknown_interfaces,
					&implement_unknown_interfaces::AddRef>(t);
				p->Release.template set_mem_fn<implement_unknown_interfaces,
					&implement_unknown_interfaces::Release>(t);
			}

		};


		std::atomic<std::size_t> counter_;

	public:
		portable_base* QueryInterfaceRaw(uuid_base* u){
			auto ret =  helper<Interfaces...>::qihelper(u,&i_);
			 //Need to increment reference count of successful query interface
			if(ret){
				++counter_;
			}
			return ret;
		}

		std::uint32_t AddRef(){
			counter_++;

			 //Truncate to 32bit, but since return is only for debugging thats ok
			return static_cast<std::uint32_t>(counter_);
		}
		std::uint32_t Release(){
			counter_--;
			if(counter_==0){
				delete static_cast<Derived*>(this);
				return 0;
			}
			 //Truncate to 32bit, but since return is only for debugging thats ok
			return static_cast<std::uint32_t>(counter_);
		}




		implement_unknown_interfaces():counter_(1){
			helper<Interfaces...>::set_mem_functions(this);
			cross_compiler_interface::object_counter::get().increment();
		}

		~implement_unknown_interfaces(){
			cross_compiler_interface::object_counter::get().decrement();
		}

		template<class... T>
		static use<InterfaceUnknown> create(T&&... t){
            using namespace std; // Need this for MSVC Milan bug

            try{
                std::unique_ptr<Derived> p(new Derived(std::forward<T>(t)...));

                use<InterfaceUnknown>piu(cross_compiler_interface::reinterpret_portable_base<InterfaceUnknown::Interface>(p->QueryInterfaceRaw(&cross_compiler_interface::Unknown_uuid_t::get())),false);

                p->Release();

                p.release();

                return piu;
            }
            catch(std::exception&){
                return nullptr;
            }

        }

	};


	use<InterfaceUnknown> create_unknown(const cross_compiler_interface::module& m,std::string func){
		typedef portable_base* (CROSS_CALL_CALLING_CONVENTION *CFun)();
		auto f = m.load_module_function<CFun>(func);
		return use<InterfaceUnknown>(cross_compiler_interface::reinterpret_portable_base<InterfaceUnknown::Interface>(f()),false);


	}

    template<class Derived, class FactoryInterface, class StaticInterface>
    struct implement_factory_static_helper
        :public implement_unknown_interfaces<Derived,FactoryInterface,StaticInterface>{

    };

    }


    // Template to help implement a runtime class
    // You inherit from this class, providing the name of your class in derived
    template<std::string(*pfun_runtime_class_name)(),class Derived, class DefaultInterface, class FactoryInterface, class StaticInterface, class... Others>
    struct implement_runtime_class<Derived,runtime_class<pfun_runtime_class_name,DefaultInterface,FactoryInterface,StaticInterface,Others...>>
        :public detail::implement_unknown_interfaces<Derived,DefaultInterface,Others...>
    {

        // The runtime class default interface
        typedef runtime_class<pfun_runtime_class_name,DefaultInterface,FactoryInterface,StaticInterface,Others...> runtime_class_t;
        cross_compiler_interface::implement_interface<DefaultInterface::template Interface>* default_interface(){
            return this->template get_implementation<DefaultInterface::template Interface>();

        }

        void map_default_implementation_to_member_functions(){

            this->template get_implementation<DefaultInterface>()->map_to_member_functions_no_prefix(static_cast<Derived*>(this));

        }

        implement_runtime_class(){
            static_cast<Derived*>(this)->map_default_implementation_to_member_functions();    
        }


        struct implement_factory_static_interfaces
            :public detail::implement_factory_static_helper<typename Derived::ImplementFactoryStaticInterfaces,FactoryInterface,StaticInterface>{

                typedef runtime_class<pfun_runtime_class_name,DefaultInterface,FactoryInterface,StaticInterface,Others...> runtime_class_t;

                cross_compiler_interface::implement_interface<FactoryInterface::template Interface>* factory_interface(){
                    return this->template get_implementation<FactoryInterface>();
                }

                cross_compiler_interface::implement_interface<StaticInterface::template Interface>* static_interface(){
                    return this->template get_implementation<StaticInterface>();
                }


                template<class... T>
                use<InterfaceUnknown> activate_instance_parms(T... t){
                    return Derived::create(t...).template QueryInterface<InterfaceUnknown>();
                }

                use<InterfaceUnknown> activate_instance(){
                    return Derived::create().template QueryInterface<InterfaceUnknown>();
                }
                implement_factory_static_interfaces(){


                    auto memp = cross_compiler_interface::type_information<cross_compiler_interface::implement_interface<FactoryInterface::template Interface>>::get_ptrs_to_members();
                    typedef typename detail::forward_to_factory_to_constructor<typename cross_compiler_interface::type_information<
                        cross_compiler_interface::implement_interface<FactoryInterface::template Interface>>::functions>::type f_t;
                    f_t::set(*this,memp,*factory_interface());
                    
                    static_interface()-> template map_to_static_functions_no_prefix<Derived>();
                }


        };

        typedef implement_factory_static_interfaces ImplementFactoryStaticInterfaces;

        static use<InterfaceUnknown> get_activation_factory(std::string s){
            if(s==runtime_class_t::get_runtime_class_name()){
                return implement_factory_static_interfaces::create();
            }
            else{
                return nullptr;
            }
        }


    };


    template<class... Imps>
    struct get_activation_factory_helper{
        template<class... I>
        struct helper{};

        template<class First, class... Rest>
        struct helper<First,Rest...>{
            static use<InterfaceUnknown> get_activation_factory(std::string hs){
                auto r = First::get_activation_factory(hs);
                if(r){
                    return r;
                }
                else{
                    return helper<Rest...>::get_activation_factory(hs);

                }
            }

        };

        template<class First>
        struct helper<First>{
            static use<InterfaceUnknown> get_activation_factory(std::string hs){
                auto r = First::get_activation_factory(hs);
                if(r){
                    return r;
                }
                else{
                    return nullptr;
                }
            }

        };
        static use<InterfaceUnknown> get_activation_factory(std::string hs){
            return helper<Imps...>::get_activation_factory(hs);
        }

    };


    template<class...Imps>
    error_code get_activation_factory(type_list<Imps...>, std::string activatibleClassId, portable_base** factory){
        try{
            auto r = get_activation_factory_helper<Imps...>::get_activation_factory(activatibleClassId);
            if(r){
                *factory = r.get_portable_base_addref();
                return 0;
            }
            else{
                return cross_compiler_interface::error_no_interface::ec;
            }
        }
        catch(std::exception& e){
            return cross_compiler_interface::general_error_mapper::error_code_from_exception(e);
        }
    }

    namespace detail{
        class runtime_class_name_mapper{

            typedef std::pair<std::string,std::string> p_t;
            std::vector<p_t> v_;

            std::string get_module_name_from_string(const std::string& s){
                auto iter = std::find(s.begin(),s.end(),'!');
                if(iter==s.end()) return std::string();
                return std::string(s.begin(),iter);

            }

        public:
            void add(std::string k, std::string v){
                v_.push_back(std::make_pair(k,v));
            }

            void finalize(){
                std::sort(v_.begin(),v_.end(),[](const p_t& a, const p_t& b){
                    return a.first < b.first;

                });

            }

            std::string match(const std::string& s){
                auto i = std::lower_bound(v_.begin(),v_.end(),s,[](const p_t& a,const std::string& b){
                    return a.first < b;

                });
                // Exact match
                if(i != v_.end()){
                    if (i->first == s)
                        return i->second;
                }
                // Check if already at beginning
                if(i == v_.begin()){
                    return get_module_name_from_string(s);
                }

                --i;
                if(s.substr(0,i->first.size()) == i->first){
                    return i->second;
                }

                return get_module_name_from_string(s);
            };


        };
    }

    detail::runtime_class_name_mapper& runtime_classes_map(){

        //static detail::runtime_class_name_mapper m_;
        //return m_;
		return cross_compiler_interface::detail::safe_static_init<
			detail::runtime_class_name_mapper, detail::runtime_class_name_mapper>::get();

    }


	struct DefaultFactoryInterface{

		// {7175F83C-6803-4472-8D5A-199E478BD8ED}
		typedef cross_compiler_interface::uuid<
			0x7175f83c, 0x6803, 0x4472, 0x8d, 0x5a, 0x19, 0x9e, 0x47, 0x8b, 0xd8, 0xed> uuid;

		use<InterfaceUnknown> Create();

		CPPCOMPONENTS_CONSTRUCT(DefaultFactoryInterface, Create);





	};        
	struct NoConstructorFactoryInterface{

		// {70844160-352C-4007-8BE2-D69FB415DE77}
		typedef cross_compiler_interface::uuid<
			0x70844160, 0x352c, 0x4007,  0x8b, 0xe2, 0xd6, 0x9f, 0xb4, 0x15, 0xde, 0x77> uuid;

		use<InterfaceUnknown> Create();

		CPPCOMPONENTS_CONSTRUCT_NO_METHODS(NoConstructorFactoryInterface);





	};


        struct DefaultStaticInterface{

            typedef cross_compiler_interface::uuid<
                // {465BEFAD-C805-4164-A7C8-84051A868B4D}
                0x465befad, 0xc805, 0x4164, 0xa7, 0xc8, 0x84, 0x5, 0x1a, 0x86, 0x8b, 0x4d
            > uuid;


            CPPCOMPONENTS_CONSTRUCT_NO_METHODS(DefaultStaticInterface);





        };




    // Usage
    template<class RC>
    struct use_runtime_class{};


    namespace detail{

        template<class Interface, class Base>
        struct inherit_use_interface_helper:public use<Interface>,Base{};

        template<class... Interfaces>
        struct inherit_use_interfaces_linearly{};

        template<class First, class... Rest>
        struct inherit_use_interfaces_linearly<First,Rest...>:public inherit_use_interface_helper<First,inherit_use_interfaces_linearly<Rest...>> {;

        };

        struct unknown_holder{
            use<InterfaceUnknown> unknown_;

            unknown_holder(use<InterfaceUnknown> i):unknown_(i){}
            unknown_holder(portable_base* p, bool addref)
                :unknown_(use<InterfaceUnknown>(cross_compiler_interface::reinterpret_portable_base<InterfaceUnknown::Interface>(p),addref)){}


            use<InterfaceUnknown>& get_unknown(){
                return unknown_;
            }

        };


    }

    namespace detail{
        template<class... Interfaces>
        struct use_runtime_class_helper{};

        template<>
        struct use_runtime_class_helper<>{
            template<class T>
            static void set_use_unknown(T* pthis){
                // do nothing
            }
        }; 
        template<class First, class... Rest>
        struct use_runtime_class_helper<First,Rest...>{
            template<class T>
            static void set_use_unknown(T* pthis){
                use<First>* pfirst = pthis;
                *pfirst = pthis->template as<First>();

                use_runtime_class_helper<Rest...>::set_use_unknown(pthis);
            }
        };

        template<class Base,class CF, class F>
        struct interface_overload_function_helper{


        };
        template<class Base,class CF, class R, class... Parms>
        struct interface_overload_function_helper<Base,CF,R(Parms...)>:public Base{

            static R overloaded_call(cross_compiler_interface::portable_base* p, Parms... parms){
                CF cf(p);
                return cf(parms...);
            }


        };
        template<class Base,class CF>
        struct interface_overload_function:public interface_overload_function_helper<Base,CF,typename CF::function_signature>{
            using interface_overload_function_helper<Base,CF,typename CF::function_signature>::overloaded_call;
            using Base::overloaded_call;

        };



        template<class... CF>
        struct inheritance_overload_helper{};
        template<class First,class... CF>
        struct inheritance_overload_helper<First,CF...>:public interface_overload_function<inheritance_overload_helper<CF...>,First>{
            typedef First first_cf_type;
            typedef inheritance_overload_helper<CF...> rest_cf_types;

        };
        template<>
        struct inheritance_overload_helper<>{

            // All calls to overloaded call have portable_base as first parameter so this will not resolve
            void overloaded_call();

        };

        template<class TypeList>
        struct forward_to_inheritance_overload_helper{};

        template<class... T>
        struct forward_to_inheritance_overload_helper<cross_compiler_interface::type_list<T...>>{
            typedef inheritance_overload_helper<T...> type;
        };

        template<class Interface,class... Parms>
        static use<InterfaceUnknown> overloaded_creator(Interface i,Parms... p){
            typedef typename cross_compiler_interface::type_information<Interface>::functions functions;
            typedef typename forward_to_inheritance_overload_helper<functions>::type helper;
            cross_compiler_interface::portable_base* pb = i.get_portable_base();
            return helper::overloaded_call(pb,p...).template QueryInterface<InterfaceUnknown>();
        }

        // Holds factory and the module
        // This assures that we won't be destructing after last RoInitializeCalled
        struct activation_factory_holder{
            typedef    cross_compiler_interface::error_code (CROSS_CALL_CALLING_CONVENTION* cross_compiler_factory_func)(const char* s,
                cross_compiler_interface::portable_base** p);
            cross_compiler_interface::module m_;
            use<InterfaceUnknown> af_;

            activation_factory_holder(const std::string& class_name)
                :m_(runtime_classes_map().match(class_name))
                ,af_(create(m_,class_name))
            {}

            static use<InterfaceUnknown> create(cross_compiler_interface::module& m,const std::string& class_name){
                    auto f = m.load_module_function<cross_compiler_factory_func>("get_cppcomponents_factory");
                    portable_base* p = nullptr;
                    auto e = f(class_name.c_str(),&p);
                    if(e < 0) cross_compiler_interface::general_error_mapper::exception_from_error_code(e);
                    return use<InterfaceUnknown>(cross_compiler_interface::reinterpret_portable_base<InterfaceUnknown::Interface>(p),false);
            }


        };

    }

    template<std::string(*pfun_runtime_class_name)(),class DefaultInterface, class FactoryInterface, 
        class StaticInterface, class... Others>
    struct use_runtime_class<runtime_class<pfun_runtime_class_name,DefaultInterface,FactoryInterface,StaticInterface,Others...>>
        :private detail::unknown_holder,
        public detail::inherit_use_interfaces_linearly<DefaultInterface,Others...>
        ,public StaticInterface::template Interface<use<StaticInterface> >
        ::template cross_compiler_interface_static_interface_mapper<
        use_runtime_class<runtime_class<pfun_runtime_class_name,DefaultInterface,FactoryInterface,StaticInterface,Others...> > >
    {
        typedef runtime_class<pfun_runtime_class_name,DefaultInterface,FactoryInterface,StaticInterface,Others...> runtime_class_t;
       use<DefaultInterface> default_interface(){
            use<DefaultInterface>* p = this;
            return *p;
        } 
        template<class Interface>
        use<Interface> as(){
            return this->get_unknown().template QueryInterface<Interface>();
        }

        static use<FactoryInterface> factory_interface(){
            // Cache the activation factory
            //static detail::activation_factory_holder afh_(runtime_class_t::get_runtime_class_name());
            //return afh_.af_.QueryInterface<FactoryInterface>();
			struct uniq{};
			return cross_compiler_interface::detail::safe_static_init<
				detail::activation_factory_holder, use_runtime_class>::get(runtime_class_t::get_runtime_class_name())
				.af_.template QueryInterface<FactoryInterface>();

        }

        static use<FactoryInterface> factory_interface(cross_compiler_interface::module& m,
            const std::string& class_name = runtime_class_t::get_runtime_class_name()){
            return detail::activation_factory_holder::create(m,class_name)
                .template QueryInterface<FactoryInterface>();

        }
        static use<StaticInterface> static_interface(){
            return factory_interface().template QueryInterface<StaticInterface>();
        }

        static use<StaticInterface> static_interface(cross_compiler_interface::module& m,
            const std::string& class_name = runtime_class_t::get_runtime_class_name()){
            return factory_interface(m,class_name).template QueryInterface<StaticInterface>();
        }

        use_runtime_class()
            :detail::unknown_holder(detail::overloaded_creator(factory_interface()))
        {
            typedef detail::use_runtime_class_helper<DefaultInterface,Others...> h_t;
            h_t::set_use_unknown(this);
        }
        use_runtime_class(cross_compiler_interface::module& m,const std::string& class_name)
            :detail::unknown_holder(detail::overloaded_creator(factory_interface(m,class_name)))
        {
            typedef detail::use_runtime_class_helper<DefaultInterface,Others...> h_t;
            h_t::set_use_unknown(this);
        }

        use_runtime_class(portable_base* p,bool addref)
            :detail::unknown_holder(p,addref)
        {
            typedef detail::use_runtime_class_helper<DefaultInterface,Others...> h_t;
            h_t::set_use_unknown(this);

        }

        template<class P,class... Parms>
        use_runtime_class(P p0,Parms... p)
            :detail::unknown_holder(detail::overloaded_creator(factory_interface(),p0,p...))
        {
            typedef detail::use_runtime_class_helper<DefaultInterface,Others...> h_t;
            h_t::set_use_unknown(this);
        }

        template<class P,class... Parms>
        use_runtime_class(cross_compiler_interface::module& m,const std::string& class_name,P p0,Parms... p)
            :detail::unknown_holder(detail::overloaded_creator(factory_interface(m,class_name),p0,p...))
        {
            typedef detail::use_runtime_class_helper<DefaultInterface,Others...> h_t;
            h_t::set_use_unknown(this);
        }


        template<class I>
        static use_runtime_class from_interface(I i){
            // Make sure I is a unknown_interface
            // To do this, check that it has uuid typedef

            typedef typename I::uuid u_t;
            return use_runtime_class(i.get_portable_base(),true);
        }

    private:



    };

}


#define CPPCOMPONENTS_DEFINE_FACTORY(...) \
	extern "C"{ \
	CROSS_CALL_EXPORT_FUNCTION cppcomponents::error_code CROSS_CALL_CALLING_CONVENTION  get_cppcomponents_factory(const char* s, \
	cppcomponents::portable_base** p){ \
	typedef cross_compiler_interface::type_list<__VA_ARGS__> t; \
		return cppcomponents::get_activation_factory(t(), s, p);\
}\
}



#endif
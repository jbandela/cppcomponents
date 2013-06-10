#pragma once
#ifndef CROSS_COMPILER_COMPONENT_HPP_05_29_2013
#define CROSS_COMPILER_COMPONENT_HPP_05_29_2013


#include "cross_compiler_introspection.hpp"

namespace cross_compiler_interface{

    // Define a runtime_class
    template<std::string(*pfun_runtime_class_name)(),
        template<class> class DefaultInterface,
        template <class> class FactoryInterface,
        template<class> class StaticInterface, 
        template<class> class... OtherInterfaces>
    struct runtime_class{
        static  std::string get_runtime_class_name(){
            return pfun_runtime_class_name();
        }
    };

    // Enables implementing runtime class
    template<class Derived, class WRC>
    struct implement_runtime_class{};

    namespace detail{

        template<class Derived, template<class>class FactoryInterface,template<class> class StaticInterface>
        struct implement_factory_static_helper
            :public implement_unknown_interfaces<Derived,FactoryInterface,StaticInterface>{

        };
        //template<class Derived, template<class> class StaticInterface>
        //struct implement_factory_static_helper<Derived,InterfaceActivationFactory,StaticInterface>
        //    :public implement_inspectable_interfaces<Derived,InterfaceActivationFactory,StaticInterface>{

        //};

        template<template<class> class Iface, class T>
        void caster(use_unknown<Iface>& r, T& t){
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

    }
    // Template to help implement a runtime class
    // You inherit from this class, providing the name of your class in derived
    template<std::string(*pfun_runtime_class_name)(),class Derived, template<class> class DefaultInterface, template<class> class FactoryInterface, template<class> class StaticInterface, template<class> class... Others>
    struct implement_runtime_class<Derived,runtime_class<pfun_runtime_class_name,DefaultInterface,FactoryInterface,StaticInterface,Others...>>
        :public implement_unknown_interfaces<Derived,DefaultInterface,Others...>
    {

        // The runtime class default interface
        typedef runtime_class<pfun_runtime_class_name,DefaultInterface,FactoryInterface,StaticInterface,Others...> runtime_class_t;
        cross_compiler_interface::implement_interface<DefaultInterface>* default_interface(){
            return this->template get_implementation<DefaultInterface>();

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

                cross_compiler_interface::implement_interface<FactoryInterface>* factory_interface(){
                    return this->template get_implementation<FactoryInterface>();
                }

                cross_compiler_interface::implement_interface<StaticInterface>* static_interface(){
                    return this->template get_implementation<StaticInterface>();
                }


                template<class... T>
                use_unknown<InterfaceUnknown> activate_instance_parms(T... t){
                    return Derived::create(t...).template QueryInterface<InterfaceUnknown>();
                }

                use_unknown<InterfaceUnknown> activate_instance(){
                    return Derived::create().template QueryInterface<InterfaceUnknown>();
                }
                implement_factory_static_interfaces(){


                    auto memp = cross_compiler_interface::type_information<cross_compiler_interface::implement_interface<FactoryInterface>>::get_ptrs_to_members();
                    typedef typename detail::forward_to_factory_to_constructor<typename cross_compiler_interface::type_information<cross_compiler_interface::implement_interface<FactoryInterface>>::functions>::type f_t;
                    f_t::set(*this,memp,*factory_interface());
                    //activation_factory_interface()->ActivateInstance.template set_mem_fn<
                    //    implement_factory_static_interfaces,&implement_factory_static_interfaces::activate_instance>(this);
                }




        };

        typedef implement_factory_static_interfaces ImplementFactoryStaticInterfaces;

        static use_unknown<InterfaceUnknown> get_activation_factory(std::string s){
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
            static use_unknown<InterfaceUnknown> get_activation_factory(std::string hs){
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
            static use_unknown<InterfaceUnknown> get_activation_factory(std::string hs){
                auto r = First::get_activation_factory(hs);
                if(r){
                    return r;
                }
                else{
                    return nullptr;
                }
            }

        };
        static use_unknown<InterfaceUnknown> get_activation_factory(std::string hs){
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
                    return std::string();
                }

                --i;
                if(s.substr(0,i->first.size()) == i->first){
                    return i->second;
                }

                return std::string();
            };


        };
    }
    detail::runtime_class_name_mapper& runtime_classes_map(){

        static detail::runtime_class_name_mapper m_;
        return m_;

    }


    namespace interface_definitions{
        struct DefaultFactoryInterface{

            // {7175F83C-6803-4472-8D5A-199E478BD8ED}
            typedef cross_compiler_interface::uuid<
                0x7175f83c, 0x6803, 0x4472, 0x8d, 0x5a, 0x19, 0x9e, 0x47, 0x8b, 0xd8, 0xed> uuid;

            cross_compiler_interface::use_unknown<cross_compiler_interface::InterfaceUnknown> Create();

            CROSS_COMPILER_INTERFACE_CONSTRUCT_UNKNOWN_INTERFACE(DefaultFactoryInterface,Create);





        };
        struct DefaultStaticInterface{

            typedef cross_compiler_interface::uuid<
                // {465BEFAD-C805-4164-A7C8-84051A868B4D}
                0x465befad, 0xc805, 0x4164, 0xa7, 0xc8, 0x84, 0x5, 0x1a, 0x86, 0x8b, 0x4d
            > uuid;


            CROSS_COMPILER_INTERFACE_CONSTRUCT_UNKNOWN_INTERFACE_NO_METHODS(DefaultStaticInterface);





        };

    }

    template<class T>
    struct DefaultFactoryInterface:public interface_definitions::DefaultFactoryInterface::Interface<T>{};

    template<class T>
    struct DefaultStaticInterface:public interface_definitions::DefaultStaticInterface::Interface<T>{};


    // Usage
    template<class RC>
    struct use_runtime_class{};


    namespace detail{

        template<template<class> class Interface, class Base>
        struct inherit_use_interface_helper:public use_unknown<Interface>,Base{};

        template<template<class> class... Interfaces>
        struct inherit_use_interfaces_linearly{};

        template<template<class> class First,template<class> class... Rest>
        struct inherit_use_interfaces_linearly<First,Rest...>:public inherit_use_interface_helper<First,inherit_use_interfaces_linearly<Rest...>> {;

        };

        struct unknown_holder{
            use_unknown<InterfaceUnknown> unknown_;

            unknown_holder(use_unknown<InterfaceUnknown> i):unknown_(i){}

            use_unknown<InterfaceUnknown>& get_unknown(){
                return unknown_;
            }

        };


    }

    namespace detail{
        template<template<class> class... Interfaces>
        struct use_runtime_class_helper{};

        template<>
        struct use_runtime_class_helper<>{
            template<class T>
            static void set_use_unknown(T* pthis){
                // do nothing
            }
        }; 
        template<template<class> class First, template<class> class... Rest>
        struct use_runtime_class_helper<First,Rest...>{
            template<class T>
            static void set_use_unknown(T* pthis){
                use_unknown<First>* pfirst = pthis;
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
        static use_unknown<InterfaceUnknown> overloaded_creator(Interface i,Parms... p){
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
            module m_;
            use_unknown<InterfaceUnknown> af_;

            activation_factory_holder(const std::string& class_name)
                :m_(runtime_classes_map().match(class_name)){
                    auto f = m_.load_module_function<cross_compiler_factory_func>("get_cross_compiler_factory");
                    portable_base* p = nullptr;
                    auto e = f(class_name.c_str(),&p);
                    if(e < 0) general_error_mapper::exception_from_error_code(e);
                    af_ = use_unknown<InterfaceUnknown>(reinterpret_portable_base<InterfaceUnknown>(p),false);

            }


        };
    }

    template<std::string(*pfun_runtime_class_name)(),template<class> class DefaultInterface, template<class> class FactoryInterface, template<class> class StaticInterface, template<class> class... Others>
    struct use_runtime_class<runtime_class<pfun_runtime_class_name,DefaultInterface,FactoryInterface,StaticInterface,Others...>>
        :private detail::unknown_holder,
        public detail::inherit_use_interfaces_linearly<DefaultInterface,Others...>
    {
        typedef runtime_class<pfun_runtime_class_name,DefaultInterface,FactoryInterface,StaticInterface,Others...> runtime_class_t;
        cross_compiler_interface::use_unknown<DefaultInterface> default_interface(){
            return this->template get_implementation<DefaultInterface>();

        } 
        template<template<class> class Interface>
        cross_compiler_interface::use_unknown<Interface> as(){
            return this->get_unknown().template QueryInterface<Interface>();
        }

        static use_unknown<FactoryInterface> activation_factory_interface(){
            // Cache the activation factory
            static detail::activation_factory_holder afh_(runtime_class_t::get_runtime_class_name());
            return afh_.af_.QueryInterface<FactoryInterface>();

        }

        static use_unknown<FactoryInterface> factory_interface(){
            return activation_factory_interface().template QueryInterface<FactoryInterface>();
        }

        static use_unknown<StaticInterface> static_interface(){
            return activation_factory_interface().template QueryInterface<StaticInterface>();
        }

        use_runtime_class()
            :detail::unknown_holder(detail::overloaded_creator(factory_interface()))
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
    private:



    };

}




#endif
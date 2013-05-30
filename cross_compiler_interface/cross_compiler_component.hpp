#pragma once
#ifndef CROSS_COMPILER_COMPONENT_HPP_05_29_2013
#define CROSS_COMPILER_COMPONENT_HPP_05_29_2013


#include "cross_compiler_introspection.hpp"

namespace cross_compiler_interface{

        // Define a runtime_class
    template<const char*(*pfun_runtime_class_name)(),
        template<class> class DefaultInterface,
        template <class> class FactoryInterface,
        template<class> class StaticInterface, 
        template<class> class... OtherInterfaces>
    struct runtime_class{
        static  const char* get_runtime_class_name(){
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
            r = t.QueryInterface<Iface>();
        }


        // Helpers for using constructors to automatically map interface
        template<class CF, class F>
        struct factory_to_constructor_helper{};

        template<class CF, class R, class... Parms>
        struct factory_to_constructor_helper<CF,R(Parms...)>{
            template<class ImpFactHelper, class MPS,class Interface>

            static void set(ImpFactHelper& helper, MPS& m,Interface& i){
                auto ptm = m.get<CF>();
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
            return this->get_implementation<DefaultInterface>();

        }

        void map_default_implementation_to_member_functions(){

            this->get_implementation<DefaultInterface>()->map_to_member_functions_no_prefix(static_cast<Derived*>(this));

        }

        implement_runtime_class(){
            static_cast<Derived*>(this)->map_default_implementation_to_member_functions();    
        }



        struct implement_factory_static_interfaces
            :public detail::implement_factory_static_helper<typename Derived::ImplementFactoryStaticInterfaces,FactoryInterface,StaticInterface>{

                typedef runtime_class<pfun_runtime_class_name,DefaultInterface,FactoryInterface,StaticInterface,Others...> runtime_class_t;

                cross_compiler_interface::implement_interface<FactoryInterface>* factory_interface(){
                    return this->get_implementation<FactoryInterface>();
                }

                cross_compiler_interface::implement_interface<StaticInterface>* static_interface(){
                    return this->get_implementation<StaticInterface>();
                }


                template<class... T>
                use_unknown<InterfaceUnknown> activate_instance_parms(T... t){
                    return Derived::create(t...).QueryInterface<InterfaceUnknown>();
                }

                use_unknown<InterfaceUnknown> activate_instance(){
                    return Derived::create().QueryInterface<InterfaceUnknown>();
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
                    helper<Rest...>::get_activation_factory(hs);

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
    error_code get_activation_factory( std::string activatibleClassId, portable_base** factory){
        try{
            auto r = get_activation_factory_helper<Imps...>::get_activation_factory(activatibleClassId);
            if(r){
                *factory = r.get_portable_base_addref();
                return S_OK;
            }
            else{
                return cross_compiler_interface::error_no_interface::ec;
            }
        }
        catch(std::exception& e){
            return cross_compiler_interface::general_error_mapper::error_code_from_exception(e);
        }
    }


}




#endif
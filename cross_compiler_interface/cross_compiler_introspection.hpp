#pragma once
#ifndef CROSS_COMPILER_INTROSPECTION_HPP_05_05_2013
#define CROSS_COMPILER_INTROSPECTION_HPP_05_05_2013
#include "interface_unknown.hpp"
#include <typeinfo>
#include <vector>
#include <array>

// Introspection depends on boost::any
#include "implementation/any.hpp"

namespace cross_compiler_interface{

    namespace detail{
        struct unspecialized_type{};

    };

    template<class T>
    struct type_name_getter{};



    template<class T>
    struct type_information{
        enum{is_interface = 0};
        static std::string name(){return type_name_getter<T>::get_type_name();};
     };  

    template<template<template<class> class> class Wrapper, template<class> class Iface > 
    struct type_information<Wrapper<Iface>>{
        enum{is_interface = 1}; 
        enum{is_unknown_interface = 0}; 
        static std::string name(){return type_name_getter< Wrapper<Iface> >::get_type_name() ;} 
        enum{names_size = 1+use_interface<Iface>::num_functions - use_interface<Iface>::base_sz};
        static const char* (&names())[names_size]{return  type_name_getter< Wrapper<Iface> >:: template get_type_names<names_size>();}
        typedef typename type_name_getter<Wrapper<Iface>>::functions functions; 
    };   
    
    template<template<class> class Iface > 
    struct type_information<use_unknown<Iface>>{
        enum{is_interface = 1}; 
        enum{is_unknown_interface = 1}; 
        static std::string name(){return type_name_getter< use_unknown<Iface> >::get_type_name() ;} 
        enum{names_size = 1+use_unknown<Iface>::num_functions - use_unknown<Iface>::base_sz};
        static const char* (&names())[names_size]{return  type_name_getter< use_unknown<Iface> >:: template get_type_names<names_size>();}
        typedef typename type_name_getter<use_unknown<Iface>>::functions functions; 
        typedef typename use_unknown<Iface>::uuid uuid_t;
        static uuid_base& get_uuid(){return uuid_t::get();};
    };

    struct cross_function_information{
        std::string name;
        std::string return_type;
        std::string return_type_raw;
        std::vector<std::string> parameter_types;
        std::vector<std::string> parameter_types_raw;
        std::function<cross_compiler_interface::any(use_unknown<InterfaceUnknown>,const std::vector<cross_compiler_interface::any>&)> call;

        typedef use_unknown<InterfaceUnknown> interface_pointer_t;
    };

    class interface_information{

        std::string name_;
        std::vector<cross_function_information> functions_;
    public:


        void add_function(std::size_t i,cross_function_information info){
            if(i < functions_.size()){
                functions_.at(i) = info;
            }
            else{
                functions_.push_back(std::move(info));
            }
        }

        cross_function_information& get_function(int i){
            return functions_.at(i);
        }        
        const cross_function_information& get_function(int i)const{
            return functions_.at(i);
        }

        int size(){
            return functions_.size();
        }

        void name(std::string name){
            name_ = std::move(name);
        }

        std::string name(){
            return name_;
        }



    };

    template<class T>
    struct type_to_type{};

    template<class V>
    struct variant_conversion_helper{
        template<class T>
        static T convert_from_variant(type_to_type<T>, const V&);

        template<class T>
        static V convert_to_variant(T&& t);

        static V create_empty_variant();
    };


        template<>
    struct variant_conversion_helper<any>{
        template<class T>
        static T convert_from_variant(type_to_type<T>, const any& v){
             return cross_compiler_interface::any_cast<T>(v);

        }

        template<class T>
        static any convert_to_variant(T&& t){
            return any(t);
        }

        static any create_empty_variant(){
            return any();
        }
    };

    template<class... T>
    struct type_list{
        enum{size = sizeof...(T)};
    };

    template<class T, std::size_t I>
    struct type_and_index{
        enum{index = I};
        typedef T type;

    };



    template<int I,class... T>
    struct to_type_and_index{};
    template<int I,class First, class... Rest>
    struct to_type_and_index<I,First,Rest...>
        :public to_type_and_index<I-1,Rest...,type_and_index<First,1 + sizeof...(Rest)-I>>{};

    template<class First,class... T>
    struct to_type_and_index<0,First,T...>{
        typedef type_list<First,T...> types;

    };
    template<>
    struct to_type_and_index<0>{
        typedef type_list<> types;

    };

    namespace detail{

         template<class V,class TI>
         typename TI::type get_value_from_variant_vector(TI,const std::vector<V>& v){
             return variant_conversion_helper<V>::convert_from_variant(type_to_type<typename TI::type>(),v.at(TI::index));

         }


         template<class... Parameters>
         struct cross_function_parameter_helper{};

        template<class P, class... Rest>
        struct cross_function_parameter_helper<P,Rest...>{
            static void add_parameter_info(cross_function_information& info){
                info.parameter_types.push_back(cross_compiler_interface::type_information<P>::name());
                cross_function_parameter_helper<Rest...>::add_parameter_info(info);
            }

        };
       template<>
       struct cross_function_parameter_helper<>{
            static void add_parameter_info(cross_function_information& info){
               
            }

        };      

 



         template<class V,class R>
         struct return_variant{
             template<class CF, class... T>
             static any do_return(CF& cf,T&&... t){
                 using namespace std;
                 return variant_conversion_helper<V>::convert_to_variant(cf(t...));
             }
         };        
         template<class V>
         struct return_variant<V,void>{
             template<class CF, class... T>
             static any do_return(CF& cf,T&&... t){
                 using namespace std;
                 cf(std::forward<T>(t)...);
                 return variant_conversion_helper<V>::create_empty_variant();
             }
         };

         template<class V, class CF,class...TI>
         struct set_call_helper{
             template<class Info>
             static void set_call(Info& info){
                 typedef typename Info::interface_pointer_t interface_pointer_t;
                 info.call = [](interface_pointer_t p,const std::vector<V>& v)->V{
                     CF cf(p.get_portable_base());

                     return return_variant<V,decltype(cf(get_value_from_variant_vector(TI(),v)...))>::do_return(cf,get_value_from_variant_vector(TI(),v)...);
                 };
             }

         };

         template<class V, class CF,class T>
         struct forward_typelist_to_set_call_helper{};

         template<class V, class CF, class... T>
         struct forward_typelist_to_set_call_helper<V,CF,type_list<T...>>{
             typedef set_call_helper<V,CF,T...> type;
         };




        template<class F>
        struct cross_function_introspection_helper{};

        template<class R, class... Parms>
        struct cross_function_introspection_helper<R(Parms...)>{

           template<class CF>
           static cross_function_information get_function_information(){
                cross_function_information info;
                info.return_type = cross_compiler_interface::type_information<R>::name();
                cross_function_parameter_helper<Parms...>::add_parameter_info(info);
                typedef typename to_type_and_index<sizeof...(Parms),Parms...>::types types;
                typedef typename forward_typelist_to_set_call_helper<any,CF,types>::type helper;
                helper::set_call(info);
                return info;
           }
           static cross_function_information get_function_information_raw(){
                cross_function_information info;
                info.return_type = cross_compiler_interface::type_information<R>::name();
                cross_function_parameter_helper<Parms...>::add_parameter_info(info);
                return info;
           }
        };

        template<class... T>
        struct interface_functions_information_helper{};

        template<class First, class... T>
        struct interface_functions_information_helper<First,T...>{
            template<class Info>
            static void get_interface_functions_information(Info& info,int i = 0){
                typedef First cf_t;
                auto func_info = detail::cross_function_introspection_helper<typename First::function_signature>:: template get_function_information<cf_t>();
                auto func_info_raw = detail::cross_function_introspection_helper<typename First::function_signature_raw>::get_function_information_raw();
                func_info.return_type_raw = std::move(func_info_raw.return_type);
                func_info.parameter_types_raw = std::move(func_info_raw.parameter_types);

                info.add_function(i,func_info);

                interface_functions_information_helper<T...>::get_interface_functions_information(info,i+1);
            }


        };
        template<>
        struct interface_functions_information_helper<>{
            template<class Info>
            static void get_interface_functions_information(Info& info,int i = 0){
                // do nothing
            }


        };
    
        template<class T>
        struct forward_typelist_to_interface_functions_information_helper{};

        template<class... T>
        struct forward_typelist_to_interface_functions_information_helper<type_list<T...>>{
            typedef interface_functions_information_helper<T...> type;
        };
        template<class Info,class T>
        struct interface_functions_information{

            static Info get(){
                Info info;

                typedef typename forward_typelist_to_interface_functions_information_helper<typename T::functions>::type helper;
                helper::get_interface_functions_information(info);

                info.name(T::names()[0]);
                for(int i = 0; i < info.size(); ++i){
                    info.get_function(i).name = T::names()[i+1];
                }

                return info;
            }

        };



    }

    template<template<class> class Iface>
    interface_information get_interface_information(){
        return detail::interface_functions_information<interface_information,type_information<use_interface<Iface>>>::get();
    }

    namespace detail{

        template<class Interface, class CF>
        struct cross_function_ptr_to_member_holder{
            CF Interface::* ptr_to_member_;
           
            //cross_function_ptr_to_member_holder(CF Interface::* p):ptr_to_member_(p){}
        };

        template<class T> struct ptr_to_member_holder_helper{};

        template<class Interface, class CF>
        struct ptr_to_member_holder_helper<CF Interface::*>{
            typedef cross_function_ptr_to_member_holder<Interface,CF> type;
        };

        template<class Interface, class... CFS>
        struct cross_function_ptrs_to_members:public cross_function_ptr_to_member_holder<Interface,CFS>...{

        private:
            void helper(){}

            template<class First,class... T>
            void helper(First f, T... t){
                typedef typename ptr_to_member_holder_helper<First>::type holder_t;
                holder_t* h = this;
                h->ptr_to_member_ = f;

                helper(t...);
            }
        public:
            template<class... T>
            cross_function_ptrs_to_members(T... t){
                helper(t...);
            }

            template<class CF>
            CF Interface::* get(){
                cross_function_ptr_to_member_holder<Interface,CF>* h = this;
                return h->ptr_to_member_;
            };
        };





    }

    template<class Interface,class Functions>
    struct interface_functions_ptrs_to_member{};

    template<class Interface, class... Functions>
    struct interface_functions_ptrs_to_member<Interface,type_list<Functions...>>{
        typedef detail::cross_function_ptrs_to_members<Interface,Functions...> type;
    };

    template<template<template<class> class> class Wrapper>
    struct wrapper_name_getter{};

    template<>struct wrapper_name_getter<use_unknown>{
        static const char* get_name(){return "cross_compiler_interface::use_unknown";}
    };
    template<>struct wrapper_name_getter<use_interface>{
        static const char* get_name(){return "cross_compiler_interface::use_interface";}
    };
    template<>struct wrapper_name_getter<implement_interface>{
        static const char* get_name(){return "cross_compiler_interface::implement_interface";}
    };

}


#define CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(T) \
    namespace cross_compiler_interface{ \
    template<> struct type_name_getter<T>{\
        static const char* get_type_name(){return #T;} \
};\
}


// Include Preprocessor Manipulation Macros
#include "implementation/cross_compiler_interface_pp.hpp"

#define CROSS_COMPILER_INTERFACE_STRINGIZE_EACH(x) CROSS_COMPILER_INTERFACE_STR(x)
#define CROSS_COMPILER_INTERFACE_DECLTYPE_EACH(x) decltype(std::declval<iface_t>().x) 
#define CROSS_COMPILER_INTERFACE_PTM_EACH(x) &iface_t::x 
//#define FOO(...) CROSS_COMPILER_INTERFACE_APPLY(FOO_EACH, __VA_ARGS__)


#define CROSS_COMPILER_INTERFACE_DEFINE_INTERFACE_INFORMATION(T,...)   \
namespace cross_compiler_interface { \
    template<template<template<class> class> class Iface> struct type_name_getter<Iface<T>>{\
    template<int N> \
    static const char*(& get_type_names())[N]{   \
        static const char* names[] = {#T,CROSS_COMPILER_INTERFACE_APPLY(CROSS_COMPILER_INTERFACE_STRINGIZE_EACH, __VA_ARGS__)}; \
        return names;    \
    }\
    static std::string get_type_name(){return std::string(wrapper_name_getter<Iface>::get_name()) + "<" #T ">" ;} \
    typedef T<Iface<T>> iface_t; \
    typedef type_list<CROSS_COMPILER_INTERFACE_APPLY(CROSS_COMPILER_INTERFACE_DECLTYPE_EACH,__VA_ARGS__)> functions;\
    typedef typename interface_functions_ptrs_to_member<iface_t,functions>::type functions_ptrs_to_members_t; \
    static functions_ptrs_to_members_t& get_ptrs_to_members(){\
       static functions_ptrs_to_members_t fpm(CROSS_COMPILER_INTERFACE_APPLY(CROSS_COMPILER_INTERFACE_PTM_EACH,__VA_ARGS__));  \
       return fpm; \
    }\
};\
}  

#define CROSS_COMPILER_INTERFACE_DEFINE_INTERFACE_INFORMATION_NO_METHODS(T)   \
namespace cross_compiler_interface { \
    template<template<template<class> class> class Iface> struct type_name_getter<Iface<T>>{\
    template<int N> \
    static const char*(& get_type_names())[N]{   \
        static const char* names[] = {#T}; \
        return names;    \
    }\
    static std::string get_type_name(){return std::string(wrapper_name_getter<Iface>::get_name()) + "<" #T ">" ;} \
    typedef T<Iface<T>> iface_t; \
    typedef type_list<> functions;\
    typedef typename interface_functions_ptrs_to_member<iface_t,functions>::type functions_ptrs_to_members_t; \
    static functions_ptrs_to_members_t& get_ptrs_to_members(){\
       static functions_ptrs_to_members_t fpm;  \
       return fpm;\
    }\
};\
}  

CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(char);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(char*);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(const char*);

    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::int8_t);
    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::int16_t);
    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::int32_t);
    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::int64_t);

    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::uint8_t);
    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::uint16_t);
    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::uint32_t);
    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::uint64_t);    
    
    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::int8_t*);
    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::int16_t*);
    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::int32_t*);
    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::int64_t*);

    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::uint8_t*);
    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::uint16_t*);
    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::uint32_t*);
    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::uint64_t*);

    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::int8_t&);
    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::int16_t&);
    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::int32_t&);
    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::int64_t&);

    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::uint8_t&);
    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::uint16_t&);
    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::uint32_t&);
    CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::uint64_t&);    

CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(double);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(double*);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(double&);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(float);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(float*);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(float&);

CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(void);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(void*);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(const void*);

CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(bool);

CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::string);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::u16string);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::u32string);

#ifndef _MSC_VER
// In MSVC char16_t and char32_t are not real types
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(char16_t);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(char32_t);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(char16_t*);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(char32_t*);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(const char16_t*);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(const char32_t*);
#endif




namespace cross_compiler_interface{ 

    template<class T>
    struct type_name_getter<std::vector<T>>{
        static std::string get_type_name(){return std::string("std::vector<") + type_name_getter<T>::get_type_name() + ">";} 
    };  
    
    template<class T>
     struct type_name_getter<cross_compiler_interface::cross_vector<T>>{
        static std::string get_type_name(){return std::string("cross_compiler_interface::cross_vector<") + type_name_getter<T>::get_type_name() + ">";} 
    };    
    
    template<class T>
     struct type_name_getter<cross_compiler_interface::cross_vector_return<T>>{
        static std::string get_type_name(){return std::string("cross_compiler_interface::cross_vector_return<") + type_name_getter<T>::get_type_name() + ">";} 
    }; 
     template<class T>
     struct type_name_getter<cross_compiler_interface::cross_vector_return<T>*>{
        static std::string get_type_name(){return std::string("cross_compiler_interface::cross_vector_return<") + type_name_getter<T>::get_type_name() + ">*";} 
    };   

    template<class T>
     struct type_name_getter<cross_compiler_interface::cross_vector_trivial<T>>{
        static std::string get_type_name(){return std::string("cross_compiler_interface::cross_vector_trivial<") + type_name_getter<T>::get_type_name() + ">";} 
    };    
    
    template<class T>
     struct type_name_getter<cross_compiler_interface::cross_vector_return_trivial<T>>{
        static std::string get_type_name(){return std::string("cross_compiler_interface::cross_vector_return_trivial<") + type_name_getter<T>::get_type_name() + ">";} 
    };     
     template<class T>
     struct type_name_getter<cross_compiler_interface::cross_vector_return_trivial<T>*>{
        static std::string get_type_name(){return std::string("cross_compiler_interface::cross_vector_return_trivial<") + type_name_getter<T>::get_type_name() + ">*";} 
    };   
    
    template<class T>
     struct type_name_getter<cross_compiler_interface::cross_string<T>>{
        static std::string get_type_name(){return std::string("cross_compiler_interface::cross_string<") + type_name_getter<T>::get_type_name() + ">";} 
    };
    template<class T>
     struct type_name_getter<cross_compiler_interface::cross_string_return<T>>{
        static std::string get_type_name(){return std::string("cross_compiler_interface::cross_string_return<") + type_name_getter<T>::get_type_name() + ">";} 
    }; 
     template<class T>
     struct type_name_getter<cross_compiler_interface::cross_string_return<T>*>{
        static std::string get_type_name(){return std::string("cross_compiler_interface::cross_string_return<") + type_name_getter<T>::get_type_name() + ">*";} 
    };
    template<class T,class U>
     struct type_name_getter<std::pair<T,U>>{
        static std::string get_type_name(){return std::string("std::pair<") + type_name_getter<T>::get_type_name() + type_name_getter<U>::get_type_name() + ">";} 
    };
    template<class T,class U>
     struct type_name_getter<cross_compiler_interface::cross_pair<T,U>>{
        static std::string get_type_name(){return std::string("cross_compiler_interface::cross_pair<") + type_name_getter<T>::get_type_name() + type_name_getter<U>::get_type_name() + ">";} 
    };
    template<class T,class U>
    struct type_name_getter<cross_compiler_interface::cross_pair_return<T,U>>{
        static std::string get_type_name(){return std::string("cross_compiler_interface::cross_pair_return<") + type_name_getter<T>::get_type_name() + type_name_getter<U>::get_type_name() + ">";} 
    };  
    template<class T,class U>
    struct type_name_getter<cross_compiler_interface::cross_pair_return<T,U>*>{
        static std::string get_type_name(){return std::string("cross_compiler_interface::cross_pair_return<") + type_name_getter<T>::get_type_name() + type_name_getter<U>::get_type_name() + ">*";} 
    };

    template<class T>
    struct type_name_getter<cross_compiler_interface::out<T>>{
        static std::string get_type_name(){return std::string("cross_compiler_interface::out<") + type_name_getter<T>::get_type_name() +">";} 
    };
    template<class T>
    struct type_name_getter<cross_compiler_interface::cross_out<T>>{
        static std::string get_type_name(){return std::string("cross_compiler_interface::cross_out<") + type_name_getter<T>::get_type_name() +">";} 
    };


}
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(cross_compiler_interface::cr_string);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(cross_compiler_interface::u16cr_string);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(cross_compiler_interface::u32cr_string);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(cross_compiler_interface::cr_string*);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(cross_compiler_interface::u16cr_string*);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(cross_compiler_interface::u32cr_string*);


CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(cross_compiler_interface::uuid_base *);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(cross_compiler_interface::portable_base *);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(cross_compiler_interface::portable_base **);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(const cross_compiler_interface::portable_base *);
CROSS_COMPILER_INTERFACE_DEFINE_INTERFACE_INFORMATION(cross_compiler_interface::InterfaceUnknown,QueryInterfaceRaw,AddRef,Release);


#endif
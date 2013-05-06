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


    template<template<class> class T>
    struct type_information<cross_compiler_interface::use_unknown<T>>{ 
        enum{is_interface = 1}; 
        static std::string name(){return type_name_getter< use_unknown<T> >::get_type_name() ;} 
        enum{names_size = 1+use_unknown<T>::num_functions - use_unknown<T>::base_sz};
        static const char* (&names())[names_size]{return  type_name_getter< use_unknown<T> >:: template get_type_names<names_size>();}
        typedef typename type_name_getter<use_unknown<T>>::functions functions;

    };


    struct cross_function_information{
        std::string name;
        std::string return_type;
        std::string return_type_raw;
        std::vector<std::string> parameter_types;
        std::vector<std::string> parameter_types_raw;
        std::function<cross_compiler_interface::any(use_unknown<InterfaceUnknown>,const std::vector<cross_compiler_interface::any>&)> call;
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

    template<template <class> class Iface>
	struct introspect_interface:private interface_information,public Iface<introspect_interface<Iface>>{ // Introspection


        interface_information& info(){
            return *this;
        }

       static interface_information get_interface_information(){
            introspect_interface intro;
            interface_information info = std::move(intro);
            return info;
        }

       enum{num_functions = sizeof(Iface<size_only>)/sizeof(cross_function<Iface<size_only>,0,void()>)};
       enum{base_sz = Iface<introspect_interface<Iface>>::base_sz};
       enum{num_interface_functions = num_functions - base_sz };

       typedef typename type_information<use_unknown<Iface>>::functions functions;

    private:
        introspect_interface(){
            static_assert(sizeof(type_information<use_unknown<Iface>>::names())/sizeof(type_information<use_unknown<Iface>>::names()[0]) == num_interface_functions + 1,
                "Mismatch in number of functions and number of names provided");
            info().name(type_information<use_unknown<Iface>>::names()[0]);
            for(int i = 0; i < info().size(); ++i){
                info().get_function(i).name = type_information<use_unknown<Iface>>::names()[i+1];
            }
        }
    };
    namespace detail{


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

 
         template<class T, int I>
         struct type_and_int{
             enum{value = I};
             typedef T type;

             static typename std::decay<T>::type get(const std::vector<cross_compiler_interface::any>& v){
                 return cross_compiler_interface::any_cast<typename std::decay<T>::type>(v.at(I));
             }
         };


         template<class R>
         struct return_any{
             template<class CF, class... T>
             static any do_return(CF& cf,T&&... t){
                 using namespace std;
                 return any(cf(t...));
             }
         };        
         template<>
         struct return_any<void>{
             template<class CF, class... T>
             static any do_return(CF& cf,T&&... t){
                 using namespace std;
                 cf(std::forward<T>(t)...);
                 return any();
             }
         };


         template<int I,class... T>
         struct to_type_and_int{};
         template<int I,class First, class... Rest>
         struct to_type_and_int<I,First,Rest...>:public to_type_and_int<I-1,Rest...,type_and_int<First,1 + sizeof...(Rest)-I>>{};

         template<class First,class... T>
         struct to_type_and_int<0,First,T...>{

             template<class CF>
             static void set_call_imp(cross_function_information& info){
                 info.call = [](use_unknown<InterfaceUnknown> punk,std::vector<any> v)->any{
                     CF cf(punk.get_portable_base());
                     return return_any<decltype(cf(First::get(v),T::get(v)...))>::do_return(cf,First::get(v),T::get(v)...);
                 };

             }
         };
         template<>
         struct to_type_and_int<0>{

             template<class CF>
             static void set_call_imp(cross_function_information& info){
                 using namespace cross_compiler_interface;
                 info.call = [](use_unknown<InterfaceUnknown> punk,std::vector<any> v)->any{
                     CF cf(punk.get_portable_base());
                     return return_any<decltype(cf())>::do_return(cf);
                 };

             }
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
               to_type_and_int<sizeof...(Parms),Parms...>::template set_call_imp<CF>(info);
                return info;
           }
           static cross_function_information get_function_information_raw(){
                cross_function_information info;
                info.return_type = cross_compiler_interface::type_information<R>::name();
                cross_function_parameter_helper<Parms...>::add_parameter_info(info);
                return info;
           }
        };


        template<class R, class... Parms>
        struct cross_function_introspection_helper<R(CROSS_CALL_CALLING_CONVENTION *)(Parms...)>{

           static cross_function_information get_function_information_raw(){
                cross_function_information info;
                info.return_type = cross_compiler_interface::type_information<R>::name();
                cross_function_parameter_helper<Parms...>::add_parameter_info(info);
                return info;
           }

        };

    }

    

	template<template<class> class Iface,template<class> class T, int Id,class F>
	struct cross_function<Iface<introspect_interface<T>>,Id,F>{
		enum{N = Id + Iface<introspect_interface<T>>::base_sz};
                template<template<class> class U>
		cross_function(Iface<introspect_interface<U>>* pi){
            // Do nothing
		}
		cross_function(Iface<introspect_interface<Iface>>* pi){
            typedef detail::cross_function_implementation<false,Iface,N,F> cf_t;
            typedef detail::cross_function_implementation<true,Iface,N,F> cfi_t;
			auto& info = static_cast<introspect_interface<Iface>*>(pi)->info();	
            auto func_info = detail::cross_function_introspection_helper<F>:: template get_function_information<cf_t>();
            auto func_info_raw = detail::cross_function_introspection_helper<decltype(&cfi_t::func)>::get_function_information_raw();
            func_info.return_type_raw = std::move(func_info_raw.return_type);
            func_info.parameter_types_raw = std::move(func_info_raw.parameter_types);

            info.add_function(Id,func_info);
		}
    };

    namespace detail{
        template<class Derived,class Iface>
        struct derived_rebinder{};

        template< template<class,int>class Derived,class Iface, int Id,class IfaceNew> 
        struct derived_rebinder<Derived<Iface,Id>,IfaceNew>{
            typedef Derived<IfaceNew,Id> type;
        };


    };

    template<template<class> class Iface, template<class> class T,int Id,class F1, class F2,class Derived,class FuncType>
	struct custom_cross_function<Iface<introspect_interface<T>>,Id,F1,F2,Derived,FuncType>{
		enum{N = Id + Iface<introspect_interface<T>>::base_sz};
        template<template<class> class U>
		custom_cross_function(Iface<introspect_interface<U>>* pi){

		}
        typedef custom_cross_function base_t;
 		custom_cross_function(Iface<introspect_interface<Iface>>* pi){
            typedef typename detail::derived_rebinder<Derived,Iface<use_unknown<Iface>>>::type cf_t;
			auto& info = static_cast<introspect_interface<Iface>*>(pi)->info();	
            auto func_info = detail::cross_function_introspection_helper<F1>:: template get_function_information<cf_t>();
            auto func_info_raw = detail::cross_function_introspection_helper<F2>::get_function_information_raw();
            func_info.return_type_raw = std::move(func_info_raw.return_type);
            func_info.parameter_types_raw = std::move(func_info_raw.parameter_types);

            info.add_function(Id,func_info);
		}
   };


    template<class... T>
    struct type_list{
        enum{size = sizeof...(T)};
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
//#define FOO(...) CROSS_COMPILER_INTERFACE_APPLY(FOO_EACH, __VA_ARGS__)


#define CROSS_COMPILER_INTERFACE_DEFINE_INTERFACE_INFORMATION(T,...)   \
namespace cross_compiler_interface { \
    template<template<template<class> class> class Iface> struct type_name_getter<Iface<T>>{\
    template<int N> \
    static const char*(& get_type_names())[N]{   \
        static const char* names[] = {#T,CROSS_COMPILER_INTERFACE_APPLY(CROSS_COMPILER_INTERFACE_STRINGIZE_EACH, __VA_ARGS__)}; \
        return names;    \
    }\
    static const char* get_type_name(){return "cross_compiler_interface::use_unknown<" #T ">" ;} \
    typedef T<Iface<T>> iface_t; \
    typedef type_list<CROSS_COMPILER_INTERFACE_APPLY(CROSS_COMPILER_INTERFACE_DECLTYPE_EACH,__VA_ARGS__)> functions;\
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

   template<template<class>class T>
    struct type_name_getter<cross_compiler_interface::use_interface<T>>{
        static std::string get_type_name(){return std::string("cross_compiler_interface::use_interface<") + "undefined interface" +">";} 
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
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
    inline const char* get_type_name(T);

    template<class T, int N> 
    const char*(& get_type_names(T))[N];


    template<class T>
    struct type_information{
        enum{is_interface = 0};
        static std::string name(){return cross_compiler_interface::get_type_name(type_information());};
     };  


    template<template<class> class T>
    struct type_information<cross_compiler_interface::use_unknown<T>>{ 
        enum{is_interface = 1}; 
        static std::string name(){return get_type_name(cross_compiler_interface::type_information()) ;} 
        enum{names_size = 1+1+use_unknown<T>::num_functions - use_unknown<T>::base_sz};
        static auto names()->const char*(&)[names_size]{return  cross_compiler_interface::get_type_names<names_size>(type_information());}
    };


    struct cross_function_information{
        std::string name;
        std::string return_type;
        std::vector<std::string> parameter_types;
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


    private:
        introspect_interface(){
            static_assert(sizeof(type_information<use_unknown<Iface>>::names())/sizeof(type_information<use_unknown<Iface>>::names()[0]) == num_interface_functions + 2,
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


       template<class P>
       struct cross_function_parameter_helper<P>{
            static void add_parameter_info(cross_function_information& info){
                info.parameter_types.push_back(cross_compiler_interface::type_information<P>::name());
            }

        };      
          
        template<class P, class... Rest>
        struct cross_function_parameter_helper<P,Rest...>{
            static void add_parameter_info(cross_function_information& info){
                static_assert(cross_compiler_interface::type_information<P>::is_specialized, "type_information not specialized");
                info.parameter_types_.push_back(cross_compiler_interface::type_information<P>::name());
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

             static T get(const std::vector<cross_compiler_interface::any>& v){
                 return cross_compiler_interface::any_cast<T>(v.at(I));
             }
         };



         template<int N, int I,class... T>
         struct to_type_and_int{};
         template<int N, int I,class First, class... Rest>
         struct to_type_and_int<N,I,First,Rest...>:public to_type_and_int<N,I+1,Rest...,type_and_int<First,I>>{};

         template<int N,class... T>
         struct to_type_and_int<N,N,T...>{

             template<class CF>
             static void set_call_imp(cross_function_information& info){
                 info.call = [](use_unknown<InterfaceUnknown> punk,std::vector<any> v)->any{
                     CF cf(punk.get_portable_base());
                     return any(cf(T::get(v)...));
                 };

             }
         };
         template<class... T>
         struct to_type_and_int<0,0,T...>{

             template<class CF>
             static void set_call_imp(cross_function_information& info){
                 using namespace cross_compiler_interface;
                 info.call = [](use_unknown<InterfaceUnknown> punk,std::vector<any> v)->any{
                     CF cf(punk.get_portable_base());
                     return any(cf());
                 };

             }
         };




        template<class F>
        struct cross_function_introspection_helper{};

        template<class R, class... Parms>
        struct cross_function_introspection_helper<R(Parms...)>{

           template<class CF>
           static void set_call(cross_function_information& info){
               to_type_and_int<sizeof...(Parms),0,Parms...>::set_call_imp<CF>(info);
            }
           
           template<class CF>
           static cross_function_information get_function_information(){
                cross_function_information info;
                info.return_type = cross_compiler_interface::type_information<R>::name();
                cross_function_parameter_helper<Parms...>::add_parameter_info(info);
               to_type_and_int<sizeof...(Parms),0,Parms...>::set_call_imp<CF>(info);
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
			auto& info = static_cast<introspect_interface<Iface>*>(pi)->info();	
            info.add_function(Id,detail::cross_function_introspection_helper<F>::get_function_information<cf_t>());
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
            typedef detail::cross_function_implementation<false,Iface,N,F> cf_t;
			auto& info = static_caset<introspect_interface<Iface>*>(pi)->info();	
            info.add_function(Id,detail::cross_function_introspection_helper<F>::get_function_information<cf_t>());
		}
   };




}


#define CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(T) \
    namespace cross_compiler_interface{ \
    inline const char* get_type_name(type_information<T>){return #T;} \
}


#define CROSS_COMPILER_INTERFACE_DEFINE_INTERFACE_INFORMATION(T,...)   \
namespace cross_compiler_interface { \
    \
    template<int N> \
    const char*(& get_type_names(cross_compiler_interface::type_information<use_unknown<T>>))[N]{   \
        static const char* names[] = {#T,__VA_ARGS__}; \
        return names;    \
    }\
    inline const char* get_type_name(cross_compiler_interface::type_information<use_unknown<T>>){return "cross_compiler_interface::use_unknown<" #T ">" ;} \
}  


CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(std::int32_t);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(cross_compiler_interface::uuid_base *);
CROSS_COMPILER_INTERFACE_DEFINE_INTERFACE_INFORMATION(cross_compiler_interface::InterfaceUnknown,"_QueryInterface","_AddRef","_Release");

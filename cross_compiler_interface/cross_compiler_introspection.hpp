#include "interface_unknown.hpp"
#include <typeinfo>
#include <vector>

// Introspection depends on boost::any
#include "implementation/any.hpp"

namespace cross_compiler_interface{

    namespace detail{
        struct unspecialized_type{};

    };

    template<class T,int dummy = 0>
    struct type_information{
        enum{is_specialized = 0};
        enum{is_interface = 0};
        static std::string name(){return "cross_compiler_interface_no_name";};
    };

    struct cross_function_information{
        std::string name;
        std::string return_type;
        std::vector<std::string> parameter_types;
        std::function<cross_compiler_interface::any(use_unknown<InterfaceUnknown>,std::vector<cross_compiler_interface::any>)> call;
    };

    class interface_information{

        std::string name_;
        std::vector<cross_function_information> functions_;
    public:


        void add_function(int i,cross_function_information info){
            if(i < functions_.size()){
                if(functions_.at(i).name.empty()){
                    functions_.at(i) = info;
                }
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
	struct introspect_interface:public Iface<introspect_interface<Iface>>{ // Introspection

       static interface_information& get_interface_information(){
            static interface_information info_;
            static introspect_interface intro_;
            return info_;
        }

       enum{num_functions = sizeof(Iface<size_only>)/sizeof(cross_function<Iface<size_only>,0,void()>)};
       enum{base_sz = Iface<introspect_interface<Iface>>::base_sz};
       enum{num_interface_functions = num_functions - base_sz };


       template<class... T>
       introspect_interface(std::string name,T&&... t){
           static_assert(sizeof...(T) == num_interface_functions,"Names for functions do not match the number of functions");
           get_interface_information().name(name);
           set_function_names(0,t...);
       }

    private:
       introspect_interface(){}
        void set_function_names(int i){};
        template<class T0, class... T>
        void set_function_names(int i,T0&& t0,T&&... t){
            get_interface_information().get_function(i).name = t0;
            set_function_names(i+1,t...);
        }

    };
    namespace detail{


         template<class... Parameters>
         struct cross_function_parameter_helper{};


       template<class P>
       struct cross_function_parameter_helper<P>{
            static void add_parameter_info(cross_function_information& info){
                static_assert(cross_compiler_interface::type_information<P>::is_specialized, "type_information not specialized");
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



        template<int... > struct int_sequence{};

        template<int... I>
        struct int_sequence<0,I...>{
            typedef int_sequence sequence;
        };


        template<int Lower, int... I>
        struct int_sequence<Lower,I...>{
            typedef typename int_sequence<Lower-1,Lower,I...>::sequence sequence;
        };

        template<int Upper>
        struct int_sequence_generator{
            typedef typename int_sequence<Upper-1,Upper>::sequence sequence;
        };

   



        template<class F>
        struct cross_function_introspection_helper{};

        template<class R, class... Parms>
        struct cross_function_introspection_helper<R(Parms...)>{



           //template<class CF,int... I>
           //static void set_call_imp(cross_function_information& info,detail::int_sequence<I...>){
           //     info.call = [](use_unknown<InterfaceUnknown> punk,std::vector<cross_compiler_interface::any> v)->cross_compiler_inteface::any{
           //         CF cf(punk.get_portable_base());
           //         return any(cf(any_cast<Parms>(v.at(I))...));
           //     };
           //};

           template<class CF>
           static void set_call(cross_function_information& info){
               //set_call_imp(info,typename int_sequence_generator<sizeof...(Parms)-1>::sequence());
            }
           

           static cross_function_information get_function_information(){
                cross_function_information info;
//                static_assert(cross_compiler_interface::type_information<P>::is_specialized, "type_information not specialized");
                info.return_type = cross_compiler_interface::type_information<R>::name();
                cross_function_parameter_helper<Parms...>::add_parameter_info(info);
                set_call<void>(info);
                return info;
           }

        };


    }

    

	template<template<class> class Iface,template<class> class T, int Id,class F>
	struct cross_function<Iface<introspect_interface<T>>,Id,F>{
		enum{N = Id + Iface<introspect_interface<T>>::base_sz};
		cross_function(Iface<introspect_interface<T>>* pi){
			auto& info = introspect_interface<Iface>::get_interface_information();	
            info.add_function(Id,detail::cross_function_introspection_helper<F>::get_function_information());
		}
    };


    template<template<class> class Iface, template<class> class T,int Id,class F1, class F2,class Derived,class FuncType>
	struct custom_cross_function<Iface<introspect_interface<T>>,Id,F1,F2,Derived,FuncType>{
		enum{N = Id + Iface<introspect_interface<T>>::base_sz};
		custom_cross_function(Iface<introspect_interface<T>>* pi){
			auto& info = introspect_interface<Iface>::get_interface_information();	
            info.add_function(Id,detail::cross_function_introspection_helper<F1>::get_function_information());
		}
        typedef custom_cross_function base_t;
    };




}

#define CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(T) \
    namespace cross_compiler_interface{ \
    template<int dummy>struct type_information<T,dummy>{ \
    enum{is_specialized = 1}; \
    enum{is_interface = 0}; \
    static std::string name(){return #T ;}\
}; \
}

#define CROSS_COMPILER_INTERFACE_DEFINE_INTERFACE_INFORMATION(T,...) \
    namespace cross_compiler_interface{ \
    template<int dummy>struct type_information<cross_compiler_interface::use_unknown<T>,dummy>{ \
    enum{is_specialized = 1}; \
    enum{is_interface = 1}; \
    static std::string name(){return "cross_compiler_interface::use_unknown<" #T ">" ;} \
    static introspect_interface<T> introspection_; \
    }; \
    template<int dummy> \
    introspect_interface<T> type_information<cross_compiler_interface::use_unknown<T>,dummy>::introspection_ = introspect_interface<T>(#T,__VA_ARGS__); \
    \
}\
    namespace{cross_compiler_interface::introspect_interface<T> cross_compiler_interface_introspect_interface_variable##__LINE__(#T,__VA_ARGS__);}


CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(int);
CROSS_COMPILER_INTERFACE_DEFINE_TYPE_INFORMATION(cross_compiler_interface::uuid_base *);

    //    namespace cross_compiler_interface{ 
    //template<int dummy>struct type_information<int,dummy>{ 
    //enum{is_specialized = 1}; 
    //enum{is_interface = 0}; 
    //static std::string name(){return "int" ;}
//}; 
//}
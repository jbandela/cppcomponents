//          Copyright John R. Bandela 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#include <string>
#include <algorithm>
#include <vector>
#include <type_traits>
#include <cstdint>
#include <limits>
#include <utility>



#pragma pack(push,1)


#ifdef __GNUC__ 

#define CROSS_COMPILER_INTERFACE_PACK __attribute__((packed))

#else

#define CROSS_COMPILER_INTERFACE_PACK 

#endif

namespace cross_compiler_interface {


    template<class T>
    struct trivial_conversion{
        typedef T converted_type;
        typedef T original_type;
        static converted_type to_converted_type(original_type i){return i;};
        static original_type to_original_type(converted_type c){return c;}
    };

    template<class T> struct cross_conversion;


    // Macro for defining trivial conversions
#define JRB_TRIVIAL_CONV(a) template<> struct cross_conversion<a>:public trivial_conversion<a>{}; \
    template<> struct cross_conversion<a*>:public trivial_conversion<a*>{}; \
    template<> struct cross_conversion<const a*>:public trivial_conversion<const a*>{}; \
    template<> struct cross_conversion<a&>:public trivial_conversion<a&>{}; \
    template<> struct cross_conversion<const a&>:public trivial_conversion<const a&>{}

    JRB_TRIVIAL_CONV(char);
    JRB_TRIVIAL_CONV(std::int8_t);
    JRB_TRIVIAL_CONV(std::int16_t);
    JRB_TRIVIAL_CONV(std::int32_t);
    JRB_TRIVIAL_CONV(std::int64_t);

    JRB_TRIVIAL_CONV(std::uint8_t);
    JRB_TRIVIAL_CONV(std::uint16_t);
    JRB_TRIVIAL_CONV(std::uint32_t);
    JRB_TRIVIAL_CONV(std::uint64_t);

    // Now do float and double
    static_assert(std::numeric_limits<float>::is_iec559,"float is not standard");
    JRB_TRIVIAL_CONV(float);
    static_assert(std::numeric_limits<double>::is_iec559,"double is not standard");
    JRB_TRIVIAL_CONV(double);

#undef JRB_TRIVIAL_CONV

    // Allow support for void* and const void*
    template<>
    struct cross_conversion<void*>:public trivial_conversion<void*>{};
    template<>
    struct cross_conversion<const void*>:public trivial_conversion<const void*>{};

    // Support for bool, bool has an implementation defined size
    // so use uint8_t
        template<>
    struct cross_conversion<bool>{
        typedef bool original_type;
        typedef std::uint8_t converted_type;
        static converted_type to_converted_type(bool b){
            return b;
        }
        static original_type to_original_type(std::uint8_t u){
            return u!=0;
        }
    };
     

    // Make sure size_t is concordant with void* in terms of size
    static_assert(sizeof(std::size_t)==sizeof(void*),"size_t is not non-concordant with void*");

    struct cross_string{
        const char* begin;
        const char* end;
    }CROSS_COMPILER_INTERFACE_PACK;


    struct cross_string_return{
        void* retstr;
        error_code (CROSS_CALL_CALLING_CONVENTION *transfer_string)(void*,const char*, const char*);
    }CROSS_COMPILER_INTERFACE_PACK;

    template<class T>
    struct cross_vector{
        const void* retvector;
        error_code (CROSS_CALL_CALLING_CONVENTION *get)(const void*, std::size_t, T*);

        std::size_t (CROSS_CALL_CALLING_CONVENTION *size)(const void*);

    }CROSS_COMPILER_INTERFACE_PACK;

    template<class T>
    struct cross_vector_return{
        void* retvector;
        error_code (CROSS_CALL_CALLING_CONVENTION *push_back)(void*, T);

        // Note do not support vector more than 2^32
        error_code (CROSS_CALL_CALLING_CONVENTION *reserve_vector)(void*,std::size_t sz);
    }CROSS_COMPILER_INTERFACE_PACK;

        template<class T>
    struct cross_vector_trivial{
        const T* begin_;
        const T* end_;
    }CROSS_COMPILER_INTERFACE_PACK;

    template<class T>
    struct cross_vector_return_trivial{
        void* retvector;
        error_code (CROSS_CALL_CALLING_CONVENTION *assign)(void*, const T*,const T*);
    }CROSS_COMPILER_INTERFACE_PACK;

    template<>
    struct cross_conversion<std::string>{
        typedef std::string original_type;
        typedef cross_string converted_type;
        static converted_type to_converted_type(const original_type& s){
            cross_string ret;
            ret.begin = &s[0];
            ret.end = &s[0] + s.size();
            return ret;
        }
        static  std::string to_original_type(converted_type& c){
           return std::string(c.begin,c.end);
        }
    };

    template<>
    struct cross_conversion_return<std::string>{
        typedef std::string return_type;
        typedef cross_string_return converted_type;

        static error_code CROSS_CALL_CALLING_CONVENTION do_transfer_string(void* str,const char* begin, const char* end){
            try{
                auto& s = *static_cast<std::string*>(str);
                s.assign(begin,end);
                return 0;
            }
            catch(std::exception& e){
                return general_error_mapper::error_code_from_exception(e);
            }

        };
        static void initialize_return(return_type& r, converted_type& c){
            c.retstr = &r;
            c.transfer_string = &do_transfer_string;
        }

        static void do_return(const return_type& r,converted_type& c){
            auto ec = c.transfer_string(c.retstr,r.data(),r.data() + r.size());
            if(ec < 0){
                general_error_mapper::exception_from_error_code(ec);
            }
        }
        static void finalize_return(return_type& r,converted_type& c){
            // do nothing
        }


    };

    template<bool b,class T>
    struct cross_conversion_vector_imp{};

    template<class T>
    struct cross_conversion_vector_imp<false,T>{
        typedef std::vector<T> original_type;
        typedef T original_value_type;
        typedef typename cross_conversion<T>::converted_type converted_value_type;
        typedef cross_vector<converted_value_type> converted_type;
        static error_code CROSS_CALL_CALLING_CONVENTION do_get(const void* vec,std::size_t i, converted_value_type* pt){
            try{
                auto& v = *static_cast<const original_type*>(vec);
                typedef cross_conversion<T> cc;
                *pt =  cc::to_converted_type(v[i]);
                return 0;
            }
            catch(std::exception& e){
                return general_error_mapper::error_code_from_exception(e);
            }
        }
        static std::size_t CROSS_CALL_CALLING_CONVENTION do_size(const void* vec){
            auto& v = *static_cast<const original_type*>(vec);
            return v.size();
        }
        static converted_type to_converted_type(const original_type& s){
            converted_type ret;
            ret.retvector =  &s;
            ret.get = &do_get;
            ret.size = &do_size;
            return ret;
        }
        static  original_type to_original_type(converted_type& c){
            original_type ret;
            auto sz = c.size(c.retvector);
            ret.reserve(sz);
            typedef cross_conversion<T> cc;
            for(std::size_t i = 0; i < sz; i++){
                converted_value_type v;

                auto ec = c.get(c.retvector,i,&v);
                if(ec < 0){
                    general_error_mapper::exception_from_error_code(ec);
                }
                ret.push_back(cc::to_original_type(v));

            }

            return ret;
        }

    };
        template<class T>
    struct cross_conversion_vector_imp<true,T>{
        typedef std::vector<T> original_type;
        typedef cross_vector_trivial<T> converted_type;
        static converted_type to_converted_type(const original_type& s){
            converted_type ret;
            ret.begin_ = nullptr; 
            ret.end_ = nullptr;
            if(s.size()){
                ret.begin_ = &s[0];
                ret.end_ = ret.begin_ + s.size();
            };
            return ret;
        }
        static  original_type to_original_type(converted_type& c){
           return original_type(c.begin_,c.end_);
        }

    };

    template<class T>
    struct is_trivial_cross_conversion{
        typedef T o_t;
        typedef cross_conversion<T> cc;
        typedef typename cc::converted_type c_t;
        enum{value = std::is_same<T,c_t>::value};

    };

    template<class T>
    struct cross_conversion<std::vector<T>>
        :public cross_conversion_vector_imp<
        is_trivial_cross_conversion<T>::value,T>
    {

    };

    template<bool b, class T>
    struct cross_conversion_vector_return_imp{};

    template<class T>
    struct cross_conversion_vector_return_imp<false,T>{
        typedef std::vector<T> original_type;
        typedef original_type return_type;
        typedef T original_value_type;
        typedef typename cross_conversion<T>::converted_type converted_value_type;
        typedef cross_vector_return<converted_value_type> converted_type;


        static error_code CROSS_CALL_CALLING_CONVENTION do_reserve_vector(void* vec, std::size_t sz){
            typedef cross_conversion<T> cc;
            try{
                auto& v = *static_cast<return_type*>(vec);
                v.reserve(sz);
                return 0;
            }
            catch(std::exception& e){
                return general_error_mapper::error_code_from_exception(e);
            }

        }

        static error_code CROSS_CALL_CALLING_CONVENTION do_push_back(void* vec, converted_value_type t){
            typedef cross_conversion<T> cc;
            try{
                auto& v = *static_cast<return_type*>(vec);
                v.push_back(cc::to_original_type(t));
                return 0;
            }
            catch(std::exception& e){
                return general_error_mapper::error_code_from_exception(e);
            }
        }

        static void initialize_return(return_type& r, converted_type& c){
            c.retvector = &r;
            c.reserve_vector=do_reserve_vector;
            c.push_back = do_push_back;
        }

        static void do_return(const return_type& r,converted_type& c){
            typedef cross_conversion<T> cc;
            auto ec = c.reserve_vector(c.retvector,r.size());
            if(ec < 0){general_error_mapper::exception_from_error_code(ec);}
            for(auto i = r.begin(); i != r.end(); ++i){
                auto ec = c.push_back(c.retvector,cc::to_converted_type(*i));
                if(ec < 0){general_error_mapper::exception_from_error_code(ec);}
            };
        }
        static void finalize_return(return_type& r,converted_type& c){
            // do nothing
        }

    };

    template<class T>
    struct cross_conversion_vector_return_imp<true,T>{
        typedef std::vector<T> original_type;
        typedef cross_vector_return_trivial<T> converted_type;
        typedef original_type return_type;


        static error_code CROSS_CALL_CALLING_CONVENTION do_assign(void* vec,
            const T* b, const T* e){
            try{
                auto& v = *static_cast<std::vector<T>*>(vec);
                v.assign(b,e);
                return 0;
            }
            catch(std::exception& e){
                return general_error_mapper::error_code_from_exception(e);
            }

        }


        static void initialize_return(original_type& r, converted_type& c){
            c.retvector = &r;
            c.assign = &do_assign;
        }

        static void do_return(const original_type& r,converted_type& c){
            const T* b = nullptr;
            const T* e = nullptr;
            if(!r.empty()){
                b = &r[0];
                e = b + r.size();
            };
            auto ec = c.assign(c.retvector,b,e);
            if(ec < 0){
                general_error_mapper::exception_from_error_code(ec);
            }
        }
        static void finalize_return(original_type& r,converted_type& c){
            // do nothing
        }

    };
    template<class T>
    struct cross_conversion_return<std::vector<T>>
    :public cross_conversion_vector_return_imp<
        is_trivial_cross_conversion<T>::value,T>{


    };



    template<template<class> class T>
    struct cross_conversion<use_interface<T>>{
        typedef use_interface<T> original_type;
        typedef portable_base* converted_type;
        static converted_type to_converted_type(const original_type& s){
            return s.get_portable_base();
        }
        static  original_type to_original_type(converted_type c){
            return use_interface<T>(reinterpret_portable_base<T>(c));
        }

    };		



    template<class T, class U>
    struct  cross_pair{
        typedef  cross_conversion<T> cct;
        typedef  cross_conversion<U> ccu;
        typename cct::converted_type first;
        typename ccu::converted_type second;
    }CROSS_COMPILER_INTERFACE_PACK;	



    template<class T,class U>
    struct cross_pair_return{
        void* retpair;
        error_code (CROSS_CALL_CALLING_CONVENTION *assign)(void*, T,U);
    }CROSS_COMPILER_INTERFACE_PACK;

    template<class T,class U>
    struct cross_conversion<std::pair<T,U>>{
        typedef std::pair<T,U> original_type;
        typedef cross_pair<T,U> converted_type;
        static converted_type to_converted_type(const original_type& s){
            converted_type ret;
            typedef cross_conversion<T> ccT;
            typedef cross_conversion<U> ccU;
            ret.first = ccT::to_converted_type(s.first);
            ret.second = ccU::to_converted_type(s.second);
            return ret;
        }
        static  original_type to_original_type(converted_type& c){
            original_type ret;
            typedef cross_conversion<T> ccT;
            typedef cross_conversion<U> ccU;
            ret.first = ccT::to_original_type(c.first);
            ret.second = ccU::to_original_type(c.second);
            return ret;
        }

    };

    template<class T,class U>
    struct cross_conversion_return<std::pair<T,U>>{
        typedef std::pair<T,U> original_type;
        typedef original_type return_type;
        typedef T original_value_typeT;
        typedef U original_value_typeU;
        typedef typename cross_conversion<T>::converted_type converted_typeT;
        typedef typename cross_conversion<U>::converted_type converted_typeU;
        typedef cross_pair_return<converted_typeT,converted_typeU> converted_type;



        static error_code CROSS_CALL_CALLING_CONVENTION do_assign(void* v, converted_typeT t,converted_typeU u){
            typedef cross_conversion<T> ccT;
            typedef cross_conversion<U> ccU;
            try{
                auto& p = *static_cast<return_type*>(v);
                p.first = ccT::to_original_type(t);
                p.second = ccU::to_original_type(u);
                return 0;
            }
            catch(std::exception& e){
                return general_error_mapper::error_code_from_exception(e);
            }
        }

        static void initialize_return(return_type& r, converted_type& c){
            c.retpair = &r;
            c.assign=&do_assign;
        }

        static void do_return(const return_type& r,converted_type& c){

            typedef cross_conversion<T> ccT;
            typedef cross_conversion<U> ccU;
            auto ec = c.assign(c.retpair,ccT::to_converted_type(r.first),ccU::to_converted_type(r.second));
            if(ec < 0){general_error_mapper::exception_from_error_code(ec);}
        }
        static void finalize_return(return_type& r,converted_type& c){
            // do nothing
        }


    };


    // out parameters
    template<class T>
    class out{
        void* original_;
        typedef cross_conversion<T> ccT;
        typedef typename ccT::converted_type c_t;

        error_code (CROSS_CALL_CALLING_CONVENTION *assign)(void*,
            c_t);
        template<class U> friend class cross_conversion;
        static error_code CROSS_CALL_CALLING_CONVENTION do_assign(void* v,
            c_t c){
                T& t = *static_cast<T*>(v);
                t = ccT::to_original_type(c);
                return 0;
        }
        out():original_(nullptr){}
    public:
        out(T* t):original_(t),assign(&do_assign){
            if(!original_){
                error_pointer e;
                throw e;
            }
        }
        void set(const T& t){
            typedef cross_conversion<T> cc;
            auto ec = assign(original_,cc::to_converted_type(t));
            if(ec < 0){general_error_mapper::exception_from_error_code(ec);}
        }

    };

    template<class T>
    struct cross_out{
        void* original_;
        typedef cross_conversion<T> ccT;
        typedef typename ccT::converted_type c_t;
        error_code (CROSS_CALL_CALLING_CONVENTION *assign)(void*,
            c_t);
    }CROSS_COMPILER_INTERFACE_PACK;

    template< class T>
    struct cross_conversion<out<T>>{
        typedef cross_out<T> converted_type;
        typedef out<T> original_type;

        static cross_out<T> to_converted_type(const out<T>& o){
            cross_out<T> ret;
            ret.assign = o.assign;
            ret.original_ = o.original_;
            return ret;
        }
        static  out<T> to_original_type(cross_out<T> c){
            out<T> ret;
            ret.assign = c.assign;
            ret.original_ = c.original_;
            return ret; 
        }

    };	
}


#include "cr_string.hpp"
namespace cross_compiler_interface{
    template<>
    struct cross_conversion<cr_string>:public trivial_conversion<cr_string>{};
}
#pragma pack(pop)

#undef CROSS_COMPILER_INTERFACE_PACK
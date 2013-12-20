#pragma once
#ifndef INCLUDE_GUARD_CPPCOMPONENTS_ITERATOR_HPP_12_19_2013_
#define INCLUDE_GUARD_CPPCOMPONENTS_ITERATOR_HPP_12_19_2013_
#include "cppcomponents.hpp"
#include "clonable.hpp"
#include "comparisons.hpp"
#include "implementation/uuid_combiner.hpp"
#include <iterator>

namespace cppcomponents{
  namespace iterator{
    typedef cppcomponents::uuid<0xf88de4e7, 0x002f, 0x4ec8, 0xa56f, 0xa8b4f17cedc3> iiterator_read_uuid;
    template<class T>
    struct IReader :define_interface<combine_uuid<iiterator_read_uuid, typename uuid_of<typename std::remove_const<T>::type>::uuid_type>>
    {
      typename std::remove_const<T>::type Read();
      CPPCOMPONENTS_CONSTRUCT_TEMPLATE(IReader, Read)
    };
    typedef cppcomponents::uuid<0x78912aae, 0xcd16, 0x40b3, 0xbc64, 0xf2f3aebb80e6> iiterator_writer_uuid;
    template<class T>
    struct IWriter :define_interface<combine_uuid<iiterator_writer_uuid, typename uuid_of<typename std::remove_const<T>::type>::uuid_type >>
    {
      void Write(typename std::remove_const<T>::type);
      CPPCOMPONENTS_CONSTRUCT_TEMPLATE(IWriter, Write)
    };


    struct IForwardAccess :define_interface<cppcomponents::uuid<0x76ff2980, 0x81a9, 0x4588, 0xb580, 0x213bc370eff3>>
    {
      void Next();
      CPPCOMPONENTS_CONSTRUCT(IForwardAccess, Next)
    };

    struct IBidirectionalAccess :define_interface<cppcomponents::uuid<0xdecb2785, 0xa3d9, 0x41a6, 0x93c8, 0xd6c5e67a4bf1>, IForwardAccess>
    {
      void Previous();
      CPPCOMPONENTS_CONSTRUCT(IBidirectionalAccess,Previous)
    };

    struct IRandomAccess :define_interface<cppcomponents::uuid<0x890eb4f6, 0x553c, 0x4c5f, 0x9d7e, 0xa1434ea2f0c9>, IBidirectionalAccess>
    {
      void Advance(std::int64_t);
      std::int64_t Distance(use<IRandomAccess>);
      CPPCOMPONENTS_CONSTRUCT(IRandomAccess, Advance, Distance)
    };

    inline std::string value_dummy_id(){ return "cppcomponents::uuid<0xc956d15c, 0x2a51, 0x4e6a, 0xa56a, 0x17a109bd5bb9>"; }

    template<class T>
    struct implement_value :implement_runtime_class<implement_value<T>, runtime_class<value_dummy_id, object_interfaces<IReader<T>, IWriter<T>>, factory_interface<NoConstructorFactoryInterface>>>
    {
      std::unique_ptr<T> ptr_;
      implement_value(T t) :ptr_{ new T{ std::move(t) } }{}
      typename std::remove_const<T>::type Read(){ return *ptr_; }
      void IWriter_Write(typename std::remove_const<T>::type t){ ptr_.reset(new T{ std::move(t) }); }
    };

    template<class T>
    struct implement_set_value{
      void set_value_imp(use<InterfaceUnknown>& iunk, T t){
        if (!iunk){
          iunk = implement_value<T>::create(std::move(t));
        }
        else{
          iunk.QueryInterface<IWriter<T>>().Write(std::move(t));
        }
      }
    };
    template<class T>
    struct implement_set_value<const T>{ };

    template<class T>
    struct proxy:private implement_set_value<T>{
    private:
      mutable use<InterfaceUnknown> iunk_;

      void set_value(T t){
        this->set_value_imp(iunk_, t);
      }

      T get_value()const{
         return iunk_.QueryInterface<IReader<T>>().Read(); 
      }
    public:
      void set(T t){
        set_value(std::move(t));
      }

      T get(){
        return get_value();
      }

      explicit proxy(use<InterfaceUnknown> iunk) :iunk_{ iunk }{}

      operator T() const {
        return get_value();
      }
      proxy& operator=(T t){
        set_value(std::move(t));
        return *this;
      }

      void cppcomponents_iterator_proxy_assign(use<InterfaceUnknown> iunk){ iunk_ = iunk; }

      proxy& operator=(proxy&& other){
        T t = other;
        set_value(std::move(t));
        other = t;
        return *this;
      } 
      proxy& operator=(const proxy& other){
        T t = other;
        set_value(std::move(t));
        return *this;
      }
      proxy(proxy&& other) {
        T t = other;
        set_value(std::move(t));
        other = t;
      }
      proxy(const proxy& other) {
        T t = other;
        set_value(std::move(t));
      }

    };
    template<class T>
    bool operator==(const proxy<T>& a, const proxy<T>& b){
      return static_cast<T>(a) == static_cast<T>(b);
    }
    template<class Type, class T>
    bool operator==(const Type& a, const proxy<T>& b){
      return (a) == static_cast<T>(b);
    }
    template<class Type, class T>
    bool operator==(const proxy<T>& a, const Type& b){
      return static_cast<T>(a) == (b);
    }
    template<class T>
    bool operator!=(const proxy<T>& a, const proxy<T>& b){
      return static_cast<T>(a) != static_cast<T>(b);
    }
    template<class Type,class T>
    bool operator!=(const Type& a, const proxy<T>& b){
      return (a) != static_cast<T>(b);
    }
    template<class Type, class T>
    bool operator!=(const proxy<T>& a, const Type& b){
      return static_cast<T>(a) != (b);
    }
    template<class T>
    bool operator<(const proxy<T>& a, const proxy<T>& b){
      return static_cast<T>(a) < static_cast<T>(b);
    }
    template<class Type, class T>
    bool operator<(const Type& a, const proxy<T>& b){
      return (a) < static_cast<T>(b);
    }
    template<class Type, class T>
    bool operator<(const proxy<T>& a, const Type& b){
      return static_cast<T>(a) < (b);
    }
    template<class T>
    bool operator<=(const proxy<T>& a, const proxy<T>& b){
      return static_cast<T>(a) <= static_cast<T>(b);
    }
    template<class Type, class T>
    bool operator<=(const Type& a, const proxy<T>& b){
      return (a) <= static_cast<T>(b);
    }
    template<class Type, class T>
    bool operator<=(const proxy<T>& a, const Type& b){
      return static_cast<T>(a) <= (b);
    }
    template<class T>
    bool operator>(const proxy<T>& a, const proxy<T>& b){
      return static_cast<T>(a) > static_cast<T>(b);
    }
    template<class Type, class T>
    bool operator>(const Type& a, const proxy<T>& b){
      return (a) > static_cast<T>(b);
    }
    template<class Type, class T>
    bool operator>(const proxy<T>& a, const Type& b){
      return static_cast<T>(a) > (b);
    }

    template<class T>
    bool operator>=(const proxy<T>& a, const proxy<T>& b){
      return static_cast<T>(a) >= static_cast<T>(b);
    }
    template<class Type, class T>
    bool operator>=(const T& a, const proxy<T>& b){
      return (a) >= static_cast<T>(b);
    }
    template<class Type, class T>
    bool operator>=(const proxy<T>& a, const T& b){
      return static_cast<T>(a) >= (b);
    }


    template<class T>
    struct input_iterator_wrapper :std::iterator<std::input_iterator_tag, T, std::int64_t>
    {
    private:
      use<IReader<T>> reader_;
      use <IForwardAccess> access_;
      use <IEqualityComparable> compare_;
      proxy<T> proxy_;


    public:
      input_iterator_wrapper(use<InterfaceUnknown> iunk)
        :reader_{ iunk.QueryInterface<IReader<T>>() },
        access_{ iunk.QueryInterface<IForwardAccess>() },
        compare_{ iunk.QueryInterface<IEqualityComparable>() },
        proxy_{iunk}
      {}
      input_iterator_wrapper(const input_iterator_wrapper& other)
        :reader_{ other.reader_.template QueryInterface<IClonable>().Clone().template QueryInterface<IReader<T>>() },
        access_{ reader_.template QueryInterface<IForwardAccess>() },
        compare_{ reader_.template QueryInterface<IEqualityComparable>() },
        proxy_{ reader_ }

      {  }

      input_iterator_wrapper(input_iterator_wrapper&& other)
        :reader_{ std::move(other.reader_) },
        access_{ std::move(other.access_) },
        compare_{ std::move(other.compare_) },
        proxy_{ reader_ }
      {
        other.proxy_.cppcomponents_iterator_proxy_assign(nullptr);
      }

      input_iterator_wrapper& operator=(input_iterator_wrapper&& other){
        reader_ = std::move(other.reader_);
        access_ = std::move(other.access_);
        compare_ = std::move(other.compare_);
        proxy_.cppcomponents_iterator_proxy_assign(reader_);
        other.proxy_.cppcomponents_iterator_proxy_assign(nullptr);
        return *this;
      }

      input_iterator_wrapper& operator=(const input_iterator_wrapper& other){
        input_iterator_wrapper temp{ other };
        *this = std::move(temp);
        return *this;
      }

      // Preincrement
      input_iterator_wrapper& operator++(){
        access_.Next();
        return *this;
      }
      // Preincrement
      input_iterator_wrapper operator++(int){
        input_iterator_wrapper ret{ *this };
        ++ret;
        return ret;
      }

      const proxy<T>& operator*() const{
        return proxy_;
      }

      bool operator==(const input_iterator_wrapper& other){
        return compare_.Equals(other.compare_);
      }
      bool operator!=(const input_iterator_wrapper& other){
        return !compare_.Equals(other.compare_);
      }
    };



    template<class T>
    struct output_iterator_wrapper :std::iterator<std::output_iterator_tag, T, std::int64_t>
    {
    private:
      use<IWriter<T>> writer_;
      use <IForwardAccess> access_;
      proxy<T> proxy_;


    public:
      output_iterator_wrapper(use<InterfaceUnknown> iunk)
        :writer_{ iunk.QueryInterface<IWriter<T>>() },
        access_{ iunk.QueryInterface<IForwardAccess>() },
        proxy_{iunk}
      {}
      output_iterator_wrapper(const output_iterator_wrapper& other)
        :writer_{ other.writer_.template QueryInterface<IClonable>().Clone().template QueryInterface<IWriter<T>>() },
        access_{ writer_.template QueryInterface<IForwardAccess>() },
        proxy_{ writer_ }

      {  }

      output_iterator_wrapper(output_iterator_wrapper&& other)
        :writer_{ std::move(other.writer_) },
        access_{ std::move(other.access_) },
        proxy_{ writer_ }
      {
        other.proxy_.cppcomponents_iterator_proxy_assign(nullptr);
      }

      output_iterator_wrapper& operator=(output_iterator_wrapper&& other){
        writer_ = std::move(other.writer_);
        access_ = std::move(other.access_);
        proxy_.cppcomponents_iterator_proxy_assign(writer_);
        other.proxy_.cppcomponents_iterator_proxy_assign(nullptr);
        return *this;
      }

      output_iterator_wrapper& operator=(const output_iterator_wrapper& other){
        output_iterator_wrapper temp{ other };
        *this = std::move(temp);
        return *this;
      }

      // Preincrement
      output_iterator_wrapper& operator++(){
        access_.Next();
        return *this;
      }
      // Preincrement
      output_iterator_wrapper operator++(int){
        output_iterator_wrapper ret{ *this };
        ++ret;
        return ret;
      }

      proxy<T>& operator*(){
        return proxy_;
      }


    };

    template<class T>
    struct forward_iterator_wrapper :std::iterator<std::forward_iterator_tag, T, std::int64_t>
    {
    private:
      mutable use<IReader<T>> reader_;
      use<IWriter<T>> writer_;
      use <IForwardAccess> access_;
      use <IEqualityComparable> compare_;
      proxy<T> proxy_;


    public:
      forward_iterator_wrapper(use<InterfaceUnknown> iunk)
        :reader_{ iunk.QueryInterface<IReader<T>>() },
         writer_{ iunk.QueryInterface<IWriter<T>>() },
        access_{ iunk.QueryInterface<IForwardAccess>() },
        compare_{ iunk.QueryInterface<IEqualityComparable>() },
        proxy_{ iunk }

      {}

      forward_iterator_wrapper(const forward_iterator_wrapper& other)
        :reader_{ other.reader_.template QueryInterface<IClonable>().Clone().template QueryInterface<IReader<T>>() },
        writer_{ reader_.template QueryInterface<IWriter<T>>() },
        access_{ reader_.template QueryInterface<IForwardAccess>() },
        compare_{ reader_.template QueryInterface<IEqualityComparable>() },
        proxy_{ reader_ }

      {  } 
      
      forward_iterator_wrapper(forward_iterator_wrapper&& other)
        :reader_{ std::move(other.reader_) },
        writer_{ std::move(other.writer_) },
        access_{ std::move(other.access_) },
        compare_{ std::move(other.compare_) },
        proxy_{ reader_ }
      {  
        other.proxy_.cppcomponents_iterator_proxy_assign(nullptr);
      }

      forward_iterator_wrapper& operator=(forward_iterator_wrapper&& other){
        reader_ = std::move(other.reader_);
        writer_ = std::move(other.writer_);
        access_ = std::move(other.access_);
        compare_ = std::move(other.compare_);
        proxy_.cppcomponents_iterator_proxy_assign(reader_);
        other.proxy_.cppcomponents_iterator_proxy_assign(nullptr);
        return *this;
      }

      forward_iterator_wrapper& operator=(const forward_iterator_wrapper& other){
        forward_iterator_wrapper temp{ other };
        *this = std::move(temp);
        return *this;
      }
      // Preincrement
      forward_iterator_wrapper& operator++(){
        access_.Next();
        return *this;
      }
      // Preincrement
      forward_iterator_wrapper operator++(int){
        forward_iterator_wrapper ret{ *this };
        ++ret;
        return ret;
      }

      proxy<T>& operator*(){
        return proxy_;
      }

      const proxy<T>& operator*() const{
        return proxy_;
      }

      bool operator==(const forward_iterator_wrapper& other){
        return compare_.Equals(other.compare_);
      }
      bool operator!=(const forward_iterator_wrapper& other){
        return !compare_.Equals(other.compare_);
      }

    };

    template<class T>
    struct bidirectional_iterator_wrapper :std::iterator<std::bidirectional_iterator_tag, proxy<T>, std::int64_t>
    {
    private:
      mutable use<IReader<T>> reader_;
      use<IWriter<T>> writer_;
      use <IBidirectionalAccess> access_;
      use <IEqualityComparable> compare_;
      proxy<T> proxy_;

    public:
      bidirectional_iterator_wrapper(use<InterfaceUnknown> iunk)
        :reader_{ iunk.QueryInterface<IReader<T>>() },
        writer_{ iunk.QueryInterface<IWriter<T>>() },
        access_{ iunk.QueryInterface<IBidirectionalAccess>() },
        compare_{ iunk.QueryInterface<IEqualityComparable>() },
        proxy_{ iunk }
      {}


      bidirectional_iterator_wrapper(const bidirectional_iterator_wrapper& other)
        :reader_{ other.reader_.template QueryInterface<IClonable>().Clone().template QueryInterface<IReader<T>>() },
        writer_{ reader_.template QueryInterface<IWriter<T>>() },
        access_{ reader_.template QueryInterface<IBidirectionalAccess>() },
        compare_{ reader_.template QueryInterface<IEqualityComparable>() },
        proxy_{ reader_ }
      {  }

      bidirectional_iterator_wrapper(bidirectional_iterator_wrapper&& other)
        :reader_{ std::move(other.reader_) },
        writer_{ std::move(other.writer_) },
        access_{ std::move(other.access_) },
        compare_{ std::move(other.compare_) },
        proxy_{ reader_ }
      {
        other.proxy_.cppcomponents_iterator_proxy_assign(nullptr);

      }



      bidirectional_iterator_wrapper& operator=(bidirectional_iterator_wrapper&& other){
        reader_ = std::move(other.reader_);
        writer_ = std::move(other.writer_);
        access_ = std::move(other.access_);
        compare_ = std::move(other.compare_);
        proxy_.cppcomponents_iterator_proxy_assign(reader_);
        other.proxy_.cppcomponents_iterator_proxy_assign(nullptr);
        return *this;
      }

      bidirectional_iterator_wrapper& operator=(const bidirectional_iterator_wrapper& other){
        bidirectional_iterator_wrapper temp{ other };
        *this = std::move(temp);
        return *this;
      }

      // Preincrement
      bidirectional_iterator_wrapper& operator++(){
        access_.Next();
        return *this;
      }
      // Preincrement
      bidirectional_iterator_wrapper operator++(int){
        bidirectional_iterator_wrapper ret{ *this };
        ++ret;
        return ret;
      }
      // Predecrement
      bidirectional_iterator_wrapper& operator--(){
        access_.Previous();
        return *this;
      }
      // Predecrement
      bidirectional_iterator_wrapper operator--(int){
        bidirectional_iterator_wrapper ret{ *this };
        --ret;
        return ret;
      }

      proxy<T>& operator*(){
        return proxy_;
      }

      const proxy<T>& operator*() const{
        return proxy_;
      }

      bool operator==(const bidirectional_iterator_wrapper& other){
        return compare_.Equals(other.compare_);
      }
      bool operator!=(const bidirectional_iterator_wrapper& other){
        return !compare_.Equals(other.compare_);
      }

    };
    template<class T>
    struct random_access_iterator_wrapper :std::iterator<std::random_access_iterator_tag, proxy<T>, std::int64_t>
    {
    private:
      mutable use<IReader<T>> reader_;
      use<IWriter<T>> writer_;
      use <IRandomAccess> access_;
      use <IComparable> compare_;
      proxy<T> proxy_;

    public:
      random_access_iterator_wrapper(use<InterfaceUnknown> iunk)
        :reader_{ iunk.QueryInterface<IReader<T>>() },
        writer_{ iunk.QueryInterface<IWriter<T>>() },
        access_{ iunk.QueryInterface<IRandomAccess>() },
        compare_{ iunk.QueryInterface<IComparable>() },
        proxy_{ iunk }
      {}


      random_access_iterator_wrapper(const random_access_iterator_wrapper& other)
        :reader_{ other.reader_.template QueryInterface<IClonable>().Clone().template QueryInterface<IReader<T>>() },
        writer_{ reader_.template QueryInterface<IWriter<T>>() },
        access_{ reader_.template QueryInterface<IRandomAccess>() },
        compare_{ reader_.template QueryInterface<IComparable>() },
        proxy_{ reader_ }
      {

      }


      random_access_iterator_wrapper(random_access_iterator_wrapper&& other)
        :reader_{ std::move(other.reader_) },
        writer_{ std::move(other.writer_) },
        access_{ std::move(other.access_) },
        compare_{ std::move(other.compare_) },
        proxy_{ reader_ }
      { 
        other.proxy_.cppcomponents_iterator_proxy_assign(nullptr);

      }

      random_access_iterator_wrapper& operator=(random_access_iterator_wrapper&& other){
        reader_ = std::move(other.reader_);
        writer_ = std::move(other.writer_);
        access_ = std::move(other.access_);
        compare_ = std::move(other.compare_);
        proxy_.cppcomponents_iterator_proxy_assign(reader_.template QueryInterface<InterfaceUnknown>());
        other.proxy_.cppcomponents_iterator_proxy_assign(nullptr);

        return *this;
      }

      random_access_iterator_wrapper& operator=(const random_access_iterator_wrapper& other){
        random_access_iterator_wrapper temp{ other };
        *this = std::move(temp);
        return *this;
      }

      // Preincrement
      random_access_iterator_wrapper& operator++(){
        access_.Next();
        return *this;
      }
      // Preincrement
      random_access_iterator_wrapper operator++(int){
        random_access_iterator_wrapper ret{ *this };
        ++ret;
        return ret;
      }
      // Predecrement
      random_access_iterator_wrapper& operator--(){
        access_.Previous();
        return *this;
      }
      // Predecrement
      random_access_iterator_wrapper operator--(int){
        random_access_iterator_wrapper ret{ *this };
        --ret;
        return ret;
      }

      proxy<T>& operator*(){
        return proxy_;
      }

      const proxy<T>& operator*() const{
        return proxy_;
      }


      bool operator==(const random_access_iterator_wrapper& other){
        return compare_.Equals(other.compare_);
      }
      bool operator!=(const random_access_iterator_wrapper& other){
        return !compare_.Equals(other.compare_);
      }
      bool operator<(const random_access_iterator_wrapper& other){
         return compare_.Compare(other.compare_) < 0;
      }
      bool operator>(const random_access_iterator_wrapper& other){
        return compare_.Compare(other.compare_) > 0;
      }
      bool operator<=(const random_access_iterator_wrapper& other){
         return compare_.Compare(other.compare_) <= 0;
      }
      bool operator>=(const random_access_iterator_wrapper& other){
        return compare_.Compare(other.compare_) >= 0;
      }

      random_access_iterator_wrapper& operator+=(std::int64_t i){
        access_.Advance(i);
        return *this;
      }
      random_access_iterator_wrapper& operator-=(std::int64_t i){
        access_.Advance(-i);
        return *this;
      }

      random_access_iterator_wrapper operator+(std::int64_t i)const{
        random_access_iterator_wrapper other{ *this };
        other += i;
        return other;
      }
      random_access_iterator_wrapper operator-(std::int64_t i)const{
        random_access_iterator_wrapper other{ *this };
        other -= i;
        return other;
      }
      std::int64_t operator-(const random_access_iterator_wrapper& other)const{
        return other.access_.Distance(access_);
      }

      const proxy<T> operator[](std::int64_t i)const{
        return *((*this) + i);
      }
    };

    namespace detail{

      template<class Iterator, class TUUID>
      struct IGetNativeIterator :define_interface<TUUID>
      {
        void* GetRaw();
        CPPCOMPONENTS_CONSTRUCT_TEMPLATE(IGetNativeIterator, GetRaw)

        CPPCOMPONENTS_INTERFACE_EXTRAS(IGetNativeIterator){
          Iterator& Get(){ return *static_cast<Iterator*>(this->get_interface().GetRaw()); }
        };

      };

      template<class Derived, class Iterator, class T>
      struct ImplementReader{

        Iterator& get_iterator(){
          return  static_cast<Derived*>(this)->iter_;
        }
        typename std::remove_const<T>::type IReader_Read(){
          return *get_iterator();
        }
      };
      template<class Derived, class Iterator, class T, class Ref = typename std::iterator_traits<Iterator>::reference>
      struct ImplementWriter{

        Iterator& get_iterator(){
          return  static_cast<Derived*>(this)->iter_;
        }
        void IWriter_Write(typename std::remove_const<T>::type t){
          *get_iterator() = std::move(t);
        }
      };     
      template<class Derived, class Iterator, class T, class Ref>
      struct ImplementWriter<Derived,Iterator,T, const Ref&>{

        Iterator& get_iterator(){
          return  static_cast<Derived*>(this)->iter_;
        }
        void IWriter_Write(typename std::remove_const<T>::type t){
          throw error_not_implemented();
        }
      };

      template<class Derived, class Iterator, class T>
      struct ImplementForwardAccess{

        Iterator& get_iterator(){
          return  static_cast<Derived*>(this)->iter_;
        }
        void IForwardAccess_Next(){
           ++(get_iterator());
        }
      };
      template<class Derived, class Iterator, class T>
      struct ImplementBidirectionalAccess :ImplementForwardAccess<Derived,Iterator, T>{

        Iterator& get_iterator(){
          return  static_cast<Derived*>(this)->iter_;
        }
        void IBidirectionalAccess_Previous(){
           --(get_iterator());
        }
      };

      template<class Derived, class Iterator, class T, class TUUID>
      struct ImplementRandomAccess :ImplementBidirectionalAccess<Derived,Iterator,T>{
        Iterator& get_iterator(){
          return  static_cast<Derived*>(this)->iter_;
        }
        Iterator& get_other_iterator(use<InterfaceUnknown> other){
          return other.QueryInterface<IGetNativeIterator<Iterator, TUUID>>().Get();
        }


        void IRandomAccess_Advance(std::int64_t i){
          (get_iterator()) += static_cast<std::ptrdiff_t>(i);
        }
        std::int64_t IRandomAccess_Distance(use<IRandomAccess> other){
        return get_other_iterator(other) - (get_iterator());
        }
      };
      template<class Derived, class Iterator, class TUUID>
      struct ImplementEqualityComparable{

        Iterator& get_iterator(){
          return  static_cast<Derived*>(this)->iter_;
        }
        Iterator& get_other_iterator(use<InterfaceUnknown> other){
          return other.QueryInterface<IGetNativeIterator<Iterator, TUUID>>().Get();
        }

        bool IEqualityComparable_Equals(use<InterfaceUnknown> other){
          return get_iterator() == get_other_iterator(other);
        }

      };
      template<class Derived, class Iterator, class TUUID>
      struct ImplementComparable:ImplementEqualityComparable<Derived,Iterator,TUUID>{

        Iterator& get_iterator(){
          return  static_cast<Derived*>(this)->iter_;
        }
        Iterator& get_other_iterator(use<InterfaceUnknown> other){
          return other.QueryInterface<IGetNativeIterator<Iterator, TUUID>>().Get();
        }

        std::int32_t IComparable_Compare(use<InterfaceUnknown> other){
          auto& iter = get_iterator();
          auto& other_iter = get_other_iterator(other);

          if (iter < other_iter){
            return -1;
          }
          if (iter > other_iter){
            return 1;
          }
          return 0;
        }

      };

      


      inline std::string dummy_iterator_id(){ return "cppcomponents::uuid<0xa4f6b262, 0xef6f, 0x42d6, 0x9608, 0xa1b751d341d1>"; }

      template<class Iter, class TUUID,class T =typename std::iterator_traits<Iter>::value_type >
      struct implement_input_iterator :implement_runtime_class<implement_input_iterator<Iter,TUUID,T>,
        runtime_class<dummy_iterator_id, object_interfaces<IGetNativeIterator<Iter, TUUID>, IReader<T>, IForwardAccess, IEqualityComparable, IClonable>, factory_interface<NoConstructorFactoryInterface>>
      >, ImplementEqualityComparable<implement_input_iterator<Iter, TUUID, T>, Iter, TUUID>, ImplementReader<implement_input_iterator<Iter, TUUID, T>, Iter, T>,
      ImplementForwardAccess<implement_input_iterator<Iter, TUUID, T>,Iter, T>
      {
        typedef Iter iterator_t;
        Iter iter_;

        implement_input_iterator(iterator_t iter) :iter_{ std::move(iter) }{}

        void* GetRaw(){ return &iter_; }

        use<InterfaceUnknown> IClonable_Clone(){
          return implement_input_iterator::create(*this);
        }
        implement_input_iterator(const implement_input_iterator& other) :iter_{ other.iter_ }{}

      };
      template<class Iter, class TUUID, class T = typename std::iterator_traits<Iter>::value_type>
      struct implement_output_iterator :implement_runtime_class<implement_output_iterator<Iter,TUUID,T>,
        runtime_class<dummy_iterator_id, object_interfaces<IGetNativeIterator<Iter, TUUID>, IWriter<T>, IForwardAccess, IClonable>, factory_interface<NoConstructorFactoryInterface>>
      >,  ImplementWriter<implement_output_iterator<Iter, TUUID, T>, Iter, T>,
      ImplementForwardAccess<implement_output_iterator<Iter, TUUID, T>, Iter, T>
      {
        typedef Iter iterator_t;
        Iter iter_;

        implement_output_iterator(iterator_t iter) :iter_{ std::move(iter) }{}

        void* GetRaw(){ return &iter_; }
        use<InterfaceUnknown> IClonable_Clone(){
          return implement_output_iterator::create(*this);
        }
        implement_output_iterator(const implement_output_iterator& other) :iter_{ other.iter_ }{}


      };
      template<class Iter, class TUUID, class T = typename std::iterator_traits<Iter>::value_type>
      struct implement_forward_iterator :implement_runtime_class<implement_forward_iterator<Iter,TUUID,T>,
        runtime_class<dummy_iterator_id, object_interfaces<IGetNativeIterator<Iter, TUUID>, IReader<T>, IWriter<T>, IForwardAccess, IEqualityComparable,IClonable>, factory_interface<NoConstructorFactoryInterface>>
      >, ImplementEqualityComparable<implement_forward_iterator<Iter, TUUID, T>, Iter, TUUID>, ImplementReader<implement_forward_iterator<Iter, TUUID, T>, Iter, T>,
      ImplementWriter<implement_forward_iterator<Iter, TUUID, T>, Iter, T>,
      ImplementForwardAccess<implement_forward_iterator<Iter, TUUID, T>, Iter, T>
      {
        typedef Iter iterator_t;
        Iter iter_;

        implement_forward_iterator(iterator_t iter) :iter_{ std::move(iter) }{}

        void* GetRaw(){ return &iter_; }
	
        use<InterfaceUnknown> IClonable_Clone(){
          return implement_forward_iterator::create(iter_);
        }
        implement_forward_iterator(const implement_forward_iterator& other) :iter_{ other.iter_ }{}

      };
      template<class Iter, class TUUID, class T = typename std::iterator_traits<Iter>::value_type>
      struct implement_bidirectional_iterator :implement_runtime_class<implement_bidirectional_iterator<Iter, TUUID, T>,
        runtime_class<dummy_iterator_id, object_interfaces<IGetNativeIterator<Iter, TUUID>, IReader<T>, IWriter<T>, IBidirectionalAccess, IEqualityComparable,IClonable>, factory_interface<NoConstructorFactoryInterface>>
      >, ImplementEqualityComparable<implement_bidirectional_iterator<Iter, TUUID, T>, Iter, TUUID>, ImplementReader<implement_bidirectional_iterator<Iter, TUUID, T>, Iter, T>,
      ImplementWriter<implement_bidirectional_iterator<Iter, TUUID, T>, Iter, T>,
      ImplementBidirectionalAccess<implement_bidirectional_iterator<Iter, TUUID, T>, Iter, T>
      {
        typedef Iter iterator_t;
        Iter iter_;

        implement_bidirectional_iterator(iterator_t iter) :iter_{ std::move(iter) }{}

        void* GetRaw(){ return &iter_; }
        use<InterfaceUnknown> IClonable_Clone(){
          return implement_bidirectional_iterator::create(*this);
        }
        implement_bidirectional_iterator(const implement_bidirectional_iterator& other) :iter_{ other.iter_ }{}

      };
      template<class Iter, class TUUID, class T = typename std::iterator_traits<Iter>::value_type>
      struct implement_random_access_iterator :implement_runtime_class<implement_random_access_iterator<Iter, TUUID, T>,
        runtime_class<dummy_iterator_id, object_interfaces<IGetNativeIterator<Iter, TUUID>, IReader<T>, IWriter<T>, IRandomAccess, IComparable,IClonable>, factory_interface<NoConstructorFactoryInterface>>
      >, ImplementComparable<implement_random_access_iterator<Iter, TUUID, T>, Iter, TUUID>, ImplementReader<implement_random_access_iterator<Iter, TUUID, T>, Iter, T>,
      ImplementWriter<implement_random_access_iterator<Iter, TUUID, T>, Iter, T>,
      ImplementRandomAccess<implement_random_access_iterator<Iter, TUUID, T>, Iter, T, TUUID>
      {
        typedef Iter iterator_t;
        Iter iter_;

        implement_random_access_iterator(iterator_t iter) :iter_{ std::move(iter) }{}

        void* GetRaw(){ return &iter_; }

        use<InterfaceUnknown> IClonable_Clone(){
          return implement_random_access_iterator::create(iter_);
        }
        implement_random_access_iterator(const implement_random_access_iterator& other) :iter_{ other.iter_ }{}

      };

      template<class TUUID, class Iterator>
      use<InterfaceUnknown> get_iterator_helper(std::input_iterator_tag, Iterator i){
        return implement_input_iterator<Iterator, TUUID>::create(i);
      }
      template<class TUUID, class Iterator>
      use<InterfaceUnknown> get_iterator_helper(std::forward_iterator_tag, Iterator i){
        return implement_forward_iterator<Iterator, TUUID>::create(i);
      }
      template<class TUUID, class Iterator>
      use<InterfaceUnknown> get_iterator_helper(std::bidirectional_iterator_tag, Iterator i){
        return implement_bidirectional_iterator<Iterator, TUUID>::create(i);
      }
      template<class TUUID, class Iterator>
      use<InterfaceUnknown> get_iterator_helper(std::random_access_iterator_tag, Iterator i){
        return implement_random_access_iterator<Iterator, TUUID>::create(i);
      }
    }

    template<class TUUID, class Iterator>
    use<InterfaceUnknown> make_iterator(Iterator i){
      return detail::get_iterator_helper<TUUID>(typename std::iterator_traits<Iterator>::iterator_category{}, i);
    }
    template<class TUUID, class T,class Iterator>
    use<InterfaceUnknown> make_output_iterator(Iterator i){
      return detail::implement_output_iterator<Iterator, TUUID,T>::create(i);
    }
  }
}

#endif
/* 
   Copyright (c) Marshall Clow 2012-2012.

   Distributed under the Boost Software License, Version 1.0. (See accompanying
   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

    For more information, see http://www.boost.org
    
    Based on the StringRef implementation in LLVM (http://llvm.org) and
    N3422 by Jeffrey Yasskin
        http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3442.html

*/
// Modified by John R. Bandela 3/27/2013 to adapt to cppcomponents
// renamed to string_ref to prevent future conflicts
// changed to store ptr,size with 1 bit of size to indicate if null-terminated
// note this makes this implementation abi incompatible with previous implementation

#ifndef CROSS_COMPILER_string_ref_HPP
#define CROSS_COMPILER_string_ref_HPP


#include <stdexcept>
#include <algorithm>
#include <functional>
#include <string>

namespace cppcomponents {
    
    namespace detail {
    //  A helper functor because sometimes we don't have lambdas
        template <typename charT, typename traits>
        class string_ref_traits_eq {
        public:
            string_ref_traits_eq ( charT ch ) : ch_(ch) {}
            bool operator () ( charT val ) const { return traits::eq ( ch_, val ); }
            charT ch_;
            };
        }
    
    template<typename charT, typename traits> class basic_string_ref;
    typedef basic_string_ref<char,     std::char_traits<char> >        string_ref;
    typedef basic_string_ref<char16_t,  std::char_traits<char16_t> >    u16string_ref;
	typedef basic_string_ref<char32_t, std::char_traits<char32_t> >    u32string_ref;
	typedef basic_string_ref<wchar_t, std::char_traits<wchar_t> >    wstring_ref;

    
    template<typename charT, typename traits>
    class basic_string_ref {
    public:
        // types
        typedef charT value_type;
        typedef const charT* pointer;
        typedef const charT& reference;
        typedef const charT& const_reference;
        typedef pointer const_iterator; // impl-defined
        typedef const_iterator iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef const_reverse_iterator reverse_iterator;
        typedef std::size_t size_type;
        typedef ptrdiff_t difference_type;
		enum: std::size_t{npos = static_cast<std::size_t>(-1)};
        
        // construct/copy
        basic_string_ref ()
            : ptr_(nullptr), size_with_null_terminated_bit_(0) {}

		// By having default copy ctor and op= this becomes trvially
		// copyable
        //basic_string_ref (const basic_string_ref &rhs)
        //    : ptr_(rhs.ptr_), end_(rhs.end_) {}

        //basic_string_ref& operator=(const basic_string_ref &rhs) {
        //    ptr_ = rhs.ptr_;
        //    end_= rhs.end_;
        //    return *this;
        //    }
            
        basic_string_ref(const charT* str)
			: ptr_(str), size_with_null_terminated_bit_( ptr_? calc_size_with_null_terminated_bit(traits::length(str),true) : 0) {}

        template<typename Allocator>
        basic_string_ref(const std::basic_string<charT, traits, Allocator>& str)
            : ptr_(str.data()), size_with_null_terminated_bit_(calc_size_with_null_terminated_bit(str.length(),true)) {}

        basic_string_ref(const charT* str, size_type len, bool is_null_terminated =  false)
			: ptr_(str), size_with_null_terminated_bit_(calc_size_with_null_terminated_bit(len,is_null_terminated)) {}

        template<typename Allocator>
        explicit operator std::basic_string<charT, traits, Allocator>() const {
            return std::basic_string<charT, traits, Allocator> ( ptr_, end() );
            }

		std::basic_string<charT, traits> to_string(){
			return std::basic_string<charT,traits>(ptr_,end());
		}
		// check for null termination
		bool null_terminated(){
			if (get_null_terminated() == true){
				return true;
			}
			if (size() > 0 && back() == 0){
				return true;
			}
			return false;
		}

        // iterators
        const_iterator   begin() const { return ptr_; }
        const_iterator  cbegin() const { return ptr_; }
        const_iterator     end() const { return ptr_ + size(); }
        const_iterator    cend() const { return end(); }
                const_reverse_iterator  rbegin() const { return const_reverse_iterator (end()); }
                const_reverse_iterator crbegin() const { return const_reverse_iterator (end()); }
                const_reverse_iterator    rend() const { return const_reverse_iterator (begin()); }
                const_reverse_iterator   crend() const { return const_reverse_iterator (begin()); }
        
        // capacity
        size_type size()     const { return get_size(); }
        size_type length()   const { return size(); }
        size_type max_size() const { return size(); }
        bool empty()         const { return size()== 0; }
        
        // element access
        const charT& operator[](size_type pos) const { return ptr_[pos]; }

        const charT& at(size_t pos) const {
            if ( pos >= size())
                throw std::out_of_range ( "boost::string_ref::at" );
            return ptr_[pos];
            }
            
        const charT& front() const { return ptr_[0]; }
        const charT& back()  const { return ptr_[size()-1]; }
        const charT* data()  const { return ptr_; }
        
        // modifiers
		void clear() { ptr_ = nullptr; size_with_null_terminated_bit_ = 0; }
		void remove_prefix(size_type n) {
			if (n > size())
				n = size();

			set_size(size() - n);
			ptr_ += n;
		}
            
		void remove_suffix(size_type n) {
			if (n > size())
				n = size(); 

			auto new_sz = size() - n;

			if (new_sz < size() && size() > 0 && ptr_[new_sz] == 0){
				set_null_terminated(true); 
			}
			else{
				set_null_terminated(false);
			}
			set_size(new_sz);
		}
            
        
        // basic_string_ref string operations
        
		basic_string_ref substr(size_type pos, size_type n = npos) const {
			if (pos > size()){
				throw std::out_of_range("string_ref::substr");
			}

			auto new_sz = n == npos || pos + n > size() ? size() - pos : n;

			bool is_null_terminated = false;

			// if the string end is within this string, check if string end is 0
			if (pos + new_sz < size() && data()[pos + new_sz] == 0){
				is_null_terminated = true;
			}

			// if the string end is this current string end, check if we are null terminated
			if (pos + new_sz == size()){
				is_null_terminated = get_null_terminated();
			}

			return basic_string_ref(data() + pos, new_sz,is_null_terminated);
		}
        
        int compare(basic_string_ref x) const {
            int cmp = traits::compare ( ptr_, x.ptr_, (std::min)(size(), x.size()));
            return cmp != 0 ? cmp : ( size()== x.size()? 0 : size()< x.size()? -1 : 1 );
            }
        
        bool starts_with(charT c) const { return !empty() && traits::eq ( c, front()); }
        bool starts_with(basic_string_ref x) const {
            return size()>= x.size()&& traits::compare ( ptr_, x.ptr_, x.size()) == 0;
            }
        
        bool ends_with(charT c) const { return !empty() && traits::eq ( c, back()); }
        bool ends_with(basic_string_ref x) const {
            return size()>= x.size() && traits::compare ( ptr_ + size() - x.size(), x.ptr_, x.size()) == 0;
            }

        size_type find(basic_string_ref s) const {
            const_iterator iter = std::search ( this->cbegin (), this->cend (), 
                                                s.cbegin (), s.cend (), traits::eq );
            return iter = this->cend () ? npos : std::distance ( this->cbegin (), iter );
            }
        
        size_type find(charT c) const {
            const_iterator iter = std::find_if ( this->cbegin (), this->cend (), 
                                    detail::string_ref_traits_eq<charT, traits> ( c ));
            return iter == this->cend () ? npos : std::distance ( this->cbegin (), iter );
            }
                        
        size_type rfind(basic_string_ref s) const {
            const_reverse_iterator iter = std::search ( this->crbegin (), this->crend (), 
                                                s.crbegin (), s.crend (), traits::eq );
            return iter == this->crend () ? npos : reverse_distance ( this->crbegin (), iter );
            }

        size_type rfind(charT c) const {
            const_reverse_iterator iter = std::find_if ( this->crbegin (), this->crend (), 
                                    detail::string_ref_traits_eq<charT, traits> ( c ));
            return iter == this->crend () ? npos : reverse_distance ( this->crbegin (), iter );
            }
        
        size_type find_first_of(charT c) const { return  find (c); }
        size_type find_last_of (charT c) const { return rfind (c); }
        
        size_type find_first_of(basic_string_ref s) const {
            const_iterator iter = std::find_first_of 
            	( this->cbegin (), this->cend (), s.cbegin (), s.cend (), traits::eq );
            return iter == this->cend () ? npos : std::distance ( this->cbegin (), iter );
            }
        
        size_type find_last_of(basic_string_ref s) const {
            const_reverse_iterator iter = std::find_first_of 
                ( this->crbegin (), this->crend (), s.cbegin (), s.cend (), traits::eq );
            return iter == this->crend () ? npos : reverse_distance ( this->crbegin (), iter);
            }
        
        size_type find_first_not_of(basic_string_ref s) const {
        	const_iterator iter = find_not_of ( this->cbegin (), this->cend (), s );
            return iter == this->cend () ? npos : std::distance ( this->cbegin (), iter );
            }
        
        size_type find_first_not_of(charT c) const {
            for ( const_iterator iter = this->cbegin (); iter != this->cend (); ++iter )
                if ( !traits::eq ( c, *iter ))
                    return std::distance ( this->cbegin (), iter );
            return npos;
            }
        
        size_type find_last_not_of(basic_string_ref s) const {
        	const_reverse_iterator iter = find_not_of ( this->crbegin (), this->crend (), s );
            return iter == this->crend () ? npos : reverse_distance ( this->crbegin (), iter );
            }
        
        size_type find_last_not_of(charT c) const {
            for ( const_reverse_iterator iter = this->crbegin (); iter != this->crend (); ++iter )
                if ( !traits::eq ( c, *iter ))
                    return reverse_distance ( this->crbegin (), iter );
            return npos;
            }

    private:
        template <typename r_iter>
        size_type reverse_distance ( r_iter first, r_iter last ) const {
            return size()- 1 - std::distance ( first, last );
            }
        
        template <typename Iterator>
        Iterator find_not_of ( Iterator first, Iterator last, basic_string_ref s ) const {
			for ( ; first != last ; ++first )
				if ( 0 == traits::find ( s.ptr_, s.size(), *first ))
					return first;
			return last;
			}
		
		
		std::size_t calc_size_with_null_terminated_bit(std::size_t sz, bool b)const{
			std::size_t new_sz = sz << 1;
			if ((new_sz >> 1) != sz){
				throw std::length_error("invalid length passed to basic_string_ref");
			}

			if (b){
				new_sz |= 1;
			}
			return new_sz;

		}
		std::size_t get_size()const{
			// get rid of null terminated indicator
			return (size_with_null_terminated_bit_ >> 1);
		}

		void set_size(size_t sz){

			size_with_null_terminated_bit_ = calc_size_with_null_terminated_bit(sz, get_null_terminated());
		}

		bool get_null_terminated()const{
			return (size_with_null_terminated_bit_ & 1) == 1;
		}

		void set_null_terminated(bool b){
			size_with_null_terminated_bit_ = calc_size_with_null_terminated_bit(get_size(), b);
		}
        
        const charT *ptr_;
		std::size_t size_with_null_terminated_bit_;
        }INTERNAL_MACRO_CPPCOMPONENTS_PACK;
    
    // Comparison operators
    template<typename charT, typename traits>
    bool operator==(basic_string_ref<charT, traits> x, basic_string_ref<charT, traits> y) {
        if ( x.size () != y.size ()) return false;
        return x.compare(y) == 0;
        }
    
    template<typename charT, typename traits>
    bool operator!=(basic_string_ref<charT, traits> x, basic_string_ref<charT, traits> y) {
        if ( x.size () != y.size ()) return true;
        return x.compare(y) != 0;
        }

    template<typename charT, typename traits>
    bool operator<(basic_string_ref<charT, traits> x, basic_string_ref<charT, traits> y) {
        return x.compare(y) < 0;
        }

    template<typename charT, typename traits>
    bool operator>(basic_string_ref<charT, traits> x, basic_string_ref<charT, traits> y) {
        return x.compare(y) > 0;
        }

    template<typename charT, typename traits>
    bool operator<=(basic_string_ref<charT, traits> x, basic_string_ref<charT, traits> y) {
        return x.compare(y) <= 0;
        }

    template<typename charT, typename traits>
    bool operator>=(basic_string_ref<charT, traits> x, basic_string_ref<charT, traits> y) {
        return x.compare(y) >= 0;
        }

    
    // Inserter
    template<class charT, class traits>
    std::basic_ostream<charT, traits>&
    operator<<(std::basic_ostream<charT, traits>& os, const basic_string_ref<charT,traits>& str) {
        for ( charT x : str )
            os << x;
        return os;
        }
    


}



#endif

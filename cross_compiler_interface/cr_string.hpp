//          Copyright John R. Bandela 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma  once
#ifndef JRB_CR_STRING_HPP
#define JRB_CR_STRING_HPP



#include <algorithm>
#include "cross_compiler_interface.hpp"
#include <stdexcept>
#include <iterator>

namespace cross_compiler_interface {
class cr_string{

public:
	typedef const char* pchar;
	pchar begin_end[2];
	typedef char value_type;//  Traits::char_type  


	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type; 

	typedef const value_type& reference ; 
	typedef const value_type& const_reference;

	typedef const value_type* pointer;  

	typedef pointer const_pointer; 

	typedef pointer	iterator;

	typedef const_pointer const_iterator;  

	typedef std::reverse_iterator<iterator> reverse_iterator; 

	typedef reverse_iterator const_reverse_iterator;

	cr_string(const char* s){
		begin_end[0] = s;
		begin_end[1] = s;
		for(;*begin_end[1];begin_end[1]++);

	}

	cr_string(const std::string& s){
		begin_end[0] = (char*)&s[0];
		begin_end[1] = begin_end[0] + s.size();

	}

	size_type size()const{return end() - begin();}

	const_reference at(size_type pos)const{
		if(pos >= size()) throw std::out_of_range("cr_string");
		return begin()[pos];
	}

	bool empty()const{return begin()==end();}

	size_t length()const{return size();}

	size_t max_size()const{return size();}
	size_t capacity()const{return size();}


	const_reference operator[](size_type pos){
		return begin()[pos];
	}

	const_reference front()const{
		return begin()[0];
	}
	const_reference back()const{
		return begin()[size()-1];
	}

	const_pointer data()const{return begin();}

	const_pointer c_str()const{return begin();}

	const_iterator begin()const{return begin_end[0];}
	const_iterator cbegin()const{return begin();}

	const_iterator end()const{return begin_end[1];}
	const_iterator cend()const{return end();}

	const_reverse_iterator rbegin()const{return const_reverse_iterator(begin());}
	const_reverse_iterator crbegin()const{return const_reverse_iterator(begin());}

	const_reverse_iterator rend()const{return const_reverse_iterator(end());}
	const_reverse_iterator crend()const{return const_reverse_iterator(end());}


	enum{npos = -1};

	operator std::string ()const{return std::string(begin(),end());}


	// To do add the find, substr stuff
};

bool operator==(const cr_string& a,const cr_string& b){
	if(a.size() != b.size()){
		return false;
	}

	return std::equal(a.begin(),a.end(),b.begin());
}

bool operator==(const cr_string& a, const std::string& b){
	return a==cr_string(b);
}
bool operator==(const std::string& a, const cr_string& b){
	return cr_string(a)==b;
}

bool operator<(const cr_string& a,const cr_string& b){


	return std::lexicographical_compare(a.begin(),a.end(),b.begin(),b.end());
}
bool operator<(const cr_string& a, const std::string& b){
	return a<cr_string(b);
}
bool operator<(const std::string& a, const cr_string& b){
	return cr_string(a)<b;
}

bool operator>(const cr_string& a,const cr_string& b){
	return ( !(a==b) && !(a<b) );
}

bool operator>(const cr_string& a, const std::string& b){
	return a>cr_string(b);
}
bool operator>(const std::string& a, const cr_string& b){
	return cr_string(a)>b;
}

bool operator!=(const cr_string& a,const cr_string& b){
	return ( !(a==b));
}

bool operator!=(const cr_string& a, const std::string& b){
	return a != cr_string(b);
}
bool operator!=(const std::string& a, const cr_string& b){
	return cr_string(a) != b;
}

//
template<>
struct cross_conversion<cr_string>:public trivial_conversion<cr_string>{};
}
#endif // !JRB_CR_STRING_HPP


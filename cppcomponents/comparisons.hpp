#pragma once
#ifndef INCLUDE_GUARD_CPPCOMPONENTS_COMPARISONS_HPP_12_19_2013_
#define INCLUDE_GUARD_CPPCOMPONENTS_COMPARISONS_HPP_12_19_2013_
#include "cppcomponents.hpp"

namespace cppcomponents{

  struct IEqualityComparable :define_interface<cppcomponents::uuid<0x4185a896, 0xdfab, 0x411f, 0xb65f, 0x5ece96943dbe>>{
    bool Equals(use<InterfaceUnknown> other);

    CPPCOMPONENTS_CONSTRUCT(IEqualityComparable, Equals)
  };

  struct IComparable :define_interface<cppcomponents::uuid<0xbf9521aa, 0x0d36, 0x456b, 0xb576, 0x5f235dfb711a>,IEqualityComparable>
  {
    // 0 - Equals; -1 Less than; 1 Greater than
    std::int32_t Compare(use<InterfaceUnknown> other);

    CPPCOMPONENTS_CONSTRUCT(IComparable, Compare)
  };

}

#endif
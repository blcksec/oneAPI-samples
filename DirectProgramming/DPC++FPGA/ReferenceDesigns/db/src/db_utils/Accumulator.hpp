#ifndef __ACCUMULATOR_HPP__
#define __ACCUMULATOR_HPP__
#pragma once

#include <limits>
#include <type_traits>

#include "Tuple.hpp"
#include "Unroller.hpp"

///////////////////////////////////////////////////////
//
// Register-based accumulator
// Accumulates into size bins of type StorageType
//
template <typename StorageType, int size, typename IndexType = unsigned int>
class RegisterAccumulator {
  // static asserts
  static_assert(std::is_arithmetic<StorageType>::value,
                "StorageType must be arithmetic to support accumulation");
  static_assert(std::is_integral<IndexType>::value,
                "IndexType must be an integral type");
  static_assert(std::numeric_limits<IndexType>::max() >= (size - 1),
                "IndexType must be large enough to index the entire array");

 public:
  // initialize the memory to 0
  void Init() {
    UnrolledLoop<0, size>([&](auto i) { 
      registers.template get<i>() = 0;
    });
  }

  // accumulate 'value' into register 'index' (i.e. registers[index] += value)
  void Accumulate(IndexType index, StorageType value) {
    UnrolledLoop<0, size>([&](auto i) {
      registers.template get<i>() += (i == index) ? value : 0;
    });
  }

  // template version of accumulate
  template <IndexType index>
  void Accumulate(StorageType value) {
    static_assert(index < size, "index is out of range");
    registers.template get<index>() += value;
  }

  // get the value of memory at 'index'
  StorageType Get(IndexType index) {
    StorageType ret;
    UnrolledLoop<0, size>([&](auto i) {
      if (i == index) {
        ret = registers.template get<i>();
      }
    });

    return ret;
  }

  // template version of get
  template <IndexType index>
  StorageType Get() {
    static_assert(index < size, "index is out of range");
    return registers.template get<index>();
  }

  // register storage using tuples
  NTuple<size, StorageType> registers;
};
///////////////////////////////////////////////////////

#endif /* __ACCUMULATOR_HPP__ */

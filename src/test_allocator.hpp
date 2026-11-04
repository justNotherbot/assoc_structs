#pragma once

#include <cassert>
#include <cstddef>
#include <cstdlib>

#include <iostream>
#include <stdexcept>

namespace tree_check {

template <typename T>
class TestAllocator final {
  std::size_t diff_ = 0;

 public:
  using value_type = T;

  TestAllocator() = default;

  TestAllocator(const TestAllocator& other) = default;

  TestAllocator& operator=(const TestAllocator& other) = default;

  T* allocate(std::size_t cnt) {
    T* out = static_cast<T*>(std::malloc(cnt * sizeof(T)));
    if (out == nullptr) {
      throw std::bad_alloc{};
    }
    diff_ += cnt;
    return out;
  }

  void deallocate(T* ptr, std::size_t cnt) {
    if (ptr == nullptr) {
      assert(cnt == 0);
      std::cerr << "Test allocator: nullptr provided to deallocate\n";
      return;
    }
    assert(cnt <= diff_);
    std::free(ptr);
    diff_ -= cnt;
  }

  friend bool operator==(
    const TestAllocator&, const TestAllocator&) noexcept {
    return true;
  }

  friend bool operator!=(
    const TestAllocator&, const TestAllocator&) noexcept {
    return false;
  }

  ~TestAllocator() { assert(diff_ == 0); }
};
}  // namespace tree_check
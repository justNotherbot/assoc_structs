#pragma once

#include <iterator>
#include <type_traits>
#include <utility>

namespace my_stl {

template<typename Iterator>
class ReverseIterator final {
public:
  using iterator_category = std::iterator_traits<Iterator>::iterator_category;
  using value_type = typename std::iterator_traits<Iterator>::value_type;
  using difference_type = 
    typename std::iterator_traits<Iterator>::difference_type;
  using pointer = typename std::iterator_traits<Iterator>::pointer;
  using reference = typename std::iterator_traits<Iterator>::reference;

  explicit ReverseIterator(Iterator iter) 
    noexcept(std::is_nothrow_copy_constructible<Iterator>::value) : 
    iter_{--iter} {}

  ReverseIterator(const ReverseIterator& other) 
    noexcept(std::is_nothrow_copy_constructible<Iterator>::value) : 
    iter_{other.iter_} {}

  ReverseIterator(ReverseIterator&& other) noexcept {
    using std::swap;
    swap(iter_, other.iter_);
  }

  ReverseIterator& operator=(const ReverseIterator& other) 
    noexcept(std::is_nothrow_copy_constructible<Iterator>::value) {
    if(this == &other) {
      return *this;
    }

    Iterator tmp = other.iter_;
    using std::swap;
    swap(iter_, tmp);
    return *this;
  }

  ReverseIterator& operator=(ReverseIterator&& other) noexcept {
    if(this == &other) {
      return *this;
    }
    
    using std::swap;
    swap(iter_, other.iter_);
    return *this;
  }

  friend void swap(ReverseIterator& it_a, ReverseIterator& it_b) {
    using std::swap;
    swap(it_a.iter_, it_b.iter_);
  }

  bool operator==(const ReverseIterator& other) const noexcept {
    return iter_ == other.iter_;
  }

  bool operator!=(const ReverseIterator& other) const noexcept {
    return iter_ != other.iter_;
  }

  pointer operator->() {
    return iter_.operator->();
  }

  reference operator*() {
    return *iter_;
  }

  ReverseIterator& operator++() {
    --iter_;
    return *this;
  }

  ReverseIterator& operator--() {
    ++iter_;
    return *this;
  }

  ReverseIterator operator++(int) {
    ReverseIterator tmp{*this};
    --(*this);
    return tmp;
  }

  ReverseIterator operator--(int) {
    ReverseIterator tmp{*this};
    ++(*this);
    return tmp;
  }

  ~ReverseIterator() = default;

private:
  Iterator iter_;
};
} // namespace my_stl

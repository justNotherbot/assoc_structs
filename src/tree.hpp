#pragma once

#include <cstddef>

#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>

#include "common_operations.hpp"
#include "reverse_iterator.hpp"
#include "tree_common.hpp"
#include "tree_wrapper.hpp"

namespace trees {

template<requirements::TreeType ImplType>
class Tree final {
public:
  using key_type = typename ImplType::key_type;
  using value_type = typename ImplType::value_type;
  using node_type = typename ImplType::node_type;
  using compare_type = typename ImplType::compare_type;
  using allocator_type = typename ImplType::actual_allocator_type;
private:
  tree_wrapper_t<ImplType> tree_;
public:
  template<typename Node, typename Value>
  class Iterator final {
    Node* ptr_ = nullptr;
  public:
    using value_type = Value;
    using reference = std::add_lvalue_reference_t<value_type>;
    using difference_type = std::ptrdiff_t;
    using pointer = std::add_pointer_t<value_type>;
    using iterator_category = std::bidirectional_iterator_tag;

    friend void swap(Iterator& it_a, Iterator& it_b) noexcept {
      std::swap(it_a.ptr_, it_b.ptr_);
    }

    friend bool operator==(const Iterator& it_a, const Iterator& it_b) noexcept {
      return it_a.ptr_ == it_b.ptr_;
    }

    friend bool operator!=(const Iterator& it_a, const Iterator& it_b) noexcept {
      return !(it_a == it_b);
    }

    Iterator() = default;

    explicit Iterator(Node* ptr) : ptr_{ptr} {}

    reference operator*() {
      return ptr_->value;
    }

    Iterator& operator++() noexcept {
      ptr_ = FindNextNode(ptr_);
      return *this;
    }

    Iterator operator++(int) noexcept {
      Iterator tmp{*this};
      ++(*this);
      return tmp;
    }

    Iterator& operator--() noexcept {
      ptr_ = FindNextNode(ptr_, true);
      return *this;
    }

    Iterator operator--(int) noexcept {
      Iterator tmp{*this};
      --(*this);
      return tmp;
    }
  };

  using iterator = Iterator<node_type, value_type>;
  using const_iterator = Iterator<const node_type, const value_type>;
  using reverse_iterator = my_stl::ReverseIterator<iterator>;
  using const_reverse_iterator = my_stl::ReverseIterator<const_iterator>;
  using size_type = std::size_t;

  friend void swap(Tree& t_a, Tree& t_b) noexcept {
    swap(t_a.tree_, t_b.tree_);
  }

  allocator_type get_allocator() const noexcept {
    return tree_.allocator;
  }

  size_type max_size() const noexcept {
    return 
      std::allocator_traits<allocator_type>::max_size(tree_.allocator) / 
        sizeof(node_type);
  }

  Tree() = default;

  template<typename InIter, typename Sentinel>
  Tree(InIter lt, Sentinel&& rt, 
    const compare_type& comparator=compare_type{}) {
    tree_.comparator = comparator;
    insert(lt, rt);
  }

  Tree(const std::initializer_list<value_type>& il, 
    const compare_type& comparator=compare_type{}) {
    tree_.comparator = comparator;
    insert(il.begin(), il.end());
  }

  Tree(const Tree& other) {
    if(std::addressof(other) == this) {
      return;
    }
    Tree tmp{other.begin(), other.end()};
    swap(tmp, *this);
  }

  Tree(Tree&& other) {
    if(std::addressof(other) == this) {
      return;
    }
    swap(*this, other);
  }

  Tree& operator=(const Tree& other) {
    if(std::addressof(other) == this) {
      return *this;
    }
    Tree tmp{other};
    swap(tmp, *this);
    return *this;
  }

  iterator begin() noexcept {
    return iterator{tree_.begin};
  }

  const_iterator begin() const noexcept {
    return const_iterator{tree_.begin};
  }

  const_iterator cbegin() const noexcept {
    return const_iterator{tree_.begin};
  }

  iterator end() noexcept {
    return iterator{};
  }

  const_iterator end() const noexcept {
    return const_iterator{};
  }

  const_iterator cend() const noexcept {
    return const_iterator{};
  }

  reverse_iterator rbegin() noexcept {
    return reverse_iterator{iterator{tree_.rbegin}};
  }

  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator{const_iterator{tree_.rbegin}};
  }

  const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator{const_iterator{tree_.rbegin}};
  }

  reverse_iterator rend() noexcept {
    return reverse_iterator{iterator{}};
  }

  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator{const_iterator{}};
  }

  const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator{const_iterator{}};
  }

  size_type size() const noexcept {
    return tree_.size;
  }

  bool empty() const noexcept {
    return tree_.size == 0U;
  }

  iterator find(const key_type& key) noexcept {
    return iterator{Find(tree_, key)};
  }

  const_iterator find(const key_type& key) const noexcept {
    return const_iterator{Find(tree_, key)};
  }

  bool contains(const key_type& key) const noexcept {
    return find(key) != end();
  }

  size_type count(const key_type& key) const noexcept {
    return static_cast<size_type>(contains(key));
  }

  template<typename ... Args>
  std::pair<iterator, bool> emplace(Args&&... args) {
    auto [node_ptr, is_inserted] = Insert(tree_, 
      std::forward<Args>(args)...);
    return std::pair<iterator, bool>{iterator{node_ptr}, is_inserted};
  }

  template<typename InValue>
  std::pair<iterator, bool> insert(InValue&& value) {
    return emplace(std::forward<InValue>(value));
  }

  template<typename InIterator, typename Sentinel>
  void insert(InIterator i, Sentinel j) {
    while (i != j) {
      Insert(tree_, *i);
      ++i;
    }
  }

  void erase(const key_type& key) noexcept {
    node_type* node = Find(tree_, key);
    Erase(tree_, node);
  }

  void erase(iterator it) noexcept {
    Erase(tree_, it.ptr_);
  }

  template<typename InIterator, typename Sentinel>
  void erase(InIterator i, Sentinel j) noexcept {
    while (i != j) {
      node_type* node = Find(tree_, *i);
      Erase(tree_, node);
      ++i;
    }
  }

  iterator upper_bound(const key_type& key) noexcept {
    return FindBound(tree_, key, false);
  }

  const_iterator upper_bound(const key_type& key) const noexcept {
    return FindBound(tree_, key, false);
  }

  iterator lower_bound(const key_type& key) noexcept {
    return FindBound(tree_, key, true);
  }

  const_iterator lower_bound(const key_type& key) const noexcept {
    return FindBound(tree_, key, true);
  }

  std::pair<iterator, iterator> equal_range(const key_type& key) noexcept {
    return std::make_pair(lower_bound(key), upper_bound(key));
  }

  std::pair<const_iterator, const_iterator> 
    equal_range(const key_type& key) const noexcept {
    return std::make_pair(lower_bound(key), upper_bound(key));
  }

  void clear() noexcept {
    erase(begin(), end());
  }

  friend bool operator==(const Tree& tr_a, const Tree& tr_b) noexcept {
    if(tr_a.size() != tr_b.size()) {
      return false;
    }
    auto b_iter = tr_b.begin();
    for(const auto& el: tr_a) {
      if(*b_iter != el) {
        return false;
      }
      ++b_iter;
    }
    return true;
  }

  friend bool operator!=(const Tree& tr_a, const Tree& tr_b) noexcept {
    return !(tr_a == tr_b);
  }

  ~Tree() {
    Destroy(tree_);
  }
};
} // namespace trees

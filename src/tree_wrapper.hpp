#pragma once

#include <cstddef>

#include <type_traits>
#include <utility>

#include "tree_common.hpp"

namespace trees {

template<requirements::TreeType Tree>
struct tree_wrapper_t : public Tree {
  using node_type = typename Tree::node_type;
  std::size_t size = 0;
  node_type* begin = nullptr;

  friend void swap(
    tree_wrapper_t& w_a, tree_wrapper_t& w_b) noexcept {
    std::swap(w_a.size, w_b.size);
    std::swap(w_a.begin, w_b.begin);
    std::swap(w_a.root, w_b.root);
    using pos_type = typename Tree::allocator_type::propagate_on_swap;
    if constexpr (
      std::is_same_v<pos_type, std::true_type>) {
      using std::swap;
      swap(w_a.allocator, w_b.allocator);
    }
  }
};
} // namespace trees

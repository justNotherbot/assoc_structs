#pragma once

#include <cstddef>

#include "tree_common.hpp"

namespace trees {

template<requirements::TreeType Tree>
struct tree_wrapper_t : Tree {
  using node_type = typename Tree::node_type;
  std::size_t size = 0;
  node_type* begin = nullptr;
};
} // namespace trees

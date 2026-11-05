#pragma once

#include <cstdint>
#include <ctime>

#include <algorithm>
#include <functional>
#include <random>
#include <vector>

#include <common_operations.hpp>
#include <avl/avl.hpp>

namespace tree_check {
namespace avl {

template<trees::requirements::TreeType Tree>
class Verificator {
public:
  using test_type = Tree;
  using test_node_type = typename test_type::node_type;
private:
  static std::int64_t GetAndVerifyHeight(const test_node_type* node) noexcept {
    if(node == nullptr) {
      return 0;
    }
    std::int64_t h_l = GetAndVerifyHeight(node->left);
    std::int64_t h_r = GetAndVerifyHeight(node->right);
    assert(h_l - h_r == static_cast<std::int64_t>(node->diff));
    assert(std::abs(node->diff) < 2);
    return std::max(h_l, h_r) + 1;
  }
public:
  void operator() (const test_type& tree) const {
    if(tree.root == nullptr) {
      return;
    }
    GetAndVerifyHeight(tree.root);
  }
};
} // namespace avl
} // namespace tree_check
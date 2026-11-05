#pragma once

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <functional>
#include <random>
#include <utility>
#include <vector>

#include <common_operations.hpp>
#include <redblack/rb_tree.hpp>

namespace tree_check {

namespace redblack {
template<trees::requirements::TreeType Tree>
class Verificator {
public:
  using test_type = Tree;
  using test_node_type = typename test_type::node_type;
private:
  static std::size_t GetBlackHeight(const test_node_type* node) noexcept {
    assert(node != nullptr);
    std::size_t cnt_l = 0;
    std::size_t cnt_r = 0;
    if(node->left != nullptr) {
      assert(!node->is_red || !node->left->is_red);
      cnt_l = GetBlackHeight(node->left);
    }
    if(node->right != nullptr) {
      assert(!node->is_red || !node->right->is_red);
      cnt_r = GetBlackHeight(node->right);
    }
    assert(cnt_l == cnt_r || node->left == nullptr || node->right == nullptr);
    return std::max(cnt_l, cnt_r) + !node->is_red;
  }
public:
  void operator() (const test_type& tree) const {
    if(tree.root == nullptr) {
      return;
    }
    GetBlackHeight(tree.root);
  }
};
} // namespace redblack
} // namespace tree_check

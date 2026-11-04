#pragma once

#include <utility>

#include "tree_common.hpp"

namespace tree_check {

template<trees::requirements::TreeType Tree>
class BstVerificator final {
public:
  using test_type = Tree;
  using test_key_type = typename test_type::key_type;
  using test_value_type = typename test_type::value_type;
  using test_projector_type = typename test_type::projection_type;
  using test_node_type = typename test_type::node_type;
private:
  static std::pair<test_key_type, test_key_type> MergeSegments(
    const std::pair<test_key_type, test_key_type>& p1, 
    const std::pair<test_key_type, test_key_type>& p2) {
    assert(p1.first <= p1.second && p2.first <= p2.second);
    assert(p1.second < p2.first);
    return {p1.first, p2.second};
  }

  static std::pair<test_key_type, test_key_type> GetRange(
    const test_node_type* node, const test_projector_type& projector) noexcept {
    assert(node != nullptr);
    std::pair<test_key_type, test_key_type> res{
      projector(node->value), 
      projector(node->value)};
    if(node->left != nullptr) {
      assert(node->left != node);
      auto segl = GetRange(node->left, projector);
      res = MergeSegments(segl, res);
    }
    if(node->right != nullptr) {
      assert(node->right != node);
      auto segr = GetRange(node->right, projector);
      res = MergeSegments(res, segr);
    }
    return res;
  }

  static void CheckParents(
    const test_node_type* node, 
    const test_node_type* root) noexcept {
    if(node == nullptr) {
      return;
    }
    if(node == root) {
      assert(node->parent == nullptr);
    } else {
      assert(node->parent != nullptr && 
        (node == node->parent->left || 
        node == node->parent->right) && 
        node->parent->left != node->parent->right);
    }
    CheckParents(node->left, root);
    CheckParents(node->right, root);
  }
public:
  void operator() (const test_type& tree) const {
    if(tree.root == nullptr) {
      return;
    }
    GetRange(tree.root, tree.projector);
    CheckParents(tree.root, tree.root);
  }
};
} // namespace tree_check

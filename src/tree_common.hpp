#pragma once

#include <cassert>

#include <concepts>
#include <type_traits>

namespace tree_impl {

template<typename Node>
Node* Node::* RetrieveLinkFromNode(bool is_left) {
  if(is_left) {
    return &Node::left;
  }
  return &Node::right;
}

template <typename Node>
void RotateImpl(Node* node, Node*& root, bool left) {
  auto left_ptr = RetrieveLinkFromNode<Node>(!left);
  auto right_ptr = RetrieveLinkFromNode<Node>(left);
  assert(node != nullptr && node->parent != nullptr && 
    node == node->parent->*left_ptr);
  Node* node_a = node;
  Node* node_b = node->parent;
  Node* node_c = node->parent->parent;
  Node* node_f = node_a->*right_ptr;
  node_b->parent = node_a;
  node_a->parent = node_c;
  node_a->*right_ptr = node_b;
  node_b->*left_ptr = node_f;
  if(node_f != nullptr) {
    node_f->parent = node_b;
  }
  if(node_c != nullptr) {
    if(node_c->right == node_b) {
      node_c->right = node_a;
    } else {
      node_c->left = node_a;
    }
  } else {
    root = node_a;
  }
}

template<typename Node, typename Key, typename Compare, typename Project>
Node* GetClosestNode(Node* node, const Key& key, 
  const Compare& compare, const Project& project) noexcept {
  if(node == nullptr) {
    return nullptr;
  }
  if(key == project(node->value)) {
    return node;
  }
  Node* res = nullptr;
  if(compare(project(node->value), key)) {
    res = GetClosestNode(node->right, key, compare, project);
  } else {
    res = GetClosestNode(node->left, key, compare, project);
  }
  if(res == nullptr) {
    return node;
  }
  return res;
}

template<typename Node>
void DestroyNode(Node* node) noexcept {
  if(node == nullptr) {
    return;
  }
  DestroyNode(node->left);
  DestroyNode(node->right);
  delete node;
}
} // namespace tree_impl

namespace trees {

namespace requirements {

template<typename T>
concept TreeType = requires(T a) {
  typename T::key_type;
  typename T::node_type;
  typename T::compare_type;
  typename T::projection_type;

  {a.root} -> std::convertible_to<typename T::node_type*>;
  {a.comparator} -> std::convertible_to<typename T::compare_type>;
  {a.projector} -> std::convertible_to<typename T::projection_type>;
};
} // namespace requirements

template<requirements::TreeType Tree>
using node_type_t = typename Tree::node_type;

template<requirements::TreeType Tree>
void Destroy(Tree& tree) {
  tree_impl::DestroyNode(tree.root);
  tree.root = nullptr;
}

template<typename T>
class Identity {
public:
  T operator()(const T& val) const noexcept {
    return val;
  }
};

template<typename T, typename U>
class PairFirst {
public:
  T operator()(const std::pair<T, U>& val) const noexcept {
    return val.first;
  }
};
} // namespace trees 

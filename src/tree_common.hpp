#pragma once

#include <cassert>

#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>

namespace trees {

struct redblack_tag {};

struct avl_tag {};

namespace requirements {

template<typename T>
concept TreeType = requires(T a) {
  typename T::key_type;
  typename T::node_type;
  typename T::compare_type;
  typename T::projection_type;
  typename T::allocator_type;
  typename T::actual_allocator_type;

  {a.root} -> std::convertible_to<typename T::node_type*>;
  {a.comparator} -> std::convertible_to<typename T::compare_type>;
  {a.projector} -> std::convertible_to<typename T::projection_type>;
  {a.allocator} -> std::convertible_to<typename T::actual_allocator_type>;
};

template<typename T>
concept Allocator = requires(T a,
  typename std::allocator_traits<T>::pointer p_a, std::size_t cnt) {
  {a.allocate(cnt)} -> std::convertible_to<
    typename std::allocator_traits<T>::pointer>;
  {a.deallocate(p_a, cnt)};
} && std::copy_constructible<T>
  && std::equality_comparable<T>;
} // namespace requirements
} // namespace trees

namespace tree_impl {

template<typename Node>
Node* Node::* RetrieveLinkFromNode(bool is_left) noexcept {
  if(is_left) {
    return &Node::left;
  }
  return &Node::right;
}

template <typename Node>
void RotateImpl(Node* node, Node*& root, bool left) noexcept {
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

template<typename Node, ::trees::requirements::Allocator Allocate>
void DestroyNode(Node* node, Allocate& allocator) noexcept {
  if(node == nullptr) {
    return;
  }
  DestroyNode(node->left, allocator);
  DestroyNode(node->right, allocator);
  std::allocator_traits<Allocate>::destroy(allocator, node);
  allocator.deallocate(node, 1);
}

template <typename Node, ::trees::requirements::Allocator Allocator>
void DeleteNode(Node* node, Allocator& alloc) noexcept {
  std::allocator_traits<Allocator>::destroy(alloc, node);
  alloc.deallocate(node, 1);
}

template <typename Node>
void FixAfterSwap(Node* node_b, Node* node_a) noexcept {
  assert(node_a != node_b);
  if(node_b->parent == node_b) {
    node_b->parent = node_a;
  }
  if(node_b->left == node_b) {
    node_b->left = node_a;
  }
  if(node_b->right == node_b) {
    node_b->right = node_a;
  }
}

template <typename Node>
void PropagateAddress(Node* node, bool insert_right) noexcept {
  assert(node->right != node && node->left != node && node->parent != node);
  if(node->left != nullptr) {
    node->left->parent = node;
  }
  if(node->right != nullptr) {
    node->right->parent = node;
  }
  if(node->parent != nullptr) {
    if(insert_right) {
      node->parent->right = node;
    } else {
      node->parent->left = node;
    }
  }
  assert(node->right != node && node->left != node && node->parent != node);
}

template<typename Node>
void SwapNodes(Node* node_a, Node* node_b) noexcept {
  assert(node_a != node_b && node_a != nullptr && node_b != nullptr);
  bool insert_right_a = true;
  if(node_a->parent != nullptr && 
    node_a->parent->left == node_a) {
    insert_right_a = false;
  }
  bool insert_right_b = true;
  if(node_b->parent != nullptr && 
    node_b->parent->left == node_b) {
    insert_right_b = false;
  }
  std::swap(node_a->left, node_b->left);
  std::swap(node_a->right, node_b->right);
  std::swap(node_a->parent, node_b->parent);
  tree_impl::FixAfterSwap(node_a, node_b);
  tree_impl::FixAfterSwap(node_b, node_a);

  tree_impl::PropagateAddress(node_a, insert_right_b);
  tree_impl::PropagateAddress(node_b, insert_right_a);
}
} // namespace tree_impl

namespace trees {

template<requirements::TreeType Tree>
using node_type_t = typename Tree::node_type;

template<requirements::TreeType Tree>
void Destroy(Tree& tree) {
  tree_impl::DestroyNode(tree.root, tree.allocator);
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

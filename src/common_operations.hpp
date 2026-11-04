#pragma once

#include "avl/avl.hpp"
#include "redblack/rb_tree.hpp"
#include "tree_wrapper.hpp"

namespace trees {

template<requirements::TreeType Tree, typename Key>
typename Tree::node_type* Find(Tree& tr, Key&& key) {
  using node_type = typename Tree::node_type;
  node_type* node = tree_impl::GetClosestNode(tr.root, key, tr.comparator, tr.projector);
  if(node == nullptr) {
    return nullptr;
  }
  if(tr.projector(node->value) == key) {
    return node;
  }
  return nullptr;
}

template<requirements::TreeType Tree, typename Key>
const typename Tree::node_type* Find(const Tree& tr, Key&& key) {
  using node_type = typename Tree::node_type;
  const node_type* node = tree_impl::GetClosestNode(
    tr.root, key, tr.comparator, tr.projector);
  if(node == nullptr) {
    return nullptr;
  }
  if(tr.projector(node->value) == key) {
    return node;
  }
  return nullptr;
}

template<requirements::TreeType Tree, typename Key>
typename Tree::node_type* FindBound(Tree& tr, Key&& key, 
  bool is_lower) {
  return tree_impl::GetBound(tr.root, key, tr.comparator, 
    tr.projector, is_lower);
}

template<requirements::TreeType Tree, typename Key>
const typename Tree::node_type* FindBound(const Tree& tr, Key&& key, 
  bool is_lower) {
  return tree_impl::GetBound(tr.root, key, tr.comparator, 
    tr.projector, is_lower);
}

template<typename Node>
Node* FindNextNode(Node* node) {
  if(node == nullptr) {
    return nullptr;
  }
  if(node->right == nullptr) {
    Node* prev = node;
    node = node->parent;
    while (node != nullptr && node->right == prev) {
      prev = node;
      node = node->parent;
    }
  } else {
    node = node->right;
    while (node->left != nullptr) {
      node = node->left;
    }
  }
  return node;
}

template <requirements::TreeType Tree>
auto Insert(tree_wrapper_t<Tree>& tr, 
  const typename Tree::value_type& value) {
  using key_type = typename Tree::key_type;
  using value_type = typename Tree::value_type;
  using compare_type = typename Tree::compare_type;
  using project_type = typename Tree::projection_type;
  using allocator_type = typename Tree::allocator_type;
  using actual_allocator_type = typename Tree::actual_allocator_type;
  auto res = Insert<key_type, value_type, compare_type, project_type,
    allocator_type, actual_allocator_type>(tr, 
    value, tr.allocator);
  if(res.second) {
    if(tr.begin == nullptr) {
      tr.begin = res.first;
    }
    while (tr.begin->left != nullptr) {
      tr.begin = tr.begin->left;
    }
    
    ++tr.size;
  }
  
  return res;
}

template <requirements::TreeType Tree>
void Erase(tree_wrapper_t<Tree>& tr, const typename Tree::key_type& key) {
  using key_type = typename Tree::key_type;
  using value_type = typename Tree::value_type;
  using compare_type = typename Tree::compare_type;
  using project_type = typename Tree::projection_type;
  using allocator_type = typename Tree::allocator_type;
  using actual_allocator_type = typename Tree::actual_allocator_type;
  using node_type = typename Tree::node_type;
  if(tr.size == 0) {
    return;
  }
  node_type* node = Find(tr, key);
  if(node == nullptr) {
    return;
  }
  if(node == tr.begin) {
    tr.begin = FindNextNode(tr.begin);
  }
  Erase<key_type, value_type, compare_type, project_type,
    allocator_type, actual_allocator_type>(
    tr, node, tr.allocator);
  --tr.size;
}
} // namespace trees

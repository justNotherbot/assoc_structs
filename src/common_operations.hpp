#pragma once

#include <utility>

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
Node* FindNextNode(Node* node, bool find_prev=false) {
  if(node == nullptr) {
    return nullptr;
  }
  auto left_ptr = tree_impl::RetrieveLinkFromNode<Node>(
    !find_prev);
  auto right_ptr = tree_impl::RetrieveLinkFromNode<Node>(
    find_prev);
  if(node->*right_ptr == nullptr) {
    Node* prev = node;
    node = node->parent;
    while (node != nullptr && node->*right_ptr == prev) {
      prev = node;
      node = node->parent;
    }
  } else {
    node = node->*right_ptr;
    while (node->*left_ptr != nullptr) {
      node = node->*left_ptr;
    }
  }
  return node;
}

template <requirements::TreeType Tree, typename ... Args>
auto Insert(tree_wrapper_t<Tree>& tr, 
  Args&&... args) {
  using key_type = typename Tree::key_type;
  using value_type = typename Tree::value_type;
  using compare_type = typename Tree::compare_type;
  using project_type = typename Tree::projection_type;
  using allocator_type = typename Tree::allocator_type;
  using actual_allocator_type = typename Tree::actual_allocator_type;
  auto res = tree_impl::Insert<key_type, value_type, compare_type, project_type,
    allocator_type, actual_allocator_type, Args...>(
      tr, tr.allocator, std::forward<Args>(args)...);
  if(res.second) {
    if(tr.begin == nullptr) {
      tr.begin = res.first;
      tr.rbegin = res.first;
    }
    while (tr.begin->left != nullptr) {
      tr.begin = tr.begin->left;
    }
    while(tr.rbegin->right != nullptr) {
      tr.rbegin = tr.rbegin->right;
    }
    
    ++tr.size;
  }
  
  return res;
}

template <requirements::TreeType Tree>
void Erase(tree_wrapper_t<Tree>& tr, typename Tree::node_type* node) {
  using key_type = typename Tree::key_type;
  using value_type = typename Tree::value_type;
  using compare_type = typename Tree::compare_type;
  using project_type = typename Tree::projection_type;
  using allocator_type = typename Tree::allocator_type;
  using actual_allocator_type = typename Tree::actual_allocator_type;
  if(tr.size == 0) {
    return;
  }
  if(node == nullptr) {
    return;
  }
  if(node == tr.begin) {
    tr.begin = FindNextNode(tr.begin);
  }
  if(node == tr.rbegin) {
    tr.rbegin = FindNextNode(tr.rbegin, true);
  }
  tree_impl::Erase<key_type, value_type, compare_type, project_type,
    allocator_type, actual_allocator_type>(
    tr, node, tr.allocator);
  --tr.size;
}
} // namespace trees

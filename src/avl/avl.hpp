#pragma once

#include <cassert>
#include <cstdlib>

#include <utility>

#include <tree_common.hpp>

namespace trees {
namespace avl {

using diff_type = char;

template<typename Key>
struct avl_node_t {
  avl_node_t* left = nullptr;
  avl_node_t* right = nullptr;
  avl_node_t* parent = nullptr;
  diff_type diff = 0;
  Key key;
};

template<typename Key, typename Compare>
struct avl_tree_t {
  using key_type = Key;
  using node_type = avl_node_t<Key>;
  using compare_type = Compare;

  node_type* root = nullptr;
  compare_type comparator;
};
} // namespace avl
} // namespace trees

namespace tree_impl {
namespace avl {

namespace avl_pub = trees::avl;

template<typename Key, typename Compare>
void FixDifferences(avl_pub::avl_node_t<Key>* node, 
  avl_pub::avl_node_t<Key>* prev, avl_pub::avl_tree_t<Key, Compare>& tr, 
  avl_pub::diff_type diff_rt, bool is_erase) noexcept {
  while (node != nullptr) {
    if(prev == node->right) {
      node->diff += diff_rt;
    } else {
      node->diff -= diff_rt;
    }
    if(node->diff == 2 || node->diff == -2)  {
      bool is_complement = node->diff == 2;
      using node_type = avl_pub::avl_node_t<Key>;
      auto left_ptr = tree_impl::RetrieveLinkFromNode<node_type>(!is_complement);
      auto right_ptr = tree_impl::RetrieveLinkFromNode<node_type>(is_complement);
      avl_pub::diff_type coeff = is_complement ? -1 : 1;
      if((node->*right_ptr)->diff * coeff == 0) {
        avl_pub::avl_node_t<Key>* pivot = node->*right_ptr;
        tree_impl::RotateImpl(pivot, tr.root, !is_complement);
        pivot->diff = coeff;
        node->diff = -coeff;
        node = pivot;
      } else if((node->*right_ptr)->diff * coeff == -1) {
        avl_pub::avl_node_t<Key>* pivot = node->*right_ptr;
        tree_impl::RotateImpl(pivot, tr.root, !is_complement);
        pivot->diff = 0;
        node->diff = 0;
        node = pivot;
      } else {
        avl_pub::avl_node_t<Key>* node_a = node;
        avl_pub::avl_node_t<Key>* node_b = node->*right_ptr;
        avl_pub::avl_node_t<Key>* pivot = node_b->*left_ptr;
        tree_impl::RotateImpl(pivot, tr.root, is_complement);
        tree_impl::RotateImpl(pivot, tr.root, !is_complement);
        if(pivot->diff * coeff == 0) {
          node_a->diff = 0;
          node_b->diff = 0;
        } else if(pivot->diff * coeff == -1) {
          node_a->diff = coeff;
          node_b->diff = 0;
        } else {
          node_a->diff = 0;
          node_b->diff = -coeff;
        }
        pivot->diff = 0;
        node = pivot;
      }
    }
    if((node->diff == 0) == (!is_erase)) {
      return;
    }
    
    prev = node;
    node = node->parent;
  }
}
}
}

namespace trees {

template<typename Key, typename Compare>
std::pair<avl::avl_node_t<Key>*, bool> Insert(
  avl::avl_tree_t<Key, Compare>& tr, const Key& key) noexcept {
  avl::avl_node_t<Key>* closest = tree_impl::GetClosestNode(
    tr.root, key, tr.comparator);
  if(closest != nullptr && closest->key == key) {
    return {closest, false};
  }
  avl::avl_node_t<Key>* new_node = new avl::avl_node_t<Key>;
  new_node->key = key;
  new_node->parent = closest;
  if(closest == nullptr) {
    tr.root = new_node;
    return {new_node, true};
  }
  if(tr.comparator(closest->key, key)) {
    assert(closest->right == nullptr);
    closest->right = new_node;
  } else {
    assert(closest->left == nullptr);
    closest->left = new_node;
  }
  tree_impl::avl::FixDifferences(new_node->parent, new_node, tr, -1, false);
  return {new_node, true};
}

template<typename Key, typename Compare>
void Erase(
  avl::avl_tree_t<Key, Compare>& tr, avl::avl_node_t<Key>* node) noexcept {
  if(node == nullptr) {
    return;
  }
  if(node->left != nullptr && node->right != nullptr) {
    avl::avl_node_t<Key>* curr = node->right;
    avl::avl_node_t<Key>* prev = curr;
    while (curr != nullptr) {
      prev = curr;
      curr = curr->left;
    }
    node->key = prev->key;
    node = prev;
  }
  assert(node->left == nullptr || node->right == nullptr);
  node->diff = 0;
  if(node->left != nullptr) {
    avl::avl_node_t<Key>* pivot = node->left;
    tree_impl::RotateImpl(pivot, tr.root, false);
    pivot->diff = -1;
  } else if(node->right != nullptr) {
    avl::avl_node_t<Key>* pivot = node->right;
    tree_impl::RotateImpl(pivot, tr.root, true);
    pivot->diff = 1;
  }
  assert(node->left == nullptr && node->right == nullptr);
  tree_impl::avl::FixDifferences(node->parent, node, tr, 1, true);
  if(node->parent == nullptr) {
    tr.root = nullptr;
  } else {
    if(node == node->parent->left) {
      node->parent->left = nullptr;
    } else {
      node->parent->right = nullptr;
    }
  }
  delete node;
}
} // namespace trees
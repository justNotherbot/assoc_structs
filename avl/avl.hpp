#pragma once

#include <cassert>
#include <cstdlib>

#include <iostream>

namespace my_tree {

using diff_type = char;

template<typename Key>
struct avl_node_t {
  avl_node_t* left = nullptr;
  avl_node_t* right = nullptr;
  avl_node_t* parent = nullptr;
  diff_type diff = 0;
  Key key;
};

template<typename Key>
struct avl_tree_t {
  avl_node_t<Key>* root = nullptr;
};
} // namespace my_tree

namespace my_tree_impl {

template<typename Key>
my_tree::avl_node_t<Key>* my_tree::avl_node_t<Key>::* GetAvlLink(bool left) noexcept {
  if(left) {
    return &my_tree::avl_node_t<Key>::left;
  }
  return &my_tree::avl_node_t<Key>::right;
}

template<typename Key>
void DestroyNode(my_tree::avl_node_t<Key>* node) noexcept {
  if(node == nullptr) {
    return;
  }
  DestroyNode(node->left);
  DestroyNode(node->right);
  delete node;
}

template<typename Key>
my_tree::avl_node_t<Key>* GetClosestNode(
  my_tree::avl_node_t<Key>* node, const Key& key) noexcept {
  if(node == nullptr) {
    return nullptr;
  }
  if(key == node->key) {
    return node;
  }
  my_tree::avl_node_t<Key>* res = nullptr;
  if(key > node->key) {
    res = GetClosestNode(node->right, key);
  } else {
    res = GetClosestNode(node->left, key);
  }
  if(res == nullptr) {
    return node;
  }
  return res;
}

template<typename Key>
void Rotate(my_tree::avl_node_t<Key>* node, my_tree::avl_tree_t<Key>& tr, 
  bool left) noexcept {
  auto left_ptr = GetAvlLink<Key>(!left);
  auto right_ptr = GetAvlLink<Key>(left);
  assert(node != nullptr && node->parent != nullptr && 
    node == node->parent->*left_ptr);
  my_tree::avl_node_t<Key>* node_a = node;
  my_tree::avl_node_t<Key>* node_b = node->parent;
  my_tree::avl_node_t<Key>* node_c = node->parent->parent;
  my_tree::avl_node_t<Key>* node_f = node_a->*right_ptr;
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
    tr.root = node_a;
  }
}

template<typename Key>
void FixDifferences(my_tree::avl_node_t<Key>* node, my_tree::avl_node_t<Key>* prev, 
  my_tree::avl_tree_t<Key>& tr, my_tree::diff_type diff_rt, bool is_erase) noexcept {
  while (node != nullptr) {
    if(prev == node->right) {
      node->diff += diff_rt;
    } else {
      node->diff -= diff_rt;
    }
    if(node->diff == 2 || node->diff == -2)  {
      bool is_complement = node->diff == 2;
      auto left_ptr = GetAvlLink<Key>(!is_complement);
      auto right_ptr = GetAvlLink<Key>(is_complement);
      my_tree::diff_type coeff = is_complement ? -1 : 1;
      if((node->*right_ptr)->diff * coeff == 0) {
        my_tree::avl_node_t<Key>* pivot = node->*right_ptr;
        Rotate(pivot, tr, !is_complement);
        pivot->diff = coeff;
        node->diff = -coeff;
        node = pivot;
      } else if((node->*right_ptr)->diff * coeff == -1) {
        my_tree::avl_node_t<Key>* pivot = node->*right_ptr;
        Rotate(pivot, tr, !is_complement);
        pivot->diff = 0;
        node->diff = 0;
        node = pivot;
      } else {
        my_tree::avl_node_t<Key>* node_a = node;
        my_tree::avl_node_t<Key>* node_b = node->*right_ptr;
        my_tree::avl_node_t<Key>* pivot = node_b->*left_ptr;
        Rotate(pivot, tr, is_complement);
        Rotate(pivot, tr, !is_complement);
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
} // namespace my_tree_impl

namespace my_tree {

template<typename Key>
void Destroy(avl_tree_t<Key>& tr) noexcept {
  my_tree_impl::DestroyNode(tr.root);
  tr.root = nullptr;
}

template<typename Key>
std::pair<avl_node_t<Key>*, bool> Insert(avl_tree_t<Key>& tr, const Key& key) noexcept {
  avl_node_t<Key>* closest = my_tree_impl::GetClosestNode(tr.root, key);
  if(closest != nullptr && closest->key == key) {
    return {closest, false};
  }
  avl_node_t<Key>* new_node = new avl_node_t<Key>;
  new_node->key = key;
  new_node->parent = closest;
  if(closest == nullptr) {
    tr.root = new_node;
    return {new_node, true};
  }
  if(key > closest->key) {
    assert(closest->right == nullptr);
    closest->right = new_node;
  } else {
    assert(closest->left == nullptr);
    closest->left = new_node;
  }
  my_tree_impl::FixDifferences(new_node->parent, new_node, tr, -1, false);
  return {new_node, true};
}

template<typename Key>
void Erase(avl_tree_t<Key>& tr, const Key& key) noexcept {
  avl_node_t<Key>* node = my_tree_impl::GetClosestNode(tr.root, key);
  if(node == nullptr) {
    return;
  }
  if(node->key != key) {
    return;
  }
  if(node->left != nullptr && node->right != nullptr) {
    avl_node_t<Key>* curr = node->right;
    avl_node_t<Key>* prev = curr;
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
    avl_node_t<Key>* pivot = node->left;
    my_tree_impl::Rotate(pivot, tr, false);
    pivot->diff = -1;
  } else if(node->right != nullptr) {
    avl_node_t<Key>* pivot = node->right;
    my_tree_impl::Rotate(pivot, tr, true);
    pivot->diff = 1;
  }
  assert(node->left == nullptr && node->right == nullptr);
  my_tree_impl::FixDifferences(node->parent, node, tr, 1, true);
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
} // namespace my_tree

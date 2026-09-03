#pragma once

#include <cassert>

#include "tree_common.hpp"

template <typename Key>
struct rb_node_t {
  bool is_red = false;
  Key key;
  rb_node_t* parent = nullptr;
  rb_node_t* left = nullptr;
  rb_node_t* right = nullptr;
};

template <typename Key>
struct rb_tree_t {
  rb_node_t<Key>* root = nullptr;
};

template <typename Key>
void DestroyImpl(rb_node_t<Key>* node) noexcept {
  if (node == nullptr) {
    return;
  }
  DestroyImpl(node->left);
  DestroyImpl(node->right);
  delete node;
}

template <typename Key>
void Destroy(rb_tree_t<Key>& tr) noexcept {
  DestroyImpl(tr.root);
  tr.root = nullptr;
}

template <typename Key>
rb_node_t<Key>* GetClosestImpl(rb_node_t<Key>* node, const Key& k) {
  assert(node != nullptr);
  if (k == node->key) {
    return node;
  }
  if (k > node->key) {
    if (node->right == nullptr) {
      return node;
    }
    return GetClosestImpl(node->right, k);
  }
  if (node->left == nullptr) {
    return node;
  }
  return GetClosestImpl(node->left, k);
}

template <typename Key>
rb_node_t<Key>* GetClosest(rb_tree_t<Key> tr, const Key& k) {
  return GetClosestImpl(tr.root, k);
}

template <typename Key>
void ColorFixup(rb_node_t<Key>* node, rb_tree_t<Key>& tr) {
  if (!node->is_red) {
    return;
  }
  if (node->parent == nullptr) {
    node->is_red = false;
    return;
  }
  if (!node->parent->is_red) {
    return;
  }
  if (node->parent->parent == nullptr) {
    node->parent->is_red = false;
    return;
  }

  rb_node_t<Key>* np = node->parent;
  rb_node_t<Key>* npp = node->parent->parent;

  bool is_complement = np != npp->left;
  rb_node_t<Key>* rb_node_t<Key>::* left_ptr = 
    my_tree_common::RetrieveLinkFromNode<rb_node_t<Key>>(!is_complement);
  rb_node_t<Key>* rb_node_t<Key>::* right_ptr = 
    my_tree_common::RetrieveLinkFromNode<rb_node_t<Key>>(is_complement);

  if (!CheckBlack(npp->*right_ptr)) {
    npp->is_red = true;
    np->is_red = false;
    (npp->*right_ptr)->is_red = false;
    ColorFixup(npp, tr);
  } else {
    rb_node_t<Key>* pivot = node;
    if (node == node->parent->*left_ptr) {
      pivot = node->parent;
    } else {
      my_tree_common::RotateImpl(node, tr.root, !is_complement);
    }
    my_tree_common::RotateImpl(pivot, tr.root, is_complement);
    (pivot->*right_ptr)->is_red = true;
    pivot->is_red = false;
  }
}

template <typename Key>
bool CheckBlack(rb_node_t<Key>* node) noexcept {
  if (node == nullptr) {
    return true;
  }
  return !node->is_red;
}

template <typename Key>
std::pair<rb_node_t<Key>*, bool> Insert(rb_tree_t<Key>& tr, const Key& k) {
  rb_node_t<Key>* closest = nullptr;
  if (tr.root != nullptr) {
    closest = GetClosest(tr, k);
    if (closest->key == k) {
      return {closest, false};
    }
  }

  rb_node_t<Key>* new_node = new rb_node_t<Key>;
  new_node->key = k;
  if (tr.root == nullptr) {
    tr.root = new_node;
    return {new_node, true};
  }
  new_node->is_red = true;
  new_node->parent = closest;

  if (k > closest->key) {
    assert(closest->right == nullptr);
    closest->right = new_node;
  } else {
    assert(closest->left == nullptr);
    closest->left = new_node;
  }
  ColorFixup(new_node, tr);
  return {new_node, true};
}

template <typename Key>
rb_node_t<Key>* FindImpl(rb_node_t<Key>* curr, const Key& key) {
  if (curr == nullptr || curr->key == key) {
    return curr;
  }
  rb_node_t<Key>* prev = nullptr;
  if (curr->key > key) {
    return FindImpl(curr->left, key);
  }
  return FindImpl(curr->right, key);
}

template <typename Key>
rb_node_t<Key>* Find(rb_tree_t<Key>& tr, const Key& key) {
  return FindImpl(tr.root, key);
}

template <typename Key>
void BlackDelete(rb_tree_t<Key>& tr, rb_node_t<Key>* node) {
  assert(!node->is_red);
  rb_node_t<Key>* np = node->parent;
  if (np == nullptr) {
    return;
  }
  bool is_complement = np->left == node;
  rb_node_t<Key>* rb_node_t<Key>::* left_ptr = 
    my_tree_common::RetrieveLinkFromNode<rb_node_t<Key>>(!is_complement);
  rb_node_t<Key>* rb_node_t<Key>::* right_ptr =
      my_tree_common::RetrieveLinkFromNode<rb_node_t<Key>>(is_complement);

  rb_node_t<Key>* npl = np->*left_ptr;
  if (np->is_red) {
    if (!CheckBlack(npl->*left_ptr)) {
      my_tree_common::RotateImpl(npl, tr.root, is_complement);
      npl->is_red = true;
      (npl->*left_ptr)->is_red = false;
      (npl->*right_ptr)->is_red = false;
      return;
    } else if (!CheckBlack(npl->*right_ptr)) {
      rb_node_t<Key>* pivot = npl->*right_ptr;
      my_tree_common::RotateImpl(pivot, tr.root, !is_complement);
      my_tree_common::RotateImpl(pivot, tr.root, is_complement);
      pivot->is_red = true;
      (pivot->*left_ptr)->is_red = false;
      (pivot->*right_ptr)->is_red = false;
      return;
    } else {
      np->is_red = false;
      npl->is_red = true;
      return;
    }
  } else {
    if (npl->is_red) {
      rb_node_t<Key>* node_c = npl->*right_ptr;
      if (CheckBlack(node_c->*left_ptr) && !CheckBlack(node_c->*right_ptr)) {
        rb_node_t<Key>* node_d = node_c->*right_ptr;
        my_tree_common::RotateImpl(node_d, tr.root, !is_complement);
        (node_d->*left_ptr)->is_red = true;
        node_d->is_red = false;
        node_c = node_d;
      }
      if (!CheckBlack(node_c->*left_ptr)) {
        my_tree_common::RotateImpl(node_c, tr.root, !is_complement);
        my_tree_common::RotateImpl(node_c, tr.root, is_complement);
        node_c->is_red = false;
        (node_c->*right_ptr)->is_red = false;
        (node_c->*left_ptr)->is_red = true;
        ((node_c->*left_ptr)->*right_ptr)->is_red = false;
        return;
      } else {
        my_tree_common::RotateImpl(npl, tr.root, is_complement);
        node_c->is_red = true;
        npl->is_red = false;
        np->is_red = false;
        return;
      }
    } else {
      if (!CheckBlack(npl->*left_ptr)) {
        my_tree_common::RotateImpl(npl, tr.root, is_complement);
        (npl->*left_ptr)->is_red = false;
        return;
      } else if (!CheckBlack(npl->*right_ptr)) {
        rb_node_t<Key>* pivot = npl->*right_ptr;
        my_tree_common::RotateImpl(pivot, tr.root, !is_complement);
        my_tree_common::RotateImpl(pivot, tr.root, is_complement);
        pivot->is_red = false;
        return;
      } else {
        npl->is_red = true;
        BlackDelete(tr, np);
      }
    }
  }
}

template<typename Key>
void CutNodeAway(rb_tree_t<Key>& tr, rb_node_t<Key>* node) {
  if (node->parent == nullptr) {
    tr.root = nullptr;
    delete node;
  } else {
    if (node == node->parent->right) {
      node->parent->right = nullptr;
    } else {
      node->parent->left = nullptr;
    }
    delete node;
  }
}

template <typename Key>
void Erase(rb_tree_t<Key>& tr, const Key& key) {
  rb_node_t<Key>* node = Find(tr, key);
  if (node == nullptr) {
    return;
  }
  if (node->left != nullptr && node->right != nullptr) {
    rb_node_t<Key>* curr = node->right;
    rb_node_t<Key>* prev = curr;
    while (curr != nullptr) {
      prev = curr;
      curr = curr->left;
    }
    node->key = prev->key;
    node = prev;
  }
  if (node->left != nullptr) {
    assert(!node->is_red);
    node->key = node->left->key;
    delete node->left;
    node->left = nullptr;
    return;
  }
  if (node->right != nullptr) {
    assert(!node->is_red);
    node->key = node->right->key;
    delete node->right;
    node->right = nullptr;
    return;
  }
  
  if(!node->is_red) {
    BlackDelete(tr, node);
  }
  
  CutNodeAway(tr, node);
}

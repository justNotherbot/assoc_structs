#pragma once

#include <cassert>
#include <utility>

#include <tree_common.hpp>

namespace trees {
namespace redblack {

template <typename Key>
struct rb_node_t {
  bool is_red = false;
  Key key;
  rb_node_t* parent = nullptr;
  rb_node_t* left = nullptr;
  rb_node_t* right = nullptr;
};

template <typename Key, typename Compare>
struct rb_tree_t {
  using key_type = Key;
  using node_type = rb_node_t<Key>;
  using compare_type = Compare;

  struct tag_t {};

  node_type* root = nullptr;
  compare_type comparator;
};
} // namespace redblack
} // namespace trees

namespace tree_impl {
namespace redblack {

namespace rb_pub = trees::redblack;

template <typename Key>
bool CheckBlack(rb_pub::rb_node_t<Key>* node) noexcept {
  if (node == nullptr) {
    return true;
  }
  return !node->is_red;
}

template <typename Key, typename Compare>
void ColorFixup(rb_pub::rb_node_t<Key>* node, 
  rb_pub::rb_tree_t<Key, Compare>& tr) {
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

  rb_pub::rb_node_t<Key>* np = node->parent;
  rb_pub::rb_node_t<Key>* npp = node->parent->parent;

  bool is_complement = np != npp->left;
  rb_pub::rb_node_t<Key>* rb_pub::rb_node_t<Key>::* left_ptr = 
    tree_impl::RetrieveLinkFromNode<rb_pub::rb_node_t<Key>>(!is_complement);
  rb_pub::rb_node_t<Key>* rb_pub::rb_node_t<Key>::* right_ptr = 
    tree_impl::RetrieveLinkFromNode<rb_pub::rb_node_t<Key>>(is_complement);

  if (!CheckBlack(npp->*right_ptr)) {
    npp->is_red = true;
    np->is_red = false;
    (npp->*right_ptr)->is_red = false;
    ColorFixup(npp, tr);
  } else {
    rb_pub::rb_node_t<Key>* pivot = node;
    if (node == node->parent->*left_ptr) {
      pivot = node->parent;
    } else {
      tree_impl::RotateImpl(node, tr.root, !is_complement);
    }
    tree_impl::RotateImpl(pivot, tr.root, is_complement);
    (pivot->*right_ptr)->is_red = true;
    pivot->is_red = false;
  }
}

template <typename Key, typename Compare>
rb_pub::rb_node_t<Key>* Find(rb_pub::rb_node_t<Key>* curr, const Key& key,
  const Compare& compare) {
  if (curr == nullptr || curr->key == key) {
    return curr;
  }
  rb_pub::rb_node_t<Key>* prev = nullptr;
  if (compare(key, curr->key)) {
    return Find(curr->left, key, compare);
  }
  return Find(curr->right, key, compare);
}

template <typename Key, typename Compare>
void BlackDelete(rb_pub::rb_tree_t<Key, Compare>& tr, 
  rb_pub::rb_node_t<Key>* node) {
  assert(!node->is_red);
  rb_pub::rb_node_t<Key>* np = node->parent;
  if (np == nullptr) {
    return;
  }
  bool is_complement = np->left == node;
  rb_pub::rb_node_t<Key>* rb_pub::rb_node_t<Key>::* left_ptr = 
    tree_impl::RetrieveLinkFromNode<rb_pub::rb_node_t<Key>>(!is_complement);
  rb_pub::rb_node_t<Key>* rb_pub::rb_node_t<Key>::* right_ptr =
      tree_impl::RetrieveLinkFromNode<rb_pub::rb_node_t<Key>>(is_complement);

  rb_pub::rb_node_t<Key>* npl = np->*left_ptr;
  if (np->is_red) {
    if (!CheckBlack(npl->*left_ptr)) {
      tree_impl::RotateImpl(npl, tr.root, is_complement);
      npl->is_red = true;
      (npl->*left_ptr)->is_red = false;
      (npl->*right_ptr)->is_red = false;
      return;
    } else if (!CheckBlack(npl->*right_ptr)) {
      rb_pub::rb_node_t<Key>* pivot = npl->*right_ptr;
      tree_impl::RotateImpl(pivot, tr.root, !is_complement);
      tree_impl::RotateImpl(pivot, tr.root, is_complement);
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
      rb_pub::rb_node_t<Key>* node_c = npl->*right_ptr;
      if (CheckBlack(node_c->*left_ptr) && !CheckBlack(node_c->*right_ptr)) {
        rb_pub::rb_node_t<Key>* node_d = node_c->*right_ptr;
        tree_impl::RotateImpl(node_d, tr.root, !is_complement);
        (node_d->*left_ptr)->is_red = true;
        node_d->is_red = false;
        node_c = node_d;
      }
      if (!CheckBlack(node_c->*left_ptr)) {
        tree_impl::RotateImpl(node_c, tr.root, !is_complement);
        tree_impl::RotateImpl(node_c, tr.root, is_complement);
        node_c->is_red = false;
        (node_c->*right_ptr)->is_red = false;
        (node_c->*left_ptr)->is_red = true;
        ((node_c->*left_ptr)->*right_ptr)->is_red = false;
        return;
      } else {
        tree_impl::RotateImpl(npl, tr.root, is_complement);
        node_c->is_red = true;
        npl->is_red = false;
        np->is_red = false;
        return;
      }
    } else {
      if (!CheckBlack(npl->*left_ptr)) {
        tree_impl::RotateImpl(npl, tr.root, is_complement);
        (npl->*left_ptr)->is_red = false;
        return;
      } else if (!CheckBlack(npl->*right_ptr)) {
        rb_pub::rb_node_t<Key>* pivot = npl->*right_ptr;
        tree_impl::RotateImpl(pivot, tr.root, !is_complement);
        tree_impl::RotateImpl(pivot, tr.root, is_complement);
        pivot->is_red = false;
        return;
      } else {
        npl->is_red = true;
        BlackDelete(tr, np);
      }
    }
  }
}

template<typename Key, typename Compare>
void CutNodeAway(rb_pub::rb_tree_t<Key, Compare>& tr, 
  rb_pub::rb_node_t<Key>* node) {
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
} // namespace redblack
} //namespace tree_impl

namespace trees {

template <typename Key, typename Compare>
std::pair<redblack::rb_node_t<Key>*, bool> Insert(
  redblack::rb_tree_t<Key, Compare>& tr, const Key& k) {
  redblack::rb_node_t<Key>* closest = nullptr;
  if (tr.root != nullptr) {
    closest = tree_impl::GetClosestNode(tr.root, k, tr.comparator);
    if (closest->key == k) {
      return {closest, false};
    }
  }

  redblack::rb_node_t<Key>* new_node = new redblack::rb_node_t<Key>;
  new_node->key = k;
  if (tr.root == nullptr) {
    tr.root = new_node;
    return {new_node, true};
  }
  new_node->is_red = true;
  new_node->parent = closest;

  if (tr.comparator(closest->key, k)) {
    assert(closest->right == nullptr);
    closest->right = new_node;
  } else {
    assert(closest->left == nullptr);
    closest->left = new_node;
  }
  tree_impl::redblack::ColorFixup(new_node, tr);
  return {new_node, true};
}

template <typename Key, typename Compare>
void Erase(redblack::rb_tree_t<Key, Compare>& tr, 
  redblack::rb_node_t<Key>* node) {
  if (node == nullptr) {
    return;
  }
  if (node->left != nullptr && node->right != nullptr) {
    redblack::rb_node_t<Key>* curr = node->right;
    redblack::rb_node_t<Key>* prev = curr;
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
    tree_impl::redblack::BlackDelete(tr, node);
  }
  
  tree_impl::redblack::CutNodeAway(tr, node);
}
} // namespace trees
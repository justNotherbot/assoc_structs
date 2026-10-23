#pragma once

#include <cassert>

#include <concepts>
#include <utility>

#include <tree_common.hpp>

namespace trees {
namespace redblack {

namespace requirements {

//template<typename Tree>
//concept RedBlack = requires
} // namespace requirements

template <typename Value>
struct rb_node_t {
  using value_type = Value;

  bool is_red = false;
  Value value;
  rb_node_t* parent = nullptr;
  rb_node_t* left = nullptr;
  rb_node_t* right = nullptr;
};

template <typename Key, typename Value, typename Compare, typename Project>
struct rb_tree_t {
  using key_type = Key;
  using value_type = Value;
  using node_type = rb_node_t<value_type>;
  using compare_type = Compare;
  using projection_type = Project;
  using tag_type = typename trees::redblack_tag;

  node_type* root = nullptr;
  compare_type comparator;
  projection_type projector; // value_type -> key_type
};
} // namespace redblack
} // namespace trees

namespace tree_impl {
namespace redblack {

namespace rb_pub = trees::redblack;

template <typename Value>
bool CheckBlack(rb_pub::rb_node_t<Value>* node) noexcept {
  if (node == nullptr) {
    return true;
  }
  return !node->is_red;
}

template <typename Key, typename Value, typename Compare, typename Project>
void ColorFixup(
  trees::node_type_t<rb_pub::rb_tree_t<Key, Value, Compare, Project>>* node, 
  rb_pub::rb_tree_t<Key, Value, Compare, Project>& tr) {
  using node_type = trees::node_type_t<rb_pub::rb_tree_t<Key, Value, Compare, Project>>;
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

  node_type* np = node->parent;
  node_type* npp = node->parent->parent;

  bool is_complement = np != npp->left;
  node_type* node_type::* left_ptr = 
    tree_impl::RetrieveLinkFromNode<node_type>(!is_complement);
  node_type* node_type::* right_ptr = 
    tree_impl::RetrieveLinkFromNode<node_type>(is_complement);

  if (!CheckBlack(npp->*right_ptr)) {
    npp->is_red = true;
    np->is_red = false;
    (npp->*right_ptr)->is_red = false;
    ColorFixup(npp, tr);
  } else {
    node_type* pivot = node;
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

template <typename Key, typename Value, typename Compare, typename Project>
void BlackDelete(rb_pub::rb_tree_t<Key, Value, Compare, Project>& tr, 
  trees::node_type_t<rb_pub::rb_tree_t<Key, Value, Compare, Project>>* node) {
  using node_type = trees::node_type_t<rb_pub::rb_tree_t<Key, Value, Compare, Project>>;
  assert(!node->is_red);
  node_type* np = node->parent;
  if (np == nullptr) {
    return;
  }
  bool is_complement = np->left == node;
  node_type* node_type::* left_ptr = 
    tree_impl::RetrieveLinkFromNode<node_type>(!is_complement);
  node_type* node_type::* right_ptr =
      tree_impl::RetrieveLinkFromNode<node_type>(is_complement);

  node_type* npl = np->*left_ptr;
  if (np->is_red) {
    if (!CheckBlack(npl->*left_ptr)) {
      tree_impl::RotateImpl(npl, tr.root, is_complement);
      npl->is_red = true;
      (npl->*left_ptr)->is_red = false;
      (npl->*right_ptr)->is_red = false;
      return;
    } else if (!CheckBlack(npl->*right_ptr)) {
      node_type* pivot = npl->*right_ptr;
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
      node_type* node_c = npl->*right_ptr;
      if (CheckBlack(node_c->*left_ptr) && !CheckBlack(node_c->*right_ptr)) {
        node_type* node_d = node_c->*right_ptr;
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
        node_type* pivot = npl->*right_ptr;
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

template<typename Key, typename Value, typename Compare, typename Project>
void CutNodeAway(rb_pub::rb_tree_t<Key, Value, Compare, Project>& tr, 
  trees::node_type_t<rb_pub::rb_tree_t<Key, Value, Compare, Project>>* node) {
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

template <typename Key, typename Value, typename Compare, typename Project>
std::pair<
  trees::node_type_t<redblack::rb_tree_t<Key, Value, Compare, Project>>*, bool> 
  Insert(
  redblack::rb_tree_t<Key, Value, Compare, Project>& tr, const Value& val) {
  using node_type = 
    trees::node_type_t<redblack::rb_tree_t<Key, Value, Compare, Project>>;
  node_type* closest = nullptr;
  if (tr.root != nullptr) {
    closest = tree_impl::GetClosestNode(tr.root, tr.projector(val), 
      tr.comparator, tr.projector);
    if (tr.projector(closest->value) == tr.projector(val)) {
      return {closest, false};
    }
  }

  node_type* new_node = new node_type;
  new_node->value = val;
  if (tr.root == nullptr) {
    tr.root = new_node;
    return {new_node, true};
  }
  new_node->is_red = true;
  new_node->parent = closest;

  if (tr.comparator(tr.projector(closest->value), tr.projector(val))) {
    assert(closest->right == nullptr);
    closest->right = new_node;
  } else {
    assert(closest->left == nullptr);
    closest->left = new_node;
  }
  tree_impl::redblack::ColorFixup(new_node, tr);
  return {new_node, true};
}

template <typename Key, typename Value, typename Compare, typename Project>
void Erase(redblack::rb_tree_t<Key, Value, Compare, Project>& tr, 
  trees::node_type_t<redblack::rb_tree_t<Key, Value, Compare, Project>>* node) {
  using node_type = 
    trees::node_type_t<redblack::rb_tree_t<Key, Value, Compare, Project>>;
  if (node == nullptr) {
    return;
  }
  if (node->left != nullptr && node->right != nullptr) {
    node_type* curr = node->right;
    node_type* prev = curr;
    while (curr != nullptr) {
      prev = curr;
      curr = curr->left;
    }
    node->value = prev->value;
    node = prev;
  }
  if (node->left != nullptr) {
    assert(!node->is_red);
    node->value = node->left->value;
    delete node->left;
    node->left = nullptr;
    return;
  }
  if (node->right != nullptr) {
    assert(!node->is_red);
    node->value = node->right->value;
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
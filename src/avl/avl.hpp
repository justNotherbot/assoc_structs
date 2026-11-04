#pragma once

#include <cassert>
#include <cstdlib>

#include <memory>
#include <utility>

#include <tree_common.hpp>

namespace trees {
namespace avl {

using diff_type = char;

template<typename Value>
struct avl_node_t {
  avl_node_t* left = nullptr;
  avl_node_t* right = nullptr;
  avl_node_t* parent = nullptr;
  diff_type diff = 0;
  Value value;

  friend void swap(avl_node_t& node_a, avl_node_t& node_b) noexcept {
    tree_impl::SwapNodes(std::addressof(node_a), std::addressof(node_b));
    std::swap(node_a.diff, node_b.diff);
  }
};

template<typename Key, typename Value, typename Compare, typename Project,
  ::trees::requirements::Allocator Allocate>
struct avl_tree_t {
  using key_type = Key;
  using value_type = Value;
  using node_type = avl_node_t<value_type>;
  using compare_type = Compare;
  using projection_type = Project;
  using allocator_type = Allocate;
  using actual_allocator_type = 
    typename std::allocator_traits<Allocate>::rebind_alloc<node_type>;
  using tag_type = typename trees::avl_tag;

  node_type* root = nullptr;
  compare_type comparator;
  projection_type projector;
  mutable actual_allocator_type allocator;
};
} // namespace avl
} // namespace trees

namespace tree_impl {
namespace avl {

namespace avl_pub = trees::avl;

template<typename Key, typename Value, typename Compare, typename Project,
  ::trees::requirements::Allocator Allocate>
void FixDifferences(
  trees::node_type_t<
    avl_pub::avl_tree_t<Key, Value, Compare, Project, Allocate>>* node, 
  trees::node_type_t<avl_pub::avl_tree_t<Key, Value, Compare, Project,
    Allocate>>* prev, 
  avl_pub::avl_tree_t<Key, Value, Compare, Project, Allocate>& tr, 
  avl_pub::diff_type diff_rt, bool is_erase) noexcept {
  using node_type = 
    trees::node_type_t<avl_pub::avl_tree_t<Key, Value, Compare, Project, 
      Allocate>>;
  while (node != nullptr) {
    if(prev == node->right) {
      node->diff += diff_rt;
    } else {
      node->diff -= diff_rt;
    }
    if(node->diff == 2 || node->diff == -2)  {
      bool is_complement = node->diff == 2;
      auto left_ptr = tree_impl::RetrieveLinkFromNode<node_type>(!is_complement);
      auto right_ptr = tree_impl::RetrieveLinkFromNode<node_type>(is_complement);
      avl_pub::diff_type coeff = is_complement ? -1 : 1;
      if((node->*right_ptr)->diff * coeff == 0) {
        node_type* pivot = node->*right_ptr;
        tree_impl::RotateImpl(pivot, tr.root, !is_complement);
        pivot->diff = coeff;
        node->diff = -coeff;
        node = pivot;
      } else if((node->*right_ptr)->diff * coeff == -1) {
        node_type* pivot = node->*right_ptr;
        tree_impl::RotateImpl(pivot, tr.root, !is_complement);
        pivot->diff = 0;
        node->diff = 0;
        node = pivot;
      } else {
        node_type* node_a = node;
        node_type* node_b = node->*right_ptr;
        node_type* pivot = node_b->*left_ptr;
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

template<typename Key, typename Value, typename Compare, typename Project,
  requirements::Allocator Allocate, requirements::Allocator AllocateUse>
std::pair<
  trees::node_type_t<avl::avl_tree_t<Key, Value, Compare, Project,
    Allocate>>*, bool> Insert(
  avl::avl_tree_t<Key, Value, Compare, Project, Allocate>& tr, 
  const Value& value, AllocateUse& allocator) noexcept {
  using node_type = 
    trees::node_type_t<avl::avl_tree_t<Key, Value, Compare, Project, 
    Allocate>>;
  node_type* closest = tree_impl::GetClosestNode(tr.root, tr.projector(value), 
    tr.comparator, tr.projector);
  if(closest != nullptr && tr.projector(closest->value) == tr.projector(value)) {
    return {closest, false};
  }
  void* new_memory = allocator.allocate(1);
  node_type* new_node = new (new_memory) node_type;
  new_node->value = value;
  new_node->parent = closest;
  if(closest == nullptr) {
    tr.root = new_node;
    return {new_node, true};
  }
  if(tr.comparator(tr.projector(closest->value), tr.projector(value))) {
    assert(closest->right == nullptr);
    closest->right = new_node;
  } else {
    assert(closest->left == nullptr);
    closest->left = new_node;
  }
  tree_impl::avl::FixDifferences(new_node->parent, new_node, tr, -1, false);
  return {new_node, true};
}

template<typename Key, typename Value, typename Compare, typename Project,
  ::trees::requirements::Allocator Allocate, 
  ::trees::requirements::Allocator AllocateUse>
void Erase(
  avl::avl_tree_t<Key, Value, Compare, Project, Allocate>& tr, 
  trees::node_type_t<avl::avl_tree_t<Key, Value, Compare, Project,
    Allocate>>* node,
  AllocateUse& allocator) noexcept {
  using node_type = 
    trees::node_type_t<avl::avl_tree_t<Key, Value, Compare, Project, 
      Allocate>>;
  if(node == nullptr) {
    return;
  }
  if(node->left != nullptr && node->right != nullptr) {
    node_type* curr = node->right;
    node_type* prev = curr;
    while (curr != nullptr) {
      prev = curr;
      curr = curr->left;
    }
    if(node == tr.root) {
      tr.root = prev;
    }
    swap(*node, *prev);
  }
  assert(node->left == nullptr || node->right == nullptr);
  node->diff = 0;
  if(node->left != nullptr) {
    node_type* pivot = node->left;
    tree_impl::RotateImpl(pivot, tr.root, false);
    pivot->diff = -1;
  } else if(node->right != nullptr) {
    node_type* pivot = node->right;
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
  ::tree_impl::DeleteNode(node, allocator);
}
} // namespace trees
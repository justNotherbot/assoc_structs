#pragma once

#include "avl/avl.hpp"
#include "redblack/rb_tree.hpp"

namespace trees {

template<requirements::TreeType Tree, typename Key>
typename Tree::node_type* Find(Tree& tr, Key&& key) {
  using node_type = typename Tree::node_type;
  node_type* node = tree_impl::GetClosestNode(tr.root, key, tr.comparator);
  if(node->key == key) {
    return node;
  }
  return nullptr;
}

template <requirements::TreeType Tree>
void Erase(Tree& tr, const typename Tree::key_type& key) {
  using key_type = typename Tree::key_type;
  using compare_type = typename Tree::compare_type;
  Erase<key_type, compare_type>(tr, Find(tr, key));
}
} // namespace trees

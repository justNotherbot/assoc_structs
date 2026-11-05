#pragma once

#include <type_traits>

#include <avl/avl.hpp>
#include <redblack/rb_tree.hpp>
#include "tree.hpp"

namespace my_stl {

struct rb_tree_tag_t {};

struct avl_tree_tag_t {};

template<typename Tag, typename Key, typename Value, 
  typename Compare, typename Project,
  ::trees::requirements::Allocator Allocate>
using tree_type_t = std::conditional_t<std::is_same_v<Tag, rb_tree_tag_t>,
  typename trees::redblack::rb_tree_t<Key, Value, Compare, Project, Allocate>,
  typename trees::avl::avl_tree_t<Key, Value, Compare, Project, Allocate>>;

template<typename Key, typename Compare=std::less<Key>, 
  typename Allocator=std::allocator<Key>, typename Tag=rb_tree_tag_t>
using Set = typename trees::Tree<tree_type_t<Tag, Key, Key, Compare, 
  trees::Identity<Key>, Allocator>>;
} // namespace my_stl

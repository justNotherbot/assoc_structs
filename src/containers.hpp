#pragma once

#include <type_traits>
#include <utility>

#include <avl/avl.hpp>
#include <redblack/rb_tree.hpp>
#include "tree.hpp"
#include "tree_common.hpp"

namespace my_stl {

template<typename Tag, trees::requirements::TreeKey Key, 
  typename Value, 
  typename Compare, typename Project,
  ::trees::requirements::Allocator Allocate>
using tree_type_t = std::conditional_t<std::is_same_v<Tag, trees::redblack_tag_t>,
  typename trees::redblack::rb_tree_t<Key, Value, Compare, Project, Allocate>,
  typename trees::avl::avl_tree_t<Key, Value, Compare, Project, Allocate>>;

template<trees::requirements::TreeKey Key, typename Compare=std::less<Key>, 
  typename Allocator=std::allocator<Key>, typename Tag=trees::redblack_tag_t>
using Set = typename trees::Tree<tree_type_t<Tag, Key, Key, Compare, 
  trees::Identity<Key>, Allocator>>;

template<trees::requirements::TreeKey Key, typename Value, 
  typename Compare=std::less<Key>, typename Allocator=std::allocator<Key>, 
  typename Tag=trees::redblack_tag_t>
using Map = typename trees::Tree<tree_type_t<Tag, Key, 
  std::pair<Key, Value>, Compare, 
  trees::PairFirst<Key, Value>, Allocator>>;
} // namespace my_stl

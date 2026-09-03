#pragma once

#include <cstddef>

namespace my_tree {

template<typename Key>
class RbTree final {
public:
  using size_type = std::size_t;

  struct rb_node_t {
    bool is_red = false;
    Key key;
    rb_node_t* parent = nullptr;
    rb_node_t* left = nullptr;
    rb_node_t* right = nullptr;
  };

private:
  rb_node_t* root_ = nullptr;
  size_type size_ = 0U;
public:
  RbTree() = default;


};
} // namespace my_tree

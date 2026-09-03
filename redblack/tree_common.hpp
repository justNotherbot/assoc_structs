#pragma once

#include <cassert>

namespace my_tree_common {

namespace requirements {

} // namespace requirements

template<typename Node>
Node* Node::* RetrieveLinkFromNode(bool is_left) {
  if(is_left) {
    return &Node::left;
  }
  return &Node::right;
}

template <typename Node>
void RotateImpl(Node* node, Node*& root, bool left) {
  auto left_ptr = RetrieveLinkFromNode<Node>(!left);
  auto right_ptr = RetrieveLinkFromNode<Node>(left);
  assert(node != nullptr && node->parent != nullptr && 
    node == node->parent->*left_ptr);
  Node* node_a = node;
  Node* node_b = node->parent;
  Node* node_c = node->parent->parent;
  Node* node_f = node_a->*right_ptr;
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
    root = node_a;
  }
}
} // namespace my_tree_common

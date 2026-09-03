#pragma once

#include <cstdint>
#include <ctime>

#include <algorithm>
#include <random>
#include <vector>

#include "avl.hpp"

namespace my_tree_impl {

template<typename Key>
std::int64_t GetAndVerifyHeight(const my_tree::avl_node_t<Key>* node) noexcept {
  if(node == nullptr) {
    return 0;
  }
  std::int64_t h_l = GetAndVerifyHeight(node->left);
  std::int64_t h_r = GetAndVerifyHeight(node->right);
  assert(h_l - h_r == static_cast<std::int64_t>(node->diff));
  assert(std::abs(node->diff) < 2);
  return std::max(h_l, h_r) + 1;
}

template<typename Key>
void VerifyParents(const my_tree::avl_node_t<Key>* node) noexcept {
  if(node == nullptr) {
    return;
  }
  assert(node->parent != nullptr &&
    (node == node->parent->left || node == node->parent->right) && 
    (node->parent->left != node->parent->right || node->parent->left == nullptr));
  VerifyParents(node->left);
  VerifyParents(node->right);
}

template<typename Key>
std::pair<Key, Key> MergeRanges(
  const std::pair<Key, Key>& p_l, const std::pair<Key, Key>& p_h) {
  assert(p_l.first <= p_l.second && p_h.first <= p_h.second && p_l.second < p_h.first);
  return std::pair{p_l.first, p_h.second};
}

template<typename Key>
std::pair<Key, Key> VerifyBst(const my_tree::avl_node_t<Key>* node) noexcept {
  assert(node != nullptr);
  std::pair<Key, Key> curr_range{node->key, node->key};
  if(node->left != nullptr) {
    std::pair<Key, Key> left_range = VerifyBst(node->left);
    curr_range = MergeRanges(left_range, curr_range);
  }
  if(node->right != nullptr) {
    std::pair<Key, Key> right_range = VerifyBst(node->right);
    curr_range = MergeRanges(curr_range, right_range);
  }
  return curr_range;
}
} // namespace my_tree_impl

namespace my_tree {

template<typename Key>
void Verify(const avl_tree_t<Key>& tr) noexcept {
  my_tree_impl::GetAndVerifyHeight(tr.root);
  if(tr.root == nullptr) {
    return;
  }
  my_tree_impl::VerifyParents(tr.root->left);
  my_tree_impl::VerifyParents(tr.root->right);
  assert(tr.root->parent == nullptr);
  my_tree_impl::VerifyBst(tr.root);
}

void InsertExample1() noexcept {
  my_tree::avl_tree_t<int> tr;
  Verify(tr);
  Insert(tr, 45);
  Verify(tr);
  Insert(tr, 1);
  Verify(tr);
  Insert(tr, 32);
  Verify(tr);
  Insert(tr, 2);
  Verify(tr);
  Insert(tr, 145);
  Verify(tr);
  Destroy(tr);
}

void VerifyInsertRandom() noexcept {
  const unsigned max_tests = 1000;
  unsigned curr_tests = 100 + std::max(1U, 
    static_cast<unsigned>(std::rand()) % max_tests);
  for(unsigned i = 0U; i < curr_tests; ++i) {
    unsigned curr_queries = 500 + (static_cast<unsigned>(std::rand()) % 200);
    my_tree::avl_tree_t<int> tr;
    for(unsigned j = 0U; j < curr_queries; ++j) {
      Verify(tr);
      int num = std::rand();
      Insert(tr, num);
      Verify(tr);
    }
    Destroy(tr);
  }
}

void EraseExample1() noexcept {
  my_tree::avl_tree_t<int> tr;
  Verify(tr);
  Insert(tr, 45);
  Verify(tr);
  Insert(tr, 1);
  Verify(tr);
  Insert(tr, 32);
  Verify(tr);
  Insert(tr, 2);
  Verify(tr);
  Insert(tr, 145);
  Verify(tr);
  Erase(tr, 32);
  Verify(tr);
  Erase(tr, 145);
  Verify(tr);
  Erase(tr, 2);
  Verify(tr);
  Destroy(tr);
}

void VerifyEraseRandom() noexcept {
  const unsigned max_tests = 1000;
  unsigned curr_tests = 100 + std::max(1U, 
    static_cast<unsigned>(std::rand()) % max_tests);
  std::random_device rd;
  std::mt19937 generator(rd());
  for(unsigned i = 0U; i < curr_tests; ++i) {
    unsigned curr_queries = 100 + (static_cast<unsigned>(std::rand()) % 100);
    my_tree::avl_tree_t<int> tr;
    std::vector<int> numbers;
    while (curr_queries) {
      int num = std::rand();
      Verify(tr);
      auto [node, was_inserted] = Insert(tr, num);
      Verify(tr);
      if(was_inserted) {
        numbers.push_back(num);
        curr_queries--;
      }
    }
    
    std::shuffle(numbers.begin(), numbers.end(), generator);
    for(std::size_t j = 0; j < numbers.size(); ++j) {
      Verify(tr);
      Erase(tr, numbers[j]);
      Verify(tr);
    }
    Destroy(tr);
  }
}

void VerifyAllCases() noexcept {
  std::srand(std::time(nullptr));
  EraseExample1();
  VerifyEraseRandom();
  InsertExample1();
  VerifyInsertRandom();
}
} // namespace my_tree

#pragma once

#include <cstdint>
#include <ctime>

#include <algorithm>
#include <functional>
#include <random>
#include <vector>

#include <common_operations.hpp>
#include "avl.hpp"

namespace tree_check {
namespace avl {

template<typename Key>
std::int64_t GetAndVerifyHeight(const trees::avl::avl_node_t<Key>* node) noexcept {
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
void VerifyParents(const trees::avl::avl_node_t<Key>* node) noexcept {
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
std::pair<Key, Key> VerifyBst(const trees::avl::avl_node_t<Key>* node) noexcept {
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


template<typename Key, typename Compare>
void Verify(const trees::avl::avl_tree_t<Key, Compare>& tr) noexcept {
  GetAndVerifyHeight(tr.root);
  if(tr.root == nullptr) {
    return;
  }
  VerifyParents(tr.root->left);
  VerifyParents(tr.root->right);
  assert(tr.root->parent == nullptr);
  VerifyBst(tr.root);
}

using test_key_type = int;
using test_compare = std::less<int>;
using test_tree_type = trees::avl::avl_tree_t<test_key_type, test_compare>;

void InsertExample1() noexcept {
  test_tree_type tr;
  Verify(tr);
  trees::Insert(tr, 45);
  Verify(tr);
  trees::Insert(tr, 1);
  Verify(tr);
  trees::Insert(tr, 32);
  Verify(tr);
  trees::Insert(tr, 2);
  Verify(tr);
  trees::Insert(tr, 145);
  Verify(tr);
  trees::Destroy(tr);
}

void VerifyInsertRandom() noexcept {
  const unsigned max_tests = 1000;
  unsigned curr_tests = 100 + std::max(1U, 
    static_cast<unsigned>(std::rand()) % max_tests);
  for(unsigned i = 0U; i < curr_tests; ++i) {
    unsigned curr_queries = 500 + (static_cast<unsigned>(std::rand()) % 200);
    test_tree_type tr;
    for(unsigned j = 0U; j < curr_queries; ++j) {
      Verify(tr);
      int num = std::rand();
      trees::Insert(tr, num);
      Verify(tr);
    }
    trees::Destroy(tr);
  }
}

void EraseExample1() noexcept {
  test_tree_type tr;
  Verify(tr);
  trees::Insert(tr, 45);
  Verify(tr);
  trees::Insert(tr, 1);
  Verify(tr);
  trees::Insert(tr, 32);
  Verify(tr);
  trees::Insert(tr, 2);
  Verify(tr);
  trees::Insert(tr, 145);
  Verify(tr);
  trees::Erase(tr, 32);
  Verify(tr);
  trees::Erase(tr, 145);
  Verify(tr);
  trees::Erase(tr, 2);
  Verify(tr);
  trees::Destroy(tr);
}

void VerifyEraseRandom() noexcept {
  const unsigned max_tests = 1000;
  unsigned curr_tests = 100 + std::max(1U, 
    static_cast<unsigned>(std::rand()) % max_tests);
  std::random_device rd;
  std::mt19937 generator(rd());
  for(unsigned i = 0U; i < curr_tests; ++i) {
    unsigned curr_queries = 100 + (static_cast<unsigned>(std::rand()) % 100);
    test_tree_type tr;
    std::vector<int> numbers;
    while (curr_queries) {
      int num = std::rand();
      Verify(tr);
      auto [node, was_inserted] = trees::Insert(tr, num);
      Verify(tr);
      if(was_inserted) {
        numbers.push_back(num);
        curr_queries--;
      }
    }
    
    std::shuffle(numbers.begin(), numbers.end(), generator);
    for(std::size_t j = 0; j < numbers.size(); ++j) {
      Verify(tr);
      trees::Erase(tr, numbers[j]);
      Verify(tr);
    }
    trees::Destroy(tr);
  }
}

void VerifyAllCases() noexcept {
  std::srand(std::time(nullptr));
  EraseExample1();
  VerifyEraseRandom();
  InsertExample1();
  VerifyInsertRandom();
}
} // namespace avl
} // namespace tree_check
#pragma once

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <random>
#include <utility>
#include <vector>

#include "rb_tree.hpp"

template<typename Key>
std::pair<Key, Key> MergeSegments(
  std::pair<Key, Key> p1, std::pair<Key, Key> p2) {
  assert(p1.first <= p1.second && p2.first <= p2.second);
  assert(p1.second < p2.first);
  return {p1.first, p2.second};
}

template<typename Key>
std::pair<Key, Key> GetRange(rb_node_t<Key>* node) noexcept {
  assert(node != nullptr);
  std::pair<Key, Key> res{node->key, node->key};
  if(node->left != nullptr) {
    assert(!node->is_red || !node->left->is_red);
    auto segl = GetRange(node->left);
    res = MergeSegments(segl, res);
  }
  if(node->right != nullptr) {
    assert(!node->is_red || !node->right->is_red);
    auto segr = GetRange(node->right);
    res = MergeSegments(res, segr);
  }
  return res;
}

template<typename Key>
std::size_t GetBlackHeight(rb_node_t<Key>* node) noexcept {
  assert(node != nullptr);
  std::size_t cnt_l = 0;
  std::size_t cnt_r = 0;
  if(node->left != nullptr) {
    cnt_l = GetBlackHeight(node->left);
  }
  if(node->right != nullptr) {
    cnt_r = GetBlackHeight(node->right);
  }
  assert(cnt_l == cnt_r || node->left == nullptr || node->right == nullptr);
  return std::max(cnt_l, cnt_r) + !node->is_red;
}

template<typename Key>
void CheckParents(rb_node_t<Key>* node, rb_node_t<Key>* root) noexcept {
  if(node == nullptr) {
    return;
  }
  if(node == root) {
    assert(node->parent == nullptr);
  } else {
    assert(node->parent != nullptr && (node == node->parent->left || 
      node == node->parent->right) && node->parent->left != node->parent->right);
  }
  CheckParents(node->left, root);
  CheckParents(node->right, root);
}

template<typename Key>
void Verify(const rb_tree_t<Key>& tr) noexcept {
  if(tr.root == nullptr) {
    return;
  }
  assert(!tr.root->is_red);
  GetRange(tr.root);
  GetBlackHeight(tr.root);
  CheckParents(tr.root, tr.root);
}

using test_type = int;

void VerifyExample1() noexcept {
  rb_tree_t<test_type> rb_tree;
  rb_node_t<test_type>* node = new rb_node_t<test_type>;
  node->key = 0;
  rb_tree.root = node;
  Verify(rb_tree);
  Destroy(rb_tree);
}

void VerifyExample2() noexcept {
  rb_tree_t<test_type> rb_tree;
  rb_node_t<test_type>* node1 = new rb_node_t<test_type>;
  rb_node_t<test_type>* node2 = new rb_node_t<test_type>;
  rb_node_t<test_type>* node3 = new rb_node_t<test_type>;
  node1->key = 1;
  node2->key = 2;
  node3->key = 3;
  node2->left = node1;
  node2->right = node3;
  node1->parent = node2;
  node3->parent = node2;
  rb_tree.root = node2;
  Verify(rb_tree);
  Destroy(rb_tree);
}

void VerifyExample3() noexcept {
  rb_tree_t<test_type> rb_tree;
  rb_node_t<test_type>* node1 = new rb_node_t<test_type>;
  rb_node_t<test_type>* node2 = new rb_node_t<test_type>;
  rb_node_t<test_type>* node5 = new rb_node_t<test_type>;
  rb_node_t<test_type>* node6 = new rb_node_t<test_type>;
  node1->key = 1;
  node2->key = 2;
  node5->key = 5;
  node6->key = 6;
  node1->is_red = true;
  node5->left = node1;
  node1->right = node2;
  node5->right = node6;
  node6->parent = node5;
  node2->parent = node1;
  node1->parent = node5;
  rb_tree.root = node5;
  Verify(rb_tree);
  Destroy(rb_tree);
}

void VerifyExamples() noexcept {
  VerifyExample1();
  VerifyExample2();
  VerifyExample3();
}

void VerifyInsert() noexcept {
  const unsigned max_tests = 100;
  unsigned curr_tests = 100 + std::max(1U, 
    static_cast<unsigned>(std::rand()) % max_tests);
  for(unsigned i = 0U; i < curr_tests; ++i) {
    unsigned curr_queries = 500 + (static_cast<unsigned>(std::rand()) % 20);
    rb_tree_t<int> tr;
    for(unsigned j = 0U; j < curr_queries; ++j) {
      Verify(tr);
      Insert(tr, rand());
      Verify(tr);
    }
    Destroy(tr);
  }
}

void EraseExample1() noexcept {
  rb_tree_t<int> tr;
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
    rb_tree_t<int> tr;
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

void VerifyErase() noexcept {
  EraseExample1();
  VerifyEraseRandom();
}

void VerifyAllCases() noexcept {
  std::srand(std::time(0));
  VerifyExamples();
  VerifyInsert();
  VerifyErase();
}

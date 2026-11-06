#pragma once

#include <ctime>
#include <cstdlib>

#include <algorithm>
#include <iostream>
#include <random>
#include <set>
#include <stdexcept>
#include <vector>

#include <tree.hpp>
#include <tree_common.hpp>
#include <tree_wrapper.hpp>

#include "bst_verificator.hpp"

namespace tree_check {

template<typename Key>
struct IntValuedPairGenerator {
  std::pair<Key, int> operator()(const Key& key) const noexcept {
    return {key, std::rand()};
  }
};

template<trees::requirements::TreeType Tree, typename ValueGenerator,
  typename Verificator>
class Tester final {
public:
  using tree_type = Tree;
  using test_type = trees::tree_wrapper_t<Tree>;
  using test_key_type = typename test_type::key_type;
  using test_value_type = typename test_type::value_type;
  using test_node_type = typename test_type::node_type;
  using test_compare_type = typename test_type::compare_type;
  using value_generator_type = ValueGenerator;
  using verificator_type = Verificator;
  using allocator_type = typename Tree::actual_allocator_type;
private:
  value_generator_type value_generator_;
  verificator_type verificator_;
  BstVerificator<test_type> bst_verificator_;

  void Verify(const test_type& tree) {
    bst_verificator_(tree);
    verificator_(tree);
  }

  test_node_type* ManualNodeCreate(allocator_type& alloc) {
    void* mem_ptr = alloc.allocate(1);
    test_node_type* node = new (mem_ptr) test_node_type;
    return node;
  }

  void VerifyExample1() noexcept {
    test_type tree;
    test_node_type* node = ManualNodeCreate(tree.allocator);
    node->value = value_generator_(0);
    tree.root = node;
    Verify(tree);
    trees::Destroy(tree);
  }

  void VerifyExample2() noexcept {
    test_type tree;
    test_node_type* node1 = ManualNodeCreate(tree.allocator);
    test_node_type* node2 = ManualNodeCreate(tree.allocator);
    test_node_type* node3 = ManualNodeCreate(tree.allocator);
    node1->value = value_generator_(1);
    node2->value = value_generator_(2);
    node3->value = value_generator_(3);
    node2->left = node1;
    node2->right = node3;
    node1->parent = node2;
    node3->parent = node2;
    tree.root = node2;
    Verify(tree);
    trees::Destroy(tree);
  }

  void InsertDuplicate() noexcept {
    test_type tr;
    test_value_type val = value_generator_(1);
    auto [node, ret] = trees::Insert(tr, val);
    assert(tr.size == 1 && ret);
    auto [node1, ret1] = trees::Insert(tr, val);
    assert(tr.size == 1 && !ret1);
    trees::Destroy(tr);
  }

  void EraseDuplicate() noexcept {
    test_type tr;
    test_value_type val1 = value_generator_(1);
    test_value_type val2 = value_generator_(2);
    auto [node, ret] = trees::Insert(tr, val1);
    assert(ret && tr.size == 1);
    auto [node1, ret1] = trees::Insert(tr, val2);
    assert(ret1 && tr.size == 2);
    trees::Erase(tr, trees::Find(tr, 1));
    assert(tr.size == 1);
    trees::Erase(tr, trees::Find(tr, 1));
    assert(tr.size == 1);
    trees::Erase(tr, trees::Find(tr, 2));
    assert(tr.size == 0);
    trees::Erase(tr, trees::Find(tr, 2));
    assert(tr.size == 0);
    trees::Destroy(tr);
  }

  void VerifyExamples() noexcept {
    VerifyExample1();
    VerifyExample2();
    InsertDuplicate();
  }

  void VerifyInsert() noexcept {
    const unsigned max_tests = 1000;
    unsigned curr_tests = 100 + std::max(1U, 
      static_cast<unsigned>(std::rand()) % max_tests);
    for(unsigned i = 0U; i < curr_tests; ++i) {
      unsigned curr_queries = 500 + (static_cast<unsigned>(std::rand()) % 20);
      test_type tr;
      std::set<test_key_type> inserted_keys;
      for(unsigned j = 0U; j < curr_queries; ++j) {
        Verify(tr);
        int rand_val = rand();
        inserted_keys.insert(rand_val);
        trees::Insert(tr, value_generator_(rand_val));
        assert(tr.begin != nullptr && 
          tr.projector(tr.begin->value) == *inserted_keys.begin()
          && inserted_keys.size() == tr.size);
        
        Verify(tr);
      }
      trees::Destroy(tr);
    }
  }

  void VerifyNextPrev() noexcept {
    const unsigned max_tests = 1000;
    unsigned curr_tests = 100 + std::max(1U, 
      static_cast<unsigned>(std::rand()) % max_tests);
    for(unsigned i = 0U; i < curr_tests; ++i) {
      unsigned curr_queries = 500 + (static_cast<unsigned>(std::rand()) % 20);
      test_type tr;
      std::set<test_key_type> inserted_keys;
      for(unsigned j = 0U; j < curr_queries; ++j) {
        int rand_val = rand();
        inserted_keys.insert(rand_val);
        trees::Insert(tr, value_generator_(rand_val));
      }
      test_node_type* curr_node = tr.begin;
      auto set_iter = inserted_keys.begin();
      assert(tr.size == inserted_keys.size());
      for(unsigned j = 0; j < tr.size; ++j) {
        assert(curr_node != nullptr && 
          tr.projector(curr_node->value) == *set_iter);
        ++set_iter;
        curr_node = trees::FindNextNode(curr_node);
      }
      auto iter_rbegin = inserted_keys.rbegin();
      curr_node = tr.rbegin;
      for(unsigned j = 0; j < tr.size; ++j) {
        assert(curr_node != nullptr && 
          tr.projector(curr_node->value) == *iter_rbegin);
        curr_node = trees::FindNextNode(curr_node, true);
        ++iter_rbegin;
      }
      trees::Destroy(tr);
    }
  }

  void EraseExample1() noexcept {
    test_type tr;
    Verify(tr);
    trees::Insert(tr, value_generator_(45));
    Verify(tr);
    trees::Insert(tr, value_generator_(1));
    Verify(tr);
    trees::Insert(tr, value_generator_(32));
    Verify(tr);
    trees::Insert(tr, value_generator_(2));
    Verify(tr);
    trees::Insert(tr, value_generator_(145));
    Verify(tr);
    trees::Erase(tr, trees::Find(tr, 32));
    Verify(tr);
    trees::Erase(tr, trees::Find(tr, 145));
    Verify(tr);
    trees::Erase(tr, trees::Find(tr, 2));
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
      test_type tr;
      std::vector<test_key_type> numbers;
      std::set<test_key_type> numbers_ord;
      while (curr_queries) {
        int num = std::rand();
        Verify(tr);
        numbers_ord.insert(num);
        auto [node, was_inserted] = trees::Insert(
          tr, value_generator_(num));
        assert(numbers_ord.size() == tr.size &&
          tr.projector(tr.begin->value) == *numbers_ord.begin());
        Verify(tr);
        if(was_inserted) {
          numbers.push_back(num);
          curr_queries--;
        }
      }
      
      std::shuffle(numbers.begin(), numbers.end(), generator);
      for(std::size_t j = 0; j < numbers.size(); ++j) {
        Verify(tr);
        numbers_ord.erase(numbers[j]);
        trees::Erase(tr, trees::Find(tr, numbers[j]));
        test_node_type* curr_node = trees::Find(tr, numbers[j]);
        assert(curr_node == nullptr);
        if(numbers_ord.size()) {
          assert(numbers_ord.size() == tr.size &&
            tr.projector(tr.begin->value) == *numbers_ord.begin());
        } else {
          assert(tr.begin == nullptr);
        }
        Verify(tr);
      }
      trees::Destroy(tr);
    }
  }

  void VerifyErase() noexcept {
    EraseExample1();
    EraseDuplicate();
    VerifyEraseRandom();
  }

  void BoundExample1() noexcept {
    test_type tr;
    std::vector<int> values = {1, 8, 4, 6};
    for(const auto& value: values) {
      trees::Insert(tr, value_generator_(value));
    }
    for(const auto& value: values) {
      test_node_type* node = trees::FindBound(tr, value, true);
      assert(tr.projector(node->value) == value);
      node = trees::FindBound(tr, value, false);
      if(value == 8) {
        assert(node == nullptr);
      } else if(value == 1) {
        assert(tr.projector(node->value) == 4);
      } else if(value == 4) {
        assert(tr.projector(node->value) == 6);
      } else if(value == 6) {
        assert(tr.projector(node->value) == 8);
      }
    }
    trees::Destroy(tr);
  }

  void BoundRandom() {
    unsigned cnt = 100U + ((static_cast<unsigned>(std::rand())) % 1000U);
    std::set<int> values;
    test_type tr;
    for(unsigned i = 0U; i < cnt; ++i) {
      int val = std::rand();
      values.insert(val);
      trees::Insert(tr, value_generator_(val));
      assert(tr.size == values.size());
    }
    for(const auto& key: values) {
      test_node_type* node_lo = trees::FindBound(tr, key, true);
      test_node_type* node_upp = trees::FindBound(tr, key, false);
      assert(node_lo != nullptr && tr.projector(node_lo->value) == key);
      auto act_upp = values.upper_bound(key);
      if(act_upp == values.end()) {
        assert(node_upp == nullptr);
      } else {
        assert(node_upp != nullptr && 
          tr.projector(node_upp->value) == *act_upp);
      }
    }
    for(unsigned i = 0U; i < cnt; ++i) {
      int val = std::rand();
      test_node_type* node_lo = trees::FindBound(tr, val, true);
      test_node_type* node_upp = trees::FindBound(tr, val, false);
      auto it_lo = values.lower_bound(val);
      auto it_upp = values.upper_bound(val);
      if(it_lo == values.end()) {
        assert(node_lo == nullptr);
      } else {
        assert(tr.projector(node_lo->value) == *it_lo);
      }
      if(it_upp == values.end()) {
        assert(node_upp == nullptr);
      } else {
        assert(tr.projector(node_upp->value) == *it_upp);
      }
    }
    trees::Destroy(tr);
  }

  void VerifyBounds() {
    BoundExample1();
    BoundRandom();
  }

public:
  void VerifyAllCases() noexcept {
    std::srand(std::time(0));
    VerifyExamples();
    VerifyInsert();
    VerifyNextPrev();
    VerifyErase();
    VerifyBounds();
  }
};
} // namespace tree_check
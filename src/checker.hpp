#pragma once

#include <ctime>

#include <algorithm>
#include <random>
#include <vector>

#include "bst_verificator.hpp"
#include "tree_common.hpp"

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
  using test_type = Tree;
  using test_key_type = typename test_type::key_type;
  using test_value_type = typename test_type::value_type;
  using test_node_type = typename test_type::node_type;
  using value_generator_type = ValueGenerator;
  using verificator_type = Verificator;
private:
  value_generator_type value_generator_;
  verificator_type verificator_;
  BstVerificator<test_type> bst_verificator_;

  void Verify(const test_type& tree) {
    bst_verificator_(tree);
    verificator_(tree);
  }
public:
  void VerifyExample1() noexcept {
    test_type tree;
    test_node_type* node = new test_node_type;
    node->value = value_generator_(0);
    tree.root = node;
    Verify(tree);
    trees::Destroy(tree);
  }

  void VerifyExample2() noexcept {
    test_type tree;
    test_node_type* node1 = new test_node_type;
    test_node_type* node2 = new test_node_type;
    test_node_type* node3 = new test_node_type;
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

  void VerifyExamples() noexcept {
    VerifyExample1();
    VerifyExample2();
  }

  void VerifyInsert() noexcept {
    const unsigned max_tests = 1000;
    unsigned curr_tests = 100 + std::max(1U, 
      static_cast<unsigned>(std::rand()) % max_tests);
    for(unsigned i = 0U; i < curr_tests; ++i) {
      unsigned curr_queries = 500 + (static_cast<unsigned>(std::rand()) % 20);
      test_type tr;
      for(unsigned j = 0U; j < curr_queries; ++j) {
        Verify(tr);
        int rand_val = rand();
        trees::Insert(tr, value_generator_(rand_val));
        Verify(tr);
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
      test_type tr;
      std::vector<int> numbers;
      while (curr_queries) {
        int num = std::rand();
        Verify(tr);
        auto [node, was_inserted] = trees::Insert(
          tr, value_generator_(num));
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
};
} // namespace tree_check
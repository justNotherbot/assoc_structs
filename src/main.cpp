#include <iostream>
#include <memory>

#include "avl/avl.hpp"
#include "redblack/rb_tree.hpp"

#include "avl/checker.hpp"
#include "redblack/checker.hpp"
#include "checker.hpp"
#include "test_allocator.hpp"

void verify_rb_set_int() {
  using test_type = trees::redblack::rb_tree_t<
    int, int, std::less<int>, trees::Identity<int>, 
    tree_check::TestAllocator<int>>;
  using test_verificator = tree_check::redblack::Verificator<test_type>;
  tree_check::Tester<
    test_type, trees::Identity<int>, test_verificator> tester;
  tester.VerifyAllCases();
}

void verify_rb_map_int() {
  using test_type = trees::redblack::rb_tree_t<
    int, std::pair<int, int>, std::less<int>, trees::PairFirst<int, int>,
    tree_check::TestAllocator<int>>;
  using test_verificator = tree_check::redblack::Verificator<test_type>;
  tree_check::Tester<
    test_type, tree_check::IntValuedPairGenerator<int>, 
    test_verificator> tester;
  tester.VerifyAllCases();
}

void verify_avl_set_int() {
  using test_type = trees::avl::avl_tree_t<
    int, int, std::less<int>, trees::Identity<int>, 
    tree_check::TestAllocator<int>>;
  using test_verificator = tree_check::avl::Verificator<test_type>;
  tree_check::Tester<
    test_type, trees::Identity<int>, test_verificator> tester;
  tester.VerifyAllCases();
}

void verify_avl_map_int() {
  using test_type = trees::avl::avl_tree_t<
    int, std::pair<int, int>, std::less<int>, trees::PairFirst<int, int>,
    tree_check::TestAllocator<int>>;
  using test_verificator = tree_check::avl::Verificator<test_type>;
  tree_check::Tester<
    test_type, tree_check::IntValuedPairGenerator<int>, 
    test_verificator> tester;
  tester.VerifyAllCases();
}

int main() {
  verify_rb_set_int();
  verify_rb_map_int();
  std::cout << "Redblack tests passed\n";
  verify_avl_set_int();
  verify_avl_map_int();
  std::cout << "Avl tests passed\n";
  return 0;
}

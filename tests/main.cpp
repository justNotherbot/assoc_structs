#include <iostream>
#include <map>
#include <memory>
#include <set>

#include <avl/avl.hpp>
#include <redblack/rb_tree.hpp>

#include "avl/checker.hpp"
#include "redblack/checker.hpp"
#include "tree_checker.hpp"
#include "container_tests.hpp"
#include "test_allocator.hpp"

struct NoDefaultConstructible {
  int val;

  NoDefaultConstructible(int x) : val{x} {}

  friend bool operator<(const NoDefaultConstructible& a,
    const NoDefaultConstructible& b) noexcept {
    return a.val < b.val;
  }

  friend bool operator==(const NoDefaultConstructible& a,
    const NoDefaultConstructible& b) noexcept {
    return a.val == b.val;
  }

  friend bool operator!=(const NoDefaultConstructible& a,
    const NoDefaultConstructible& b) noexcept {
    return !(a == b);
  }
};

struct NdcCompare {
  bool operator()(const NoDefaultConstructible& a, 
    const NoDefaultConstructible& b) const noexcept {
    return a.val < b.val;
  }
};

struct NdcFromInt {
  NoDefaultConstructible operator()(int x) const noexcept {
    return NoDefaultConstructible{x};
  }
};

void verify_rb_set_int() {
  using test_type = trees::redblack::rb_tree_t<
    int, int, std::less<int>, trees::Identity<int>, 
    tree_check::TestAllocator<int>>;
  using test_verificator = tree_check::redblack::Verificator<test_type>;
  tree_check::Tester<
    test_type, trees::Identity<int>, test_verificator> tester;
  tester.VerifyAllCases();
  using set_tests = tree_check::ContainerTester<
    my_stl::Set<int, std::less<int>, 
    tree_check::TestAllocator<int>>, trees::Identity<int>, std::set<int>>;
  set_tests set_tester;
  set_tester.VerifyAllCases();
  using set_tests_no_default = tree_check::ContainerTester<
    my_stl::Set<NoDefaultConstructible, NdcCompare, 
    tree_check::TestAllocator<int>>, 
    NdcFromInt, 
    std::set<NoDefaultConstructible>>;
  set_tests_no_default no_default_tester;
  no_default_tester.VerifyAllCases();
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
  using map_tests_int = tree_check::ContainerTester<
    my_stl::Map<int, int, std::less<int>, tree_check::TestAllocator<int>>,
    tree_check::IntValuedPairGenerator<int>, std::map<int, int>>;
  map_tests_int int_tests;
  int_tests.VerifyAllCases();
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

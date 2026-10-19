#include <iostream>

//#include "avl/checker.hpp"
#include "redblack/checker.hpp"

int main() {
  //tree_check::avl::VerifyAllCases();
  std::cout << "Avl tests passed\n";
  tree_check::redblack::Tester<int, int, 
    trees::Identity<int>, trees::Identity<int>> int_tester;
  int_tester.VerifyAllCases();
  tree_check::redblack::Tester<int, std::pair<int, int>, 
    trees::PairFirst<int, int>, 
    tree_check::IntValuedPairGenerator<int>> int_pair_tester;
  int_pair_tester.VerifyAllCases();
  std::cout << "Redblack tests passed\n";
  return 0;
}

#include <iostream>

#include "avl/checker.hpp"
#include "redblack/checker.hpp"

int main() {
  //trees::avl::avl_tree_t<int, std::less<int>> tr;
  //auto [node, inserted] = trees::Insert(tr, 23);
  //assert(trees::)
  tree_check::avl::VerifyAllCases();
  std::cout << "Avl tests passed\n";
  tree_check::redblack::VerifyAllCases();
  std::cout << "Redblack tests passed\n";
  return 0;
}

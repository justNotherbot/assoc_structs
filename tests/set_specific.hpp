#pragma once

#include <cassert>

#include <string>
#include <vector>

#include <containers.hpp>

namespace my_stl_tests {

class SetSpecificTests {
  void InitializerList() {
    my_stl::Set<std::string> my_set{"bb", "aa", "a", "kk", "lmn"};
    std::vector<std::string> res{my_set.begin(), my_set.end()};
    std::vector<std::string> target{"a", "aa", "bb", "kk", "lmn"};
    assert(res == target);
  }
public:
  void VerifyAllCases() {
    InitializerList();
  }
};
} // namespace my_stl_tests

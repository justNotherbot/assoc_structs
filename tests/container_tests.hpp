#pragma once

#include <vector>

#include <containers.hpp>
#include "test_allocator.hpp"

namespace tree_check {

template<typename Test, typename IntToKey, typename Control>
class ContainerTester final {
public:
  using test_type = Test;
  using value_type = typename test_type::value_type;
  using converter_type = IntToKey;
  using control_type = Control;

private:
  converter_type converter_;

  template<typename TreeContainer>
  static bool CompareToSet(TreeContainer& tr, const control_type& st) {
    if(tr.size() != st.size()) {
      return false;
    }
    auto st_it = st.begin();
    for(auto key: tr){
      if(key != *st_it) {
        return false;
      }
      ++st_it;
    }
    return true;
  }

  std::vector<value_type> ToKeyVec(const std::vector<int>& intvec) const {
    std::vector<value_type> ret;
    for(auto i: intvec) {
      ret.push_back(converter_(i));
    }
    return ret;
  }

  void IterationTest() const noexcept {
    std::vector<int> keys = {1, 9, 5, 2, 6};
    test_type tr;
    tr.get_allocator();
    tr.max_size();
    control_type set_keys;
    for(auto key: keys) {
      value_type act_key = converter_(key);
      auto [iter, was_inserted] = tr.insert(act_key);
      auto [s_iter, s_was_inserted] = set_keys.insert(act_key);
      assert(was_inserted == s_was_inserted);
    }

    assert(CompareToSet(tr, set_keys));
  }

  void FromVectorTest() const noexcept {
    std::vector<int> int_vec = {5, 2, 13, 42, 21, 32};
    std::vector<value_type> key_vec = ToKeyVec(int_vec);
    assert(int_vec.size() == key_vec.size());
    test_type tr{key_vec.begin(), key_vec.end()};
    control_type target{key_vec.begin(), key_vec.end()};
    assert(CompareToSet(tr, target));
    const test_type& cref = tr;
    assert(CompareToSet(cref, target));
  }

  void CopyTest() const noexcept {
    std::vector<int> int_vec = {1, 89, 211, -3, 344, 45, 3};
    std::vector<value_type> key_vec = ToKeyVec(int_vec);
    test_type tr{key_vec.begin(), key_vec.end()};
    test_type tr1 = tr;
    assert(tr1 == tr);
    control_type target{key_vec.begin(), key_vec.end()};
    assert(CompareToSet(tr, target));
    assert(CompareToSet(tr1, target));
    test_type tr2;
    tr2.insert(converter_(3));
    assert(tr2 != tr1 && tr2 != tr);
    tr2 = tr;
    assert(tr2 == tr1 && tr2 == tr && CompareToSet(tr2, target));
  }

  void EmplaceTest() const noexcept {
    test_type tr;
    auto [iter, is_inserted] = tr.emplace(converter_(1));
    assert(is_inserted);
    assert(tr.find(1) != tr.end() && tr.size() == 1);
  }

  void EraseTest() const noexcept {
    std::vector<int> int_vec = {2, 54, 4, 9};
    std::vector<int> delete_vec = {2, 4, 6, 8};
    std::vector<value_type> key_vec = ToKeyVec(int_vec);
    test_type tr{key_vec.begin(), key_vec.end()};
    tr.erase(delete_vec.begin(), delete_vec.end());
    std::vector<value_type> res_key = {key_vec[1], key_vec[3]};
    test_type res_tr{res_key.begin(), res_key.end()};
    assert(res_tr == tr);
    assert(res_tr.count(54) && res_tr.count(9) && 
      res_tr.contains(54) && res_tr.count(9));
  }
public:
  void VerifyAllCases() const noexcept {
    IterationTest();
    FromVectorTest();
    CopyTest();
    EmplaceTest();
    EraseTest();
  }
};
} // namespace tree_check

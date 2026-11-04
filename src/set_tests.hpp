#pragma once

#include <set>
#include <vector>

#include "test_allocator.hpp"
#include "tree.hpp"

namespace tree_check {

template<typename Key, typename Comp, typename IntToKey>
class SetTester final {
public:
  using tree_type = trees::redblack::rb_tree_t<
    Key, Key, Comp, trees::Identity<Key>, TestAllocator<Key>>;
  using test_type = trees::Tree<tree_type>;
  using key_type = typename test_type::key_type;
  using converter_type = IntToKey;

private:
  converter_type converter_;

  template<typename TreeContainer>
  static bool CompareToSet(TreeContainer& tr, const std::set<key_type>& st) {
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

  std::vector<key_type> ToKeyVec(const std::vector<int>& intvec) const {
    std::vector<key_type> ret;
    for(auto i: intvec) {
      ret.push_back(converter_(i));
    }
    return ret;
  }

  void IterationTest() const noexcept {
    std::vector<int> keys = {1, 9, 5, 2, 6};
    test_type tr;
    std::set<key_type> set_keys;
    for(auto key: keys) {
      key_type act_key = converter_(key);
      auto [iter, was_inserted] = tr.insert(act_key);
      auto [s_iter, s_was_inserted] = set_keys.insert(act_key);
      assert(was_inserted == s_was_inserted);
    }

    assert(CompareToSet(tr, set_keys));
  }

  void FromVectorTest() const noexcept {
    std::vector<int> int_vec = {5, 2, 13, 42, 21, 32};
    std::vector<key_type> key_vec = ToKeyVec(int_vec);
    assert(int_vec.size() == key_vec.size());
    test_type tr{key_vec.begin(), key_vec.end()};
    std::set<key_type> target{key_vec.begin(), key_vec.end()};
    assert(CompareToSet(tr, target));
    const test_type& cref = tr;
    assert(CompareToSet(cref, target));
  }

  void CopyTest() const noexcept {
    std::vector<int> int_vec = {1, 89, 211, -3, 344, 45, 3};
    std::vector<key_type> key_vec = ToKeyVec(int_vec);
    test_type tr{key_vec.begin(), key_vec.end()};
    test_type tr1 = tr;
    assert(tr1 == tr);
    std::set<key_type> target{key_vec.begin(), key_vec.end()};
    assert(CompareToSet(tr, target));
    assert(CompareToSet(tr1, target));
    test_type tr2;
    tr2.insert(converter_(3));
    assert(tr2 != tr1 && tr2 != tr);
    tr2 = tr;
    assert(tr2 == tr1 && tr2 == tr && CompareToSet(tr2, target));
  }
public:
  void VerifyAllCases() const noexcept {
    IterationTest();
    FromVectorTest();
    CopyTest();
  }
};
} // namespace tree_check

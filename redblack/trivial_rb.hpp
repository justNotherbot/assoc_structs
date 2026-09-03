#pragma once

template<typename Key>
struct rb_node_t {
  bool is_red = false;
  Key key;
  rb_node_t* parent = nullptr;
  rb_node_t* left = nullptr;
  rb_node_t* right = nullptr;
};

template<typename Key>
struct rb_tree_t {
  rb_node_t<Key>* root = nullptr;
};

template<typename Key>
void DestroyImpl(rb_node_t<Key>* node) noexcept {
  if(node == nullptr) {
    return;
  }
  DestroyImpl(node->left);
  DestroyImpl(node->right);
  delete node;
}

template<typename Key>
void Destroy(rb_tree_t<Key> tr) noexcept {
  DestroyImpl(tr.root);
}

template<typename Key>
rb_node_t<Key>* GetClosestImpl(rb_node_t<Key>* node, const Key& k) {
  assert(node != nullptr);
  if(k == node->key) {
    return node;
  }
  if(k > node->key) {
    if(node->right == nullptr) {
      return node;
    }
    return GetClosestImpl(node->right, k);
  }
  if(node->left == nullptr) {
    return node;
  }
  return GetClosestImpl(node->left, k);
}

template<typename Key>
rb_node_t<Key>* GetClosest(rb_tree_t<Key> tr, const Key& k) {
  return GetClosestImpl(tr.root, k);
}

template<typename Key>
void RightRotate(rb_node_t<Key>* node, rb_tree_t<Key>& tr) {
  assert(node != nullptr && node->parent != nullptr && 
    node == node->parent->left);
  node->parent->left = node->right;
  if(node->right != nullptr) {
    node->right->parent = node->parent;
  }
  node->right = node->parent;
  rb_node_t<Key>* prev_parent = node->parent->parent;
  bool rt = false;
  if(prev_parent != nullptr && prev_parent->right == node->parent) {
    rt = true;
  }
  node->parent->parent = node;
  node->parent = prev_parent;
  if(node->parent == nullptr) {
    tr.root = node;
    return;
  }
  if(rt) {
    prev_parent->right = node;
  } else {
    prev_parent->left = node;
  }
}

template<typename Key>
void LeftRotate(rb_node_t<Key>* node, rb_tree_t<Key>& tr) {
  assert(node != nullptr && node->parent != nullptr && 
    node == node->parent->right);
  node->parent->right = node->left;
  if(node->left != nullptr) {
    node->left->parent = node->parent;
  }
  node->left = node->parent;
  rb_node_t<Key>* prev_parent = node->parent->parent;
  bool rt = false;
  if(prev_parent != nullptr && prev_parent->right == node->parent) {
    rt = true;
  }
  node->parent->parent = node;
  node->parent = prev_parent;
  if(node->parent == nullptr) {
    tr.root = node;
    return;
  }
  if(rt) {
    prev_parent->right = node;
  } else {
    prev_parent->left = node;
  }
}

template<typename Key>
void ColorFixup(rb_node_t<Key>* node, rb_tree_t<Key>& tr) {
  if(!node->is_red) {
    return;
  }
  if(node->parent == nullptr) {
    node->is_red = false;
    return;
  }
  if(!node->parent->is_red) {
    return;
  }
  if(node->parent->parent == nullptr) {
    node->parent->is_red = false;
    return;
  }
  rb_node_t<Key>* np = node->parent;
  rb_node_t<Key>* npp = node->parent->parent;
  if(np == npp->left) {
    if(!CheckBlack(npp->right)) {
      npp->is_red = true;
      np->is_red = false;
      npp->right->is_red = false;
      ColorFixup(npp, tr);
    } else {
      rb_node_t<Key>* pivot = node;
      if(node == node->parent->left) {
        pivot = node->parent;
      } else {
        LeftRotate(node, tr);
      }
      RightRotate(pivot, tr);
      pivot->right->is_red = true;
      pivot->is_red = false;
    }
  } else {
    if(!CheckBlack(npp->left)) {
      npp->is_red = true;
      np->is_red = false;
      npp->left->is_red = false;
      ColorFixup(npp, tr);
    } else {
      rb_node_t<Key>* pivot = node;
      if(node == node->parent->right) {
        pivot = node->parent;
      } else {
        RightRotate(node, tr);
      }
      LeftRotate(pivot, tr);
      pivot->left->is_red = true;
      pivot->is_red = false;
    }
  }
}

template<typename Key>
bool CheckBlack(rb_node_t<Key>* node) noexcept {
  if(node == nullptr) {
    return true;
  }
  return !node->is_red;
}

template<typename Key>
std::pair<rb_node_t<Key>*, bool> Insert(rb_tree_t<Key>& tr, const Key& k) {
  rb_node_t<Key>* closest = nullptr;
  if(tr.root != nullptr) {
    closest = GetClosest(tr, k);
    if(closest->key == k) {
      return {closest, false};
    }
  }

  rb_node_t<Key>* new_node = new rb_node_t<Key>;
  new_node->key = k;
  if(tr.root == nullptr) {
    tr.root = new_node;
    return {new_node, true};
  }
  new_node->is_red = true;
  new_node->parent = closest;

  if(k > closest->key) {
    assert(closest->right == nullptr);
    closest->right = new_node;
  } else {
    assert(closest->left == nullptr);
    closest->left = new_node;
  }
  ColorFixup(new_node, tr);
  return {new_node, true};
}

template<typename Key>
rb_node_t<Key>* FindImpl(rb_node_t<Key>* curr, const Key& key) {
  if(curr == nullptr || curr->key == key) {
    return curr;
  }
  rb_node_t<Key>* prev = nullptr;
  if(curr->key > key) {
    return FindImpl(curr->left, key);
  }
  return FindImpl(curr->right, key);
}

template<typename Key>
rb_node_t<Key>* Find(rb_tree_t<Key>& tr, const Key& key) {
  return FindImpl(tr.root, key);
}

template<typename Key>
void BlackDelete(rb_tree_t<Key>& tr, rb_node_t<Key>* node) {
  assert(!node->is_red);
  rb_node_t<Key>* np = node->parent;
  if(np == nullptr) {
    return;
  }
  if(np->right == node) {
    rb_node_t<Key>* npl = np->left;
    if(np->is_red) {
      if(!CheckBlack(npl->left)) {
        RightRotate(npl, tr);
        npl->is_red = true;
        npl->left->is_red = false;
        npl->right->is_red = false;
        return;
      } else if(!CheckBlack(npl->right)) {
        rb_node_t<Key>* pivot = npl->right;
        LeftRotate(pivot, tr);
        RightRotate(pivot, tr);
        pivot->is_red = true;
        pivot->left->is_red = false;
        pivot->right->is_red = false;
        return;
      } else {
        np->is_red = false;
        npl->is_red = true;
        return;
      }
    } else {
      if(npl->is_red) {
        rb_node_t<Key>* node_c = npl->right;
        if(CheckBlack(node_c->left) && !CheckBlack(node_c->right)) {
          rb_node_t<Key>* node_d = node_c->right;
          LeftRotate(node_d, tr);
          node_d->left->is_red = true;
          node_d->is_red = false;
          node_c = node_d;
        }
        if(!CheckBlack(node_c->left)) {
          LeftRotate(node_c, tr);
          RightRotate(node_c, tr);
          node_c->is_red = false;
          node_c->right->is_red = false;
          node_c->left->is_red = true;
          node_c->left->right->is_red = false;
          return;
        } else {
          RightRotate(npl, tr);
          node_c->is_red = true;
          npl->is_red = false;
          np->is_red = false;
          return;
        }
      } else {
        if(!CheckBlack(npl->left)) {
          RightRotate(npl, tr);
          npl->left->is_red = false;
          return;
        } else if(!CheckBlack(npl->right)) {
          rb_node_t<Key>* pivot = npl->right;
          LeftRotate(pivot, tr);
          RightRotate(pivot, tr);
          pivot->is_red = false;
          return;
        } else {
          npl->is_red = true;
          BlackDelete(tr, np);
        }
      }
    }
  } else {
    rb_node_t<Key>* npr = np->right;
    if(np->is_red) {
      if(!CheckBlack(npr->right)) {
        LeftRotate(npr, tr);
        npr->is_red = true;
        npr->left->is_red = false;
        npr->right->is_red = false;
        return;
      } else if(!CheckBlack(npr->left)) {
        rb_node_t<Key>* pivot = npr->left;
        RightRotate(pivot, tr);
        LeftRotate(pivot, tr);
        pivot->is_red = true;
        pivot->left->is_red = false;
        pivot->right->is_red = false;
        return;
      } else {
        np->is_red = false;
        npr->is_red = true;
        return;
      }
    } else {
      if(npr->is_red) {
        rb_node_t<Key>* node_c = npr->left;
        if(CheckBlack(node_c->right) && !CheckBlack(node_c->left)) {
          rb_node_t<Key>* node_d = node_c->left;
          RightRotate(node_d, tr);
          node_d->right->is_red = true;
          node_d->is_red = false;
          node_c = node_d;
        }
        if(!CheckBlack(node_c->right)) {
          RightRotate(node_c, tr);
          LeftRotate(node_c, tr);
          node_c->is_red = false;
          node_c->left->is_red = false;
          node_c->right->is_red = true;
          node_c->right->left->is_red = false;
          return;
        } else {
          LeftRotate(npr, tr);
          node_c->is_red = true;
          npr->is_red = false;
          np->is_red = false;
          return;
        }
      } else {
        if(!CheckBlack(npr->right)) {
          LeftRotate(npr, tr);
          npr->right->is_red = false;
          return;
        } else if(!CheckBlack(npr->left)) {
          rb_node_t<Key>* pivot = npr->left;
          RightRotate(pivot, tr);
          LeftRotate(pivot, tr);
          pivot->is_red = false;
          return;
        } else {
          npr->is_red = true;
          BlackDelete(tr, np);
        }
      }
    }
  }
}

template<typename Key>
void Erase(rb_tree_t<Key>& tr, const Key& key) {
  rb_node_t<Key>* node = Find(tr, key);
  if(node == nullptr) {
    return;
  }
  if(node->left != nullptr && node->right != nullptr) {
    rb_node_t<Key>* curr = node->right;
    rb_node_t<Key>* prev = curr;
    while (curr != nullptr) {
      prev = curr;
      curr = curr->left;
    }
    node->key = prev->key;
    node = prev;
  }
  if(node->left != nullptr) {
    assert(!node->is_red);
    node->key = node->left->key;
    delete node->left;
    node->left = nullptr;
    return;
  }
  if(node->right != nullptr) {
    assert(!node->is_red);
    node->key = node->right->key;
    delete node->right;
    node->right = nullptr;
    return;
  }
  if(node->is_red) {
    if(node == node->parent->right) {
      node->parent->right = nullptr;
    } else {
      node->parent->left = nullptr;
    }
    delete node;
    return;
  }
  BlackDelete(tr, node);
  if(node->parent == nullptr) {
    tr.root = nullptr;
    delete node;
  } else {
    if(node == node->parent->right) {
      node->parent->right = nullptr;
    } else {
      node->parent->left = nullptr;
    }
    delete node;
  }
}

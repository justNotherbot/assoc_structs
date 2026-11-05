# What it is
This is a light-weight dependency-free library, that provides stl-compatible set and map containers.
Provided are implementations of map and set, that can be based on redblack or avl tree
# How to use
Just include the containers.hpp header and use my_stl namespace. Like so:
```C++
#include containers.hpp

int main() {
  my_stl::Set<int> st;
  st.insert(35);
  return 0;
}
```
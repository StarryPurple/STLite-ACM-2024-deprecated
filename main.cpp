#include "vector/src/vector.hpp"

#include "vector/data/class-integer.hpp"
#include "vector/data/class-matrix.hpp"
#include "vector/data/class-bint.hpp"

#include <iostream>
#include <fstream>
#include <string>
int main()
{
  sjtu::vector<long long> v;
  for (long long i = 0; i < 1LL << 20; ++i) {
    v.push_back(i);
  }
  std::cout << v.back() << std::endl;
  for (long long i = 0; i < 1LL << 11; ++i) {
    v.insert(v.begin(), i);
  }
  for (size_t i = 0; i < 1LL << 10; ++i) {
    std::cout << v.front() << std::endl;
    v.erase(v.begin());
  }
  return 0;
}
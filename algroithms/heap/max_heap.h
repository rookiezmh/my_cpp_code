#pragma once

#include <vector>

class MaxHeap {
public:
  typedef std::vector<int>::iterator iterator;
public:
  MaxHeap();
  void Insert(int i);
  void DelMax();
  size_t Size() const;
  bool Empty() const;
  iterator begin();
  iterator end();
private:
  std::vector<int> vec;
  void Swim(size_t hole); // 上溯
  void Sink(size_t hole); // 下溯
};

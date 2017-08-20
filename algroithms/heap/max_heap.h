#pragma once

#include <vector>

class MaxHeap {
public:
  typedef std::vector<int>::const_iterator const_iterator;
public:
  MaxHeap();
  int Top() const;
  void Insert(int i);
  void DelMax();
  size_t Size() const;
  bool Empty() const;
  const_iterator begin() const;
  const_iterator end() const;
private:
  std::vector<int> vec;
  void Swim(size_t hole); // 上溯
  void Sink(size_t hole); // 下溯
};

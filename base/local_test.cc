#include "thread_local.h"
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <iostream>
#include <unistd.h>
#include <stdio.h>

using namespace std;
using namespace util;

struct T {
  string name;
  map<int, string> m;
  vector<string> v;
};
  
ThreadLocal<T> tl;

void T1() {
  printf("thread[1] sleep for 5\n");
  sleep(5);
  T &t = tl.Value();
  printf("%s, %ld, %ld\n", t.name.c_str(), t.m.size(), t.v.size());
}

void T2() {
  printf("thread[2] do the change\n");
  T &t = tl.Value();
  t.name = "haha";
  t.m[1] = "test";
  t.v.push_back("xxx");
  printf("%s, %ld, %ld\n", t.name.c_str(), t.m.size(), t.v.size());
  printf("thread[2] done\n");
}

int main() {
  thread t1(T1);
  thread t2(T2);
  t1.join();
  t2.join();
}

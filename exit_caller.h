#ifndef GALAXY_EXIT_CALLER_H_
#define GALAXY_EXIT_CALLER_H_

namespace util {

class ExitCaller {
public:
  typedef void(*fn)(void *);

public:
  ExitCaller(fn f, void *arg) : fn_(f), arg_(arg) { }
  
  ~ExitCaller() {
    if (fn_) {
      fn_(arg_);
    }
  }

private:
  
  fn fn_;

  void *arg_;

};

}

#endif // GALAXY_EXIT_CALLER_H_

#ifndef PROF_COUNTER
#define PROF_COUNTER


#include <vector>

namespace Professor {
  class Counter {
  public:

    ///
    Counter(int dim, int maxval) {
      for (unsigned int i=0; i< dim;++i) _data.push_back(0);
      _maxval=maxval;
    };
    ~Counter(void);

    bool next(int index);

    int sum();

    std::vector<int> data() { return _data;}

    void print();

  private:
    int _maxval;
    std::vector<int> _data;
  };
}
#endif

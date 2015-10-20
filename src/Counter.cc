#include "Professor/Counter.h"
#include <iostream>
#include <algorithm>

namespace Professor {

  using namespace std;



  Counter::~Counter(void) { }

  bool Counter::next(int index) {
    if (_data[index] == _maxval) {
      if (index==0) return false;

      _data[index]=0;
      return next(index - 1);
    }
    else {
      _data[index]++;
      return true;
    }
  }

  int Counter::sum() {
    int sum_v = 0;
    for (int n : _data)
      sum_v += n;
    return sum_v;
  }

  void Counter::print() {
    for (int n : _data)
      cout << n;
    cout << endl;
  }

}

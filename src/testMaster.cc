// Test for ParamPoints class
#include "Professor/ParamPoints.h"
#include "Professor/ProfMaster.h"
#include <iostream>

int main(int argc, char** argv) {

  vector<double> anchor1{0,0}, anchor2{0,1}, anchor3{0,2}
  vector< vector<double> > points{anchor1, anchor2, anchor3};

  Professor::ParamPoints pp(points);
  Professor::ProfMaster pm(pp), pmvec(points);

  vector<double> val;
  val.push_back(0.0);
  val.push_back(1.0);
  val.push_back(2.0);

  pm.addIpol("Test1", val, 1);
  pm.addIpol("Test2: 1 1.11022e-16 0 1");
  pmvec.addIpol("Test1", val, 1);
  pmvec.addIpol("Test2: 1 1.11022e-16 0 1");

  for (size_t i = 0; i < 10; ++i) {
    double y = i*0.2;
    std::cout << "Value of Test1 at (x,y) = (0," << y << ") = " << pm.getValue("Test1", {0,y}) << std::endl;
  }
  for (size_t i = 0; i < 10; ++i) {
    double y = i*0.2;
    std::cout << "Value of Test1 at (x,y) = (0," << y << ") = " << pmvec.getValue("Test1", {0,y}) << std::endl;
  }

  return 0;
}

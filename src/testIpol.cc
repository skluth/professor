// Test for ParamPoints class
#include "Professor/ParamPoints.h"
#include "Professor/Ipol.h"
#include <iostream>

int main(int argc, char** argv)
{
  vector<double> anchor1, anchor2, anchor3, point;
  anchor1.push_back(0.0);
  anchor1.push_back(0.0);
  anchor2.push_back(0.0);
  anchor2.push_back(1.0);
  anchor3.push_back(0.0);
  anchor3.push_back(2.0);


  vector< vector<double> > points;
  points.push_back(anchor1);
  points.push_back(anchor2);
  points.push_back(anchor3);

  ParamPoints pp(points);

  vector<double> val;
  val.push_back(0.0);
  val.push_back(1.0);
  val.push_back(2.0);
  
  point.push_back(0.0);
  point.push_back(0.5);

  Ipol* ip = new Ipol(pp, val, 1, "FirstTest");
  std::cout << ip->value(anchor1) << std::endl;
  std::cout << ip->value(point) << std::endl;

  std::cout << ip->toString() << std::endl;
  std::cout << ip->toString("Crazy") << std::endl;

  Ipol* ip2 = new Ipol("Test: 2 1 1.11022e-16 0 1");
  Ipol* ip3 = new Ipol("2 1 1.11022e-16 0 1");
  std::cout << ip2->value(point) << std::endl;
  std::cout << ip3->value(point) << std::endl;
  std::cout << ip2->toString() << std::endl;
  std::cout << ip3->toString("Awesome") << std::endl;
  return 0;
}

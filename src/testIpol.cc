// Test for ParamPoints class
#include "ParamPoints.h"
#include "Ipol.h"
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
  //ParamPoints* pp = new ParamPoints(points);

  vector<double> val;
  val.push_back(0.0);
  val.push_back(1.0);
  val.push_back(2.0);
  
  point.push_back(0.0);
  point.push_back(0.5);

  Ipol* ip = new Ipol(pp, val, 1);
  std::cout << ip->value(anchor1) << std::endl;
  std::cout << ip->value(point) << std::endl;



  return 0;
}

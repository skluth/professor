/// Test for ParamPoints class

#include "Professor/ParamPoints.h"

int main(int argc, char** argv) {

  using namespace std;

  vector<double> anchor1{0.5449,0.6028}, anchor2{0.3834,0.9637}, anchor3{0.5289,0.7917};
  // anchor1.push_back(0.5449);
  // anchor1.push_back(0.6028);
  // anchor2.push_back(0.3834);
  // anchor2.push_back(0.9637);
  // anchor3.push_back(0.5289);
  // anchor3.push_back(0.7917);

  vector< vector<double> > points{anchor1,anchor2,anchor3};
  // points.push_back(anchor1);
  // points.push_back(anchor2);
  // points.push_back(anchor3);

  // Test first ctor
  Professor::ParamPoints pp(points);
  pp.printMeta();
  pp.printPoints();

  // Test reset and second ctor
  pp.reset();

  pp.addParamPoint(anchor3);
  pp.addParamPoint(anchor2);
  pp.addParamPoint(anchor1);
  pp.printMeta();
  pp.printPoints();

  return 0;
}

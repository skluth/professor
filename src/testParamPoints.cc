// Test for ParamPoints class
#include "ParamPoints.h"

int main(int argc, char** argv)
{
  vector<double> anchor1, anchor2, anchor3;
  anchor1.push_back(0.5449);
  anchor1.push_back(0.6028);
  anchor2.push_back(0.3834);
  anchor2.push_back(0.9637);
  anchor3.push_back(0.5289);
  anchor3.push_back(0.7917);

  vector< vector<double> > points;
  points.push_back(anchor1);
  points.push_back(anchor2);
  points.push_back(anchor3);


  // Test first ctor
  ParamPoints* pp = new ParamPoints(points);
  pp->printMeta();
  pp->printPoints();

  // Test reset and second ctor
  pp->reset();

  pp->addParamPoint(anchor3);
  pp->addParamPoint(anchor2);
  pp->addParamPoint(anchor1);
  pp->printMeta();
  pp->printPoints();

  delete pp;

  return 0;
}

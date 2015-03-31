#include "Professor/ParamPoints.h"

ParamPoints::ParamPoints(vector< vector<double> > p) {
  _parampoints.clear();
  _locked=false;
  for (int i=0;i<p.size();++i){
    _parampoints.push_back(p[i]);
  }
  _locked=true;
}

vector<double> ParamPoints::center() {
  vector<double> temp_max, temp_min;
  for (unsigned int i=0;i<dim();i++) { // iteration over coordinates
    vector<double> temp;
      for (unsigned int j=0;j<npoints();j++) { // iteration over anchors
        temp.push_back(_parampoints[j][i]);
      }
    temp_max.push_back(*max_element(temp.begin(), temp.end()));
    temp_min.push_back(*min_element(temp.begin(), temp.end()));
  }
  vector<double> center;
  for (unsigned int i=0;i<dim();i++) { // iteration over coordinates
    center.push_back(temp_min[i] + 0.5* (temp_max[i] - temp_min[i]));
  }

  return center;
}

vector<double> ParamPoints::min() {
  vector<double> temp_min;
  for (unsigned int i=0;i<dim();i++) { // iteration over coordinates
    vector<double> temp;
      for (unsigned int j=0;j<npoints();j++) { // iteration over anchors
        temp.push_back(_parampoints[j][i]);
      }
    temp_min.push_back(*min_element(temp.begin(), temp.end()));
  }
  return temp_min;
}

vector<double> ParamPoints::max() {
  vector<double> temp_max;
  for (unsigned int i=0;i<dim();i++) { // iteration over coordinates
    vector<double> temp;
      for (unsigned int j=0;j<npoints();j++) { // iteration over anchors
        temp.push_back(_parampoints[j][i]);
      }
    temp_max.push_back(*max_element(temp.begin(), temp.end()));
  }
  return temp_max;
}


vector<tuple<double, double> > ParamPoints::edges() {
  vector<double> MIN = min();
  vector<double> MAX = max();
  vector<tuple<double, double> > EDGE;

  for (int i=0;i<dim();i++) {
    EDGE.push_back(tuple<double, double>(MIN[i], MAX[i]));
  }
  return EDGE;

}

void ParamPoints::printMeta() {
  std::cout << "Nr. of points: " << npoints() << std::endl;
  std::cout << "Dimension:     " << dim() << std::endl;
  std::cout << "Center:       ";
  for (int i=0;i<dim();i++) {
    std::cout << " " << center()[i];
  }
  std::cout<<std::endl;
  std::cout << "Edges:" << std::endl;
  for (int i=0;i<dim();i++) {
    std::cout << boost::get<0>(edges()[i]) << " < " << boost::get<1>(edges()[i]) << std::endl;
  }
  std::cout << std::endl;
}

void ParamPoints::printPoints() {
  for (int i=0;i<_parampoints.size();++i) {
    std::cout << "Point "<<i<<":"<<std::endl;
    for (int j=0;j<dim();++j) {
      std::cout << _parampoints[i][j] << " ";
    }
    std::cout << std::endl;

  }
}

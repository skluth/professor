#include "Professor/ParamPoints.h"

namespace Professor {

  using namespace std;


  ParamPoints::ParamPoints(const vector< vector<double> >& p) {
    _parampoints.clear();
    _locked = false;
    for (size_t i = 0; i < p.size(); ++i) {
      _parampoints.push_back(p[i]);
    }
    _locked = true;
  }


  vector<double> ParamPoints::center() const {
    vector<double> temp_max, temp_min;
    for (size_t i = 0; i < dim(); i++) { // iteration over coordinates
      vector<double> temp;
      for (size_t j = 0; j < numPoints(); j++) { // iteration over anchors
        temp.push_back(_parampoints[j][i]);
      }
      temp_max.push_back(*max_element(temp.begin(), temp.end()));
      temp_min.push_back(*min_element(temp.begin(), temp.end()));
    }
    vector<double> center;
    for (size_t i = 0; i < dim(); i++) { // iteration over coordinates
      center.push_back(temp_min[i] + 0.5* (temp_max[i] - temp_min[i]));
    }

    return center;
  }


  vector<double> ParamPoints::min() const {
    vector<double> temp_min;
    for (size_t i = 0; i < dim(); i++) { // iteration over coordinates
      vector<double> temp;
      for (size_t j = 0; j < numPoints(); j++) { // iteration over anchors
        temp.push_back(_parampoints[j][i]);
      }
      temp_min.push_back(*min_element(temp.begin(), temp.end()));
    }
    return temp_min;
  }


  vector<double> ParamPoints::max() const {
    vector<double> temp_max;
    for (size_t i = 0; i < dim(); i++) { // iteration over coordinates
      vector<double> temp;
      for (size_t j = 0; j < numPoints(); j++) { // iteration over anchors
        temp.push_back(_parampoints[j][i]);
      }
      temp_max.push_back(*max_element(temp.begin(), temp.end()));
    }
    return temp_max;
  }


  vector< boost::tuple<double, double> > ParamPoints::edges() const {
    /// @todo Use conventional capitalisation
    vector<double> MIN = min();
    vector<double> MAX = max();
    vector< boost::tuple<double, double> > EDGE;
    /// @todo Reserve known size of returned vector before pushing
    for (int i = 0; i < dim(); i++) {
      EDGE.push_back( boost::tuple<double, double>(MIN[i], MAX[i]));
    }
    return EDGE;
  }


  void ParamPoints::printMeta() const {
    cout << "Nr. of points: " << numPoints() << endl;
    cout << "Dimension:     " << dim() << endl;
    cout << "Center:       ";
    for (size_t i = 0; i < dim(); i++) {
      cout << " " << center()[i];
    }
    cout<<endl;
    cout << "Edges:" << endl;
    for (size_t i = 0; i < dim(); i++) {
      cout << boost::get<0>(edges()[i]) << " < " << boost::get<1>(edges()[i]) << endl;
    }
    cout << endl;
  }


  void ParamPoints::printPoints() const {
    for (size_t i = 0; i < numPoints(); ++i) {
      cout << "Point " << i << ":" << endl;
      for (size_t j = 0; j < dim(); ++j) {
        cout << _parampoints[i][j] << " ";
      }
      cout << endl;
    }
  }


}

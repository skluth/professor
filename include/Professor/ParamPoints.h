#ifndef PARAMPOINTS_H
#define PARAMPOINTS_H

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "boost/tuple/tuple.hpp"
#include <cassert> // TODO find proper include for assert
//#include<boost/algorithm/string/split.hpp> // TODO find proper include for assert

using std::string;
using std::vector;
using std::stringstream;
using boost::tuple;

class ParamPoints{
  public:
    ParamPoints();
    ParamPoints(vector< vector<double> > ppoints);
    /// The destructor
    ~ParamPoints() {};
    
    void addParamPoint(vector<double> p) {
      if (!_locked) {
        // This ensures that all ppoints are of the same dimension
        if (_parampoints.size() > 0) assert(p.size() == _parampoints[0].size());
        _parampoints.push_back(p);
      }
      else {
        std::cerr << "Adding point to locked collection not  implemented, aborting" <<std::endl;
        abort();
      }
    };

    int npoints() {return _parampoints.size();}
    int dim() {
      if (npoints()>0) return _parampoints[0].size();
      else return 0;
    }

    vector<double> center();
    vector<double> min();
    vector<double> max();
    vector<tuple<double, double> > edges();
    void printPoints();
    void printMeta();
    void reset() {_parampoints.clear();_locked=false;};


    string toString(const string& info="") {
      stringstream ss;
      if (!info.empty()) ss << "# INFO " << info << "\n";
      ss << "# DIM " << dim() <<"\n";
      ss << "# MINV ";
      for (const double& a : min()) ss << a<< " ";
      ss << " \n";
      ss << "# MAXV ";
      for (const double& a : max()) ss << a<< " ";
      ss << " \n";
      return ss.str();
    }


    vector< vector<double> > points() {return _parampoints;};

  private:

    vector< vector<double> > _parampoints;

    bool _locked;
};
#endif

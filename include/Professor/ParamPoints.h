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

/// Class for the parametrisation hypercube, i.e. anchors
class ParamPoints{
  public:
    /// ctor, takes anchors as vector< vector<double> >
    ParamPoints(vector< vector<double> > ppoints);
    /// The destructor
    ~ParamPoints() {};
    
    /// Overly complicated push_back
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

    /// Number of anchor points
    int npoints() {return _parampoints.size();}

    /// Dimension of (anchor) points
    int dim() {
      if (npoints()>0) return _parampoints[0].size();
      else return 0;
    }

    /// Centre of the anchor hyper cube
    vector<double> center();

    /// Lowest edge of anchor hyper cube
    vector<double> min();
    
    /// Top edge of anchor hyper cube
    vector<double> max();

    /// Edges of the anchor hyper cube
    vector<tuple<double, double> > edges();

    /// print message: anchors
    void printPoints();
    
    /// print message: meta info
    void printMeta();

    /// remove all anchors
    void reset() {_parampoints.clear();_locked=false;};

    /// Header representation with metadata necesary to write out ProfDF
    string toString(const string& info="") {
      stringstream ss;
      if (!info.empty()) ss << "# INFO " << info << "\n";
      ss << "# MINV ";
      for (const double& a : min()) ss << a<< " ";
      ss << " \n";
      ss << "# MAXV ";
      for (const double& a : max()) ss << a<< " ";
      ss << " \n";
      return ss.str();
    }

    /// all anchors
    vector< vector<double> > points() {return _parampoints;};

  private:

    vector< vector<double> > _parampoints;

    bool _locked;
};
#endif

#ifndef PARAMPOINTS_H
#define PARAMPOINTS_H

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <cassert>
#include "boost/tuple/tuple.hpp"

/// @todo Use Professor namespace


/// Class for the parametrisation hypercube, i.e. anchors
class ParamPoints {
public:

  /// ctor, takes anchors as vector< vector<double> >
  ParamPoints(const std::vector< std::vector<double> >& ppoints);
  /// The destructor
  // ~ParamPoints() {};

  /// Overly complicated push_back
  void addParamPoint(const std::vector<double>& p) {
    if (!_locked) {
      // This ensures that all ppoints are of the same dimension
      if (_parampoints.size() > 0) assert(p.size() == _parampoints[0].size());
      _parampoints.push_back(p);
    }
    else {
      std::cerr << "Adding point to locked collection not implemented, aborting" <<std::endl;
      abort();
    }
  };

  /// Number of anchor points
  int npoints() const { return _parampoints.size(); }

  /// Dimension of (anchor) points
  int dim() const {
    return (npoints() > 0) ? _parampoints[0].size() : 0;
  }

  /// Centre of the anchor hyper cube
  std::vector<double> center() const;

  /// Lowest edge of anchor hyper cube
  std::vector<double> min() const;

  /// Top edge of anchor hyper cube
  std::vector<double> max() const;

  /// Edges of the anchor hyper cube
  /// @todo Since we're already using C++11, can we use std::tuple rather than boost::tuple?
  std::vector< boost::tuple<double, double> > edges() const;

  /// print message: anchors
  /// @todo These non-redirectable print functions are a bad idea
  void printPoints() const;

  /// print message: meta info
  /// @todo These non-redirectable print functions are a bad idea
  void printMeta() const;

  /// Remove all anchors
  void reset() { _parampoints.clear(); _locked = false; };

  /// Header representation with metadata necesary to write out ProfDF
  std::string toString(const std::string& info="") const {
    std::stringstream ss;
    if (!info.empty()) ss << "# INFO " << info << "\n";
    ss << "# MINV ";
    for (const double& a : min()) ss << a<< " ";
    ss << " \n";
    ss << "# MAXV ";
    for (const double& a : max()) ss << a<< " ";
    ss << " \n";
    return ss.str();
  }

  /// Get all anchors
  const std::vector< std::vector<double> >& points() const { return _parampoints; }


private:

  std::vector< std::vector<double> > _parampoints;

  bool _locked;

};


#endif

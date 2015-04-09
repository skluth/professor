#ifndef PROF_PARAMPOINTS_H
#define PROF_PARAMPOINTS_H

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <cassert>
#include "boost/tuple/tuple.hpp"

/// @todo Use Professor namespace


/// Typedef for a list of parameters, defining a parameter point
typedef std::vector<double> ParamPoint;


/// Class for the parametrisation hypercube, i.e. anchors
class ParamPoints {
public:

  /// @todo Also record the parameter names in this object

  /// Constructor, takes anchors as vector<vector<double>> = vector<ParamPoint>
  ParamPoints(const std::vector< std::vector<double> >& ppoints);

  /// Implicit conversion operator to vector<vector<double>> = vector<ParamPoint>
  operator std::vector< std::vector<double> > () const {
    return _parampoints;
  }


  /// Overly complicated push_back
  void addParamPoint(const std::vector<double>& p) {
    if (!_locked) {
      // This ensures that all ppoints are of the same dimension
      if (_parampoints.size() > 0) assert(p.size() == _parampoints[0].size());
      _parampoints.push_back(p);
    } else {
      std::cerr << "Adding point to locked collection not implemented, aborting" <<std::endl;
      abort();
    }
  }

  /// Number of anchor points
  int numPoints() const { return _parampoints.size(); }

  /// Dimension of (anchor) points
  int dim() const {
    return (numPoints() > 0) ? _parampoints.front().size() : 0;
  }

  /// Centre of the anchor hyper cube
  std::vector<double> center() const;

  /// Lowest edge of anchor hyper cube
  std::vector<double> min() const;

  /// Top edge of anchor hyper cube
  std::vector<double> max() const;

  /// Edges of the anchor hyper cube
  /// @todo Since we're already using C++11, can we use std::tuple rather than boost::tuple? Why not just std::pair?
  std::vector< boost::tuple<double, double> > edges() const;

  /// print message: anchors
  /// @todo These non-redirectable print functions are a bad idea
  void printPoints() const;

  /// print message: meta info
  /// @todo These non-redirectable print functions are a bad idea
  void printMeta() const;

  /// @todo Add toString method(s) for the main logic in the print functions above, and to connect to Python's ParamPoints.__str__ function.

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

  /// Get all anchor points
  const std::vector< std::vector<double> >& points() const { return _parampoints; }

  /// Get one anchor point
  /// @todo Generalise to other sorts of key lookup? Needed? Bounds checking / checking key existence
  const std::vector<double>& point(size_t i) const { return points().at(i); }


private:

  std::vector< std::vector<double> > _parampoints;

  bool _locked;

};


#endif

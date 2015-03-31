#ifndef IPOL_H
#define IPOL_H
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <map>
#include "boost/tuple/tuple.hpp"
#include "Professor/ParamPoints.h"

using std::string;
using std::min;
using std::cout;
using std::endl;
using std::vector;
using std::stringstream;

class IpolError
{
public:
    IpolError(string reason){
      std::cerr << reason << std::endl;
    };
    ~IpolError(){};

};

class Ipol {
public:

  Ipol(const string s) {
    fromString(s);
  };
  
  // ctor for (lazy) calculation of coeffs
  // TODO understand the const stuff and how to fix it: error: passing ‘const ParamPoints’ as ‘this’ argument of ‘void ParamPoints::printMeta()’ discards qualifiers [-fpermissive]
  Ipol(ParamPoints& pts, vector<double> values, int order, string name="") {
    _values=values;
    _pts=&pts;
    _order=order;
    _name=name;
  };

  int numOfCoefficients(int, int);
  void fromString(const string& s);

  string toString() {
    stringstream ss;
    if (!_name.empty()) ss << _name << ": ";
    ss << this->order() << " ";
    for (const double& a : coeffs())
      ss << a << " ";
    return ss.str();
  }
  string toString(const string& name) {
    stringstream ss;
    if (!name.empty()) ss << name << ": ";
    ss << this->order() << " ";
    for (const double& a : coeffs())
      ss << a << " ";
    return ss.str();
  }
  
  double value(vector <double> p);

  double value(const ParamPoints& pt) const {
    return 1.0;
  }

  vector<double> coeffs() const {
    if (_coeffs.empty()) {
      if (_pts == NULL) throw IpolError("No ParameterPoints available when calculating ipol coeffs");
      //_calcCoeffs(); TODO get this to work, something about constness being lost
      _pts = NULL; //< Not necessary, but ensures consistency
    }
    return _coeffs;
  }

  int dim() const { return _pts->dim(); }

  int order() const { return _order; }

  string name() const {return _name; }

private:

  int _order;

  string _name;

  vector<double> _values;

  mutable vector<double> _coeffs;

  mutable ParamPoints* _pts; //= 0; TODO: warning: non-static data member initializers only available with -std=c++11
  
  void _calcCoeffs(); // Most generic way of parametrisation
  
  vector<double> getLongVector1D(vector<double> p);
  vector<double> getLongVector2D(vector<double> p);
  vector<double> getLongVector3D(vector<double> p);
  vector<double> getLongVector4D(vector<double> p);
  vector<double> getLongVector5D(vector<double> p);
  vector<double> getLongVector6D(vector<double> p);
  vector<double> getLongVector(vector<double> p, vector<double> coeffs, int order);
  vector<double> getLongVector(vector<double> p, int order);

};

#endif

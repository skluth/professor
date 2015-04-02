#ifndef IPOL_H
#define IPOL_H
#include <string>
#include <vector>
#include <sstream>
#include "Professor/ParamPoints.h"

using std::string;
using std::cout;
using std::endl;
using std::vector;
using std::stringstream;

/// Throwable error
class IpolError
{
public:
    IpolError(string reason){
      std::cerr << reason << std::endl;
    };
    ~IpolError(){};
};

/// The heart of Professor
class Ipol 
{
public:
  /// ctor for calculation of coefficients
  Ipol(ParamPoints& pts, vector<double> values, int order, string name="") {
    _values=values;
    _pts=&pts;
    _order=order;
    _name=name;
  };
 
  /// ctor to read ipol from file (one string for each object) 
  Ipol(const string s) {
    fromString(s);
  };
  
  /// The dtor
  ~Ipol(){};


  int numOfCoefficients(int, int);

  /// Read and set coefficients (name), order from string
  void fromString(const string& s);

  /// Get string representation
  string toString() {
    stringstream ss;
    if (!_name.empty()) ss << _name << ": ";
    ss << this->order() << " ";
    for (const double& a : coeffs())
      ss << a << " ";
    return ss.str();
  }
  
  /// Get string representation --- define name explicitly
  string toString(const string& name) {
    stringstream ss;
    if (!name.empty()) ss << name << ": ";
    ss << this->order() << " ";
    for (const double& a : coeffs())
      ss << a << " ";
    return ss.str();
  }
  
  /// Get the value of the parametrisation at point p
  double value(vector <double> p);

  /// Get a vector of coefficients --- at some point figure out how to call _calcCoeffs
  vector<double> coeffs() const {
    if (_coeffs.empty()) {
      if (_pts == NULL) throw IpolError("No ParameterPoints available when calculating ipol coeffs");
      //_calcCoeffs(); TODO get this to work, something about constness being lost
      _pts = NULL; //< Not necessary, but ensures consistency
    }
    return _coeffs;
  }

  /// Accessor to the dimension of the ParamPoints --- can probably safely be removed
  int dim() const { return _pts->dim(); }

  /// Get the order of the parametrisation
  int order() const { return _order; }

  /// Get the name of the parametrised object
  string name() const {return _name; }

  ParamPoints* params() const {return _pts; }

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

#ifndef PROF_IPOL_H
#define PROF_IPOL_H

#include "Professor/ParamPoints.h"
#include <string>
#include <vector>
#include <sstream>

/// @todo Don't import these in public namespace. We should probably have a Professor namespace.
using std::string;
using std::cout;
using std::endl;
using std::vector;
using std::stringstream;


/// Throwable error
/// @todo  What's the point in a non-silenceable exception with no state?! Assume this is a placeholder to be improved...
class IpolError {
public:
    IpolError(const string& reason) {
      std::cerr << reason << std::endl;
    };
    ~IpolError(){};
};


/// Calculate the number of coefficients for the given parameter space dimension and polynomial order
/// @todo Rename to numCoeffs() for compactness & compatibility with coeffs() function
/// @todo Deal in uints
int numCoeffs(int dim, int order);


/// The heart of Professor: the interpolation of a single numerical value through the parameter space
class Ipol {
public:

  /// Constructor for calculation of coefficients
  Ipol(const ParamPoints& pts, const vector<double>& values, int order, const string& name="") {
    _values = values;
    _pts = &pts;
    _order = order;
    _name = name;
  };

  /// Constructor to read ipol from file (one string for each object)
  Ipol(const string& s) {
    fromString(s);
  };

  /// The dtor
  // ~Ipol() { };

  /// Read and set coefficients (name), order from string
  void fromString(const string& s);

  /// Get string representation
  string toString(const string& name="") const {
    stringstream ss;
    if (!name.empty()) ss << name << ": ";
    else if (!_name.empty()) ss << _name << ": ";
    ss << this->order() << " ";
    for (const double& a : coeffs())
      ss << a << " ";
    return ss.str();
  }

  /// Get the value of the parametrisation at point p
  double value(const vector<double>& p) const;

  /// Get a single coefficient, calculated lazily and cached
  double coeff(size_t i) const {
    return coeffs()[i];
  }

  /// Get the vector of coefficients, calculated lazily and cached
  const vector<double>& coeffs() const;

  /// Accessor to the dimension of the ParamPoints
  int dim() const { return _pts->dim(); }

  /// Get the order of the parametrisation
  int order() const { return _order; }

  /// Get the name of the parametrised object
  string name() const {return _name; }

  /// Get the attached params -- may be NULL after the coeffs have been computed
  const ParamPoints* params() const { return _pts; }


protected:

  /// Calculate parametrisation coefficients
  void _calcCoeffs() const;

  /// Get the vector of
  vector<double> _getLongVector(const vector<double>& p, int order) const;

  /// @todo What is the point of this, since the passed coeffs are not used?
  vector<double> _getLongVector(const vector<double>& p, const vector<double>& coeffs, int order) const {
    /// @todo Throw an IpolError instead
    if (coeffs.size() != numCoeffs(p.size(), order))
      std::cout << "ERROR invalid number of coefficients: " << coeffs.size() << " supplied, " << numCoeffs(p.size(), order) << " required, exiting" << std::endl;
    return _getLongVector(p, order);
  }

  /// @name Explicit long-vector calculators for various polynomial orders
  //@{
  vector<double> _getLongVector1D(const vector<double>& p) const;
  vector<double> _getLongVector2D(const vector<double>& p) const;
  vector<double> _getLongVector3D(const vector<double>& p) const;
  vector<double> _getLongVector4D(const vector<double>& p) const;
  vector<double> _getLongVector5D(const vector<double>& p) const;
  vector<double> _getLongVector6D(const vector<double>& p) const;
  //@}


private:

  int _order;
  string _name;
  vector<double> _values;
  mutable vector<double> _coeffs;
  mutable const ParamPoints* _pts; //= 0; TODO: warning: non-static data member initializers only available with -std=c++11

};

#endif

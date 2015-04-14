#ifndef PROF_IPOL_H
#define PROF_IPOL_H

#include "Professor/ParamPoints.h"
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

namespace Professor {


  /// Throwable error
  /// @todo  What's the point in a non-silenceable exception with no state?! Assume this is a placeholder to be improved...
  class IpolError {
  public:
    IpolError(const std::string& reason) {
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
    ///
    /// @todo The pts nested vector passed in is stored by pointer for lazy
    /// evaluation. The object passed as pts therefore must remain valid until the
    /// ipol.coeffs() or ipol.value() function is called (at which stage the
    /// pointer is nullified). This is for memory & CPU efficiency: lazy
    /// evaluation of coeffs is good, but we can't afford to have every bin
    /// storing the same full list of hundreds of N-dimensional parameter points!
    // Ipol(const std::vector< std::vector<double> >& pts, const std::vector<double>& values, int order, const std::string& name="") {
    Ipol(const ParamPoints& pts, const std::vector<double>& values, int order, const std::string& name="") {
      _values = values;
      _pts = &pts;
      _dim = pts.numPoints();
      _order = order;
      _name = name;
    };

    /// Constructor to read ipol from file (one string for each object)
    Ipol(const std::string& s) {
      fromString(s);
    };

    /// Read and set coefficients (name), order from string
    void fromString(const std::string& s);

    /// Get string representation
    std::string toString(const std::string& name="") const {
      std::stringstream ss;
      if (!name.empty()) ss << name << ": ";
      else if (!_name.empty()) ss << _name << ": ";
      ss << this->order() << " ";
      for (const double& a : coeffs())
        ss << a << " ";
      return ss.str();
    }

    /// Get the value of the parametrisation at point p
    double value(const std::vector<double>& p) const;

    /// Get a single coefficient, calculated lazily and cached
    double coeff(size_t i) const {
      return coeffs()[i];
    }

    /// Get the vector of coefficients, calculated lazily and cached
    const std::vector<double>& coeffs() const;

    /// Accessor to the dimension of the param points
    /// @todo This should be known, cf. order(), without needing to have an attached ParamPoints object
    int dim() const {
      if (_pts == NULL) throw IpolError("Ipol: Attempted to get parameter dimension without an attached ParamPoints");
      return _pts->dim();
    }

    /// Get the order of the parametrisation
    int order() const { return _order; }

    /// Get the name of the parametrised object
    std::string name() const {return _name; }

    /// Get the attached params -- may be NULL after the coeffs have been computed
    const ParamPoints* params() const { return _pts; }


  protected:

    /// Calculate parametrisation coefficients
    void _calcCoeffs() const;

    /// Get the vector of
    std::vector<double> _getLongVector(const std::vector<double>& p, int order) const;

    /// @todo What is the point of this, since the passed coeffs are not used?
    std::vector<double> _getLongVector(const std::vector<double>& p, const std::vector<double>& coeffs, int order) const {
      /// @todo Throw an IpolError instead
      if (coeffs.size() != numCoeffs(p.size(), order))
        std::cout << "ERROR invalid number of coefficients: " << coeffs.size() << " supplied, " << numCoeffs(p.size(), order) << " required, exiting" << std::endl;
      return _getLongVector(p, order);
    }

    /// @name Explicit long-vector calculators for various polynomial orders
    //@{
    std::vector<double> _getLongVector1D(const std::vector<double>& p) const;
    std::vector<double> _getLongVector2D(const std::vector<double>& p) const;
    std::vector<double> _getLongVector3D(const std::vector<double>& p) const;
    std::vector<double> _getLongVector4D(const std::vector<double>& p) const;
    std::vector<double> _getLongVector5D(const std::vector<double>& p) const;
    std::vector<double> _getLongVector6D(const std::vector<double>& p) const;
    //@}


  private:

    int _dim, _order;
    std::string _name;
    std::vector<double> _values;
    mutable std::vector<double> _coeffs;
    mutable const ParamPoints* _pts; //= 0; TODO: warning: non-static data member initializers only available with -std=c++11

  };


}

#endif

#ifndef PROF_IPOL_H
#define PROF_IPOL_H

#include "Professor/ParamPoints.h"
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <stdexcept>

namespace Professor {


  /// Throwable error
  struct IpolError : public std::runtime_error {
    IpolError(const std::string& reason) : std::runtime_error(reason) { }
  };


  /// @name Calculator functions for parameterisation elements
  //@{

  /// Calculate the number of coefficients for the given parameter space dimension and polynomial order
  /// @todo Deal in uints
  int numCoeffs(int dim, int order);

  /// Calculate parametrisation coefficients
  std::vector<double> calcCoeffs(const ParamPoints& pts, const std::vector<double>& vals, int order, double threshold);

  /// Make the vector of polynomial terms to which the coeffs are to be applied, at the given order
  std::vector<double> mkLongVector(const std::vector<double>& p, int order);

  //@}



  /// The heart of Professor: the interpolation of a single numerical value through the parameter space
  class Ipol {
  public:

    /// Constructor for calculation of coefficients
    Ipol(const ParamPoints& pts, const std::vector<double>& ptvals, int order, const std::string& name="", double threshold=1e-15) {
      _dim = pts.dim();
      _order = order;
      _name = name;
      _coeffs = calcCoeffs(pts, ptvals, _order, threshold);
      _minPV = pts.ptmins();
      _maxPV = pts.ptmaxs();
    };


    /// Constructor to read ipol from file (one string for each object)
    Ipol(const std::string& s) {
      fromString(s);
    };


    /// Get string representation
    std::string toString(const std::string& name="") const;

    /// Read and set coefficients (name), order from string
    void fromString(const std::string& s);


    /// Get the value of the parametrisation at point p
    double value(const std::vector<double>& p) const;

    /// Get the vector of coefficients, calculated lazily and cached
    const std::vector<double>& coeffs() const { return _coeffs; }

    /// Get a single coefficient, calculated lazily and cached
    double coeff(size_t i) const { return coeffs()[i]; }


    /// Accessor to the dimension of the param points
    int dim() const { return _dim; }

    /// Get the order of the parametrisation
    int order() const { return _order; }

    /// Get the name of the parametrised object
    std::string name() const {return _name; }

    void setMinPV(std::vector<double> c) {_minPV=c;}
    void setMaxPV(std::vector<double> c) {_maxPV=c;}

    //@}


  private:

    int _dim, _order;
    std::string _name;
    std::vector<double> _coeffs, _minPV, _maxPV;

  };


}

#endif

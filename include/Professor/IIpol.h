// -*- C++ -*-
#ifndef PROF_IIPOL_H
#define PROF_IIPOL_H

// #include "Professor/ParamPoints.h"
#include <string>
#include <vector>
//#include <sstream>
//#include <iostream>
#include <stdexcept>

namespace Professor {

  /// Throwable error
  struct IpolError : public std::runtime_error {
    IpolError( const std::string& reason ) : std::runtime_error( reason ) { }
  };

  class IIpol {
  public:

    /// Get string representation
    virtual std::string toString( const std::string& ) const = 0;

    /// Read and set coefficients (name), order from string
    virtual void fromString( const std::string& ) = 0;

    /// @name Calculations
    //@{

    /// Get the value of the parametrisation at point p
    virtual double value( const std::vector<double>& ) const = 0;

    /// Get the value of the derivative of the parametrisation at point p
    /// @todo Expose as a standalone calcDerivative function, cf. calcValue
    virtual double derivative( const std::vector<double>& ) const = 0;

    /// Get the gradient of the parametrisation at point p
    /// @todo Expose as a standalone calcGradient function, cf. calcValue
    virtual std::vector<double> gradient( const std::vector<double>& ) const = 0;

    //@}

    /// @name Limit-setting
    //@{

    void setParamLimits(const std::vector<double>& minpvs, const std::vector<double>& maxpvs) {
      setMinParamVals(minpvs);
      setMaxParamVals(maxpvs);
    }

    const std::vector<double>& minParamVals() { return _minPV; }
    const std::vector<double>& maxParamVals() { return _maxPV; }

    void setMinParamVals(const std::vector<double>& minpvs) { _minPV = minpvs; }
    void setMaxParamVals(const std::vector<double>& maxpvs) { _maxPV = maxpvs; }

    //@}

  protected:

    std::vector<double> _minPV, _maxPV;

  };


}

#endif

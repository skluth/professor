// -*- C++ -*-
#ifndef PROF_SIMPLEXINTERPOLATION_H
#define PROF_SIMPLEXINTERPOLATION_H

#include "Professor/IIpol.h"
//#include "Professor/ParamPoints.h"
#include <string>
#include <vector>


namespace Professor {

  class ParamPoints;

  class SimplexInterpolation : public IIpol {
  public:

    SimplexInterpolation( const ParamPoints&, const std::vector<double>& );
    
    virtual ~SimplexInterpolation() {}

    /// Get string representation
    virtual std::string toString( const std::string& ) const;

    /// Read and set coefficients (name), order from string
    virtual void fromString( const std::string& );

    /// @name Calculations
    //@{

    /// Get the value of the parametrisation at point p
    virtual double value( const std::vector<double>& ) const;

    /// Get the value of the derivative of the parametrisation at point p
    /// @todo Expose as a standalone calcDerivative function, cf. calcValue
    virtual double derivative( const std::vector<double>& ) const;

    /// Get the gradient of the parametrisation at point p
    /// @todo Expose as a standalone calcGradient function, cf. calcValue
    virtual std::vector<double> gradient( const std::vector<double>& ) const;

    //@}

  private:

    double barycentricInterpolation_2d( std::vector<double> parameters,
					std::vector<double> closest,
					std::vector<double> secondclosest,
					std::vector<double> thirdclosest ) const;

    std::vector< std::vector<double> > points;

  };

}

#endif

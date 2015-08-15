#include "Professor/Ipol.h"
#include "eigen3/Eigen/SVD"
#include <sstream>
#include <cassert>

namespace Professor {

  using namespace std;
  using namespace Eigen;


  // NB. Not a member function
  int numCoeffs(int dim, int order) {
    int ntok = 1;
    int r = min(order, dim);
    for (int i = 0; i < r; ++i) {
      ntok = ntok*(dim+order-i)/(i+1);
    }
    return ntok;
  }


  // NB. Not a member function
  std::vector<double> calcCoeffs(const ParamPoints& pts, const vector<double>& vals, int order, double threshold) {
    vector<double> rtn;
    if (order==0) {
      rtn.push_back(vals[0]);
      return rtn;
    }

    if (pts.numPoints() != vals.size())
      throw IpolError("pts.numPoints() != vals.size() in calcCoeffs");
    const int ncoeff = numCoeffs(pts.dim(), order);
    if (ncoeff > pts.numPoints()) {
      stringstream ss;
      ss << "Ipol: not enough (" << ncoeff << " vs. " << pts.numPoints() << ") anchor points "
         << "for interpolating with " << pts.dim() << " params at order " << order;
      throw IpolError(ss.str());
    }
    MatrixXd DP = MatrixXd(pts.numPoints(), ncoeff);
    VectorXd MC = VectorXd(pts.numPoints());

    vector<double> tempLV;
    vector<double> tempDP;
    // Populate the matrix to be inverted
    for (int a = 0; a < pts.numPoints(); ++a) {
      tempLV = mkLongVector(pts.point(a), order);
      for (size_t i = 0; i < tempLV.size(); ++i) {
        DP(a, i) = tempLV[i];
      }
      // The vector of values (corresponding to anchors)
      MC[a] = vals[a];
    }
    JacobiSVD<MatrixXd> svd = DP.jacobiSvd(ComputeThinU|ComputeThinV);

    // Check for singular values, i.e. fully correlated parameters
    // TODO maybe figure out how to use Eigen's setTreshold 
    VectorXd svals = svd.singularValues();
    for (unsigned int i=0;i<svd.nonzeroSingularValues();++i) {
      if (fabs(svals[i]) < threshold) {
        std::cout << "Singular value encountered, aborting" << std::endl;
        abort();
      }
    }
    
    VectorXd co = svd.solve(MC);

    for (size_t i = 0; i < ncoeff; ++i) rtn.push_back(co[i]);
    return rtn;
  }


  // NB. Not a member function
  vector<double> mkLongVector(const vector<double>& p, int order) {
    if (order < 0 || order > 6) {
      std::cout << "ERROR degree " << order << " not implemented, exiting" << std::endl;
      /// @todo Never call exit() from a library function. Throw an IpolError instead
      // exit(1);
    }
    if (order == 0) return mkLongVector0D(p);
    if (order == 1) return mkLongVector1D(p);
    if (order == 2) return mkLongVector2D(p);
    if (order == 3) return mkLongVector3D(p);
    if (order == 4) return mkLongVector4D(p);
    if (order == 5) return mkLongVector5D(p);
    if (order == 6) return mkLongVector6D(p);
  }


  // /// @todo What's the point, other than adding an optional consistency check?
  // std::vector<double> Ipol::mkLongVector(const std::vector<double>& p, const std::vector<double>& coeffs, int order) {
  //   if (coeffs.size() != numCoeffs(p.size(), order)) {
  //     stringstream ss;
  //     ss << "Invalid number of coefficients: " << coeffs.size() << " supplied, " << numCoeffs(p.size(), order) << " required";
  //     throw IpolError(ss.str());
  //   }
  //   return mkLongVector(p, order);
  // }

  // NB. Not a member function --- for constant values
  vector<double> mkLongVector0D(const vector<double>& p) {
    vector<double> retvec;
    // 0th order offset
    retvec.push_back(1.0);
    return retvec;
  }

  // NB. Not a member function
  vector<double> mkLongVector1D(const vector<double>& p) {
    vector<double> retvec;
    retvec.reserve(1 + p.size());
    // 0th order offset
    retvec.push_back(1.0);
    // Linear coefficients
    for (size_t i = 0; i < p.size(); ++i)
      retvec.push_back(p[i]);
    assert(retvec.size() == numCoeffs(p.size(), 1));
    return retvec;
  }


  // NB. Not a member function
  vector<double> mkLongVector2D(const vector<double>& p) {
    vector<double> retvec = mkLongVector1D(p);
    // Quadratic coefficients
    for (size_t i = 0; i < p.size(); i++)
      for (size_t j = 0; j < p.size(); j++)
        if (i <= j)
          retvec.push_back(p[i]*p[j]);
    assert(retvec.size() == numCoeffs(p.size(), 2));
    return retvec;
  }


  // NB. Not a member function
  vector<double> mkLongVector3D(const vector<double>& p) {
    vector<double> retvec = mkLongVector2D(p);
    // Cubic coefficients
    for (size_t i = 0; i < p.size(); i++)
      for (size_t j = 0; j < p.size(); j++)
        for (size_t k = 0; k < p.size(); k++)
          if (i<=j && i<=k && j<=k)
            retvec.push_back(p[i]*p[j]*p[k]);
    assert(retvec.size() == numCoeffs(p.size(), 3));
    return retvec;
  }


  // NB. Not a member function
  vector<double> mkLongVector4D(const vector<double>& p) {
    vector<double> retvec = mkLongVector3D(p);
    // Quartic coefficients
    for (size_t i = 0; i < p.size(); i++)
      for (size_t j = 0; j < p.size(); j++)
        for (size_t k = 0; k < p.size(); k++)
          for (size_t l = 0; l < p.size(); l++)
            if (i<=j && i<=k && i<=l &&
                j<=k && j<=l &&
                k<=l)
              retvec.push_back(p[i]*p[j]*p[k]*p[l]);
    assert(retvec.size() == numCoeffs(p.size(), 4));
    return retvec;
  }


  // NB. Not a member function
  vector<double> mkLongVector5D(const vector<double>& p) {
    vector<double> retvec = mkLongVector4D(p);
    // Quintic coefficients
    for (size_t i = 0; i < p.size(); i++)
      for (size_t j = 0; j < p.size(); j++)
        for (size_t k = 0; k < p.size(); k++)
          for (size_t l = 0; l < p.size(); l++)
            for (size_t m = 0; m < p.size(); m++)
              if (i<=j && i<=k && i<=l && i<=m &&
                  j<=k && j<=l && j<=m &&
                  k<=l && k<=m &&
                  l<=m)
                retvec.push_back(p[i]*p[j]*p[k]*p[l]*p[m]);
    assert(retvec.size() == numCoeffs(p.size(), 5));
    return retvec;
  }


  // NB. Not a member function
  vector<double> mkLongVector6D(const vector<double>& p) {
    vector<double> retvec = mkLongVector5D(p);
    // 6th order coefficients
    for (size_t i = 0; i < p.size(); i++)
      for (size_t j = 0; j < p.size(); j++)
        for (size_t k = 0; k < p.size(); k++)
          for (size_t l = 0; l < p.size(); l++)
            for (size_t m = 0; m < p.size(); m++)
              for (size_t n = 0; n < p.size(); n++)
                if (i<=j && i<=k && i<=l && i<=m && i<=n &&
                    j<=k && j<=l && j<=m && j<=n &&
                    k<=l && k<=m && k<=n &&
                    l<=m && l<=n &&
                    m<=n)
                  retvec.push_back(p[i]*p[j]*p[k]*p[l]*p[m]*p[n]);
    assert(retvec.size() == numCoeffs(p.size(), 6));
    return retvec;
  }



  ///////////////////////////////////////////////////////



  string Ipol::toString(const string& name) const {
    stringstream ss;
    if (!name.empty()) ss << name << ": ";
    else if (!_name.empty()) ss << _name << ": ";
    ss << this->dim() << " ";
    ss << this->order() << " ";
    for (const double& a : coeffs())
      ss << a << " ";
    return ss.str();
  }


  void Ipol::fromString(const string& s) {
    // Extract a name if given at the start of the string
    _name = (s.find(":") != std::string::npos) ? s.substr(0, s.find(":")) : "";
    // Load the rest of the string into a stringstream and load into numerical variables
    istringstream numss( (s.find(":") != std::string::npos) ? s.substr(s.find(":")+1) : s );
    numss >> _dim;
    numss >> _order;
    double tmp; int ncoeffs = 0;
    while (numss >> tmp) {
      _coeffs.push_back(tmp);
      ncoeffs += 1;
    }
    assert(ncoeffs == numCoeffs(dim(),order()));
  }


  double Ipol::value(const vector<double>& params) const {
    if (params.size() != dim()) {
      stringstream ss;
      ss << "Incorrect number of parameters passed to Ipol::value ("
         << dim() << " params required, " << params.size() << " supplied)";
      throw IpolError(ss.str());
    }
    const vector<double> lv = mkLongVector(params, order());
    assert(lv.size() == coeffs().size());
    double v = 0.0;
    for (size_t i = 0; i < lv.size(); ++i) {
      v += lv[i] * coeff(i);
    }
    return v;
  }


}

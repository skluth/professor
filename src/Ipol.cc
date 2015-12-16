#include "Professor/Ipol.h"
#include "Professor/Counter.h"
#include "Eigen/SVD"
#include <sstream>
#include <cassert>
#include <cmath>


namespace Professor {

  using namespace std;
  using namespace Eigen;


  namespace { //< hide this symbol, since not in API

    // Scaling function to map x from [a,b] into [1,2].
    // NB. Target range does not touch 0, e.g. [0,1] to avoid raising very small numbers to large powers.
    double map_prange(double x, double a, double b) {
      return (x-a)/(b-a);
    }
  }


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
  // structure is the pre-calculated algebraic structure of the polynomial
  std::vector<double> calcCoeffs(const ParamPoints& pts, const vector<double>& vals, int order, double threshold, vector<vector<int> > structure) {

    // Early exit if this is a trivial 0th order polynomial
    vector<double> rtn;
    if (order == 0) {
      rtn.push_back(vals[0]);
      return rtn;
    }

    // Check the inputs
    if (pts.numPoints() != vals.size())
      throw IpolError("pts.numPoints() != vals.size() in calcCoeffs");
    const int ncoeff = numCoeffs(pts.dim(), order);
    if (ncoeff > pts.numPoints()) {
      stringstream ss;
      ss << "Ipol: not enough (" << ncoeff << " vs. " << pts.numPoints() << ") anchor points "
         << "for interpolating with " << pts.dim() << " params at order " << order;
      for (unsigned int i_order=1;i_order<order;i_order++) {
        if (numCoeffs(pts.dim(), i_order)<=pts.numPoints())
          ss << "\n Order " << i_order  << " requires " << numCoeffs(pts.dim(), i_order) << " anchors";
      }
      throw IpolError(ss.str());
    }

    // Create Eigen objects for the SVD solving
    MatrixXd DP = MatrixXd(pts.numPoints(), ncoeff);
    VectorXd MC = VectorXd(pts.numPoints());

    // The parameter scaling business
    std::vector<std::vector<double> > origpoints = pts.points();
    std::vector<std::vector<double> > scaledpoints;
    std::vector<double> minPV = pts.ptmins();
    std::vector<double> maxPV = pts.ptmaxs();

    for (int p = 0; p < origpoints.size(); ++p) {
      std::vector<double> temp;
      for (int i = 0; i < pts.dim(); ++i) {
        temp.push_back(map_prange(origpoints[p][i], minPV[i], maxPV[i]));
      }
      scaledpoints.push_back(temp);
    }


    // Populate the matrix to be inverted
    vector<double> tempLV;
    for (int a = 0; a < pts.numPoints(); ++a) {
      tempLV = mkLongVector(scaledpoints[a], order, structure);
      for (size_t i = 0; i < tempLV.size(); ++i) {
        DP(a, i) = tempLV[i];
      }
      // The vector of values (corresponding to anchors)
      MC[a] = vals[a];
    }
    JacobiSVD<MatrixXd> svd = DP.jacobiSvd(ComputeThinU|ComputeThinV);
    #if EIGEN_WORLD_VERSION >= 3 && EIGEN_MAJOR_VERSION >= 2 && EIGEN_MINOR_VERSION >= 1
    svd.setThreshold(threshold); // Needed TODO find transform for dependence on stuff
    #endif

    // Check for singular values, i.e. fully correlated parameters
    /// @todo Maybe figure out how to use Eigen's setThreshold better?
    VectorXd svals = svd.singularValues();
    for (unsigned int i = 0; i < svd.nonzeroSingularValues();++i) {
      if (fabs(svals[i]) < threshold) {
        std::cout << "Singular value encountered, aborting" << std::endl;
        abort();
      }
    }

    // Solve for coefficients
    VectorXd co = svd.solve(MC);

    // Populate the coefficient std::vector and return
    for (size_t i = 0; i < ncoeff; ++i) rtn.push_back(co[i]);
    return rtn;
  }


  // NB. Not a member function
  vector<vector<int> > mkStructure(int p, int order) {
    if (order < 0)
      throw IpolError("Polynomial order " + to_string(order) + " not implemented");

    const int N = p;
    const vector<int> zero(N, 0);
    vector<vector<int> > rtn;
    rtn.push_back(zero);

    for (unsigned int i = 0; i <= order; ++i) {
      Professor::Counter c(N,i);
      while (c.next(N-1)) {
        if (c.sum() == i) {
          rtn.push_back(c.data());
        }
      }
    }
    return rtn;
  }


  // NB. Not a member function
  vector<double> mkLongVector(const vector<double>& p, int order, vector< vector<int> > structure) {
    if (order < 0)
      throw IpolError("Polynomial order " + to_string(order) + " not implemented");


    vector<double> rtn;
    for (const vector<int>& v : structure) {
      double prod = 1.0;
      for (size_t i = 0; i < v.size(); ++i) {
        prod *= std::pow(p[i],v[i]);
      }
      rtn.push_back(prod);
    }
    return rtn;
  }


  // NB. Not a member function
  vector<double> mkLongVectorDerivative(const vector<double>& p, int order, vector<double> minPV, vector<double> maxPV, vector<vector<int> > structure) {
    if (order < 0)
      throw IpolError("Polynomial order " + to_string(order) + " not implemented");

    const int N = p.size();
    vector<vector<int> > temp;

    for (unsigned int i = 0; i <= order; ++i) {
      Professor::Counter c(N,i);
      while (c.next(N-1)) {
        if (c.sum() == i) {
          temp.push_back(c.data());
        }
      }
    }

    vector<double> rtn;
    rtn.push_back(0.0); // Derivative of constant term
    for (const vector<int>& v : temp) {
      double part = 0.0;
      // Differentiate x^a*y^b*z^c*...
      for (unsigned int c = 0; c < v.size(); c++) { // d/dx, d/dy, d/dz, ...
        double temp2=1.0;
        for (unsigned int i = 0; i <v.size(); i++) { // x, y, z
          if (c==i) {  // d/dx x*y*z
            temp2*=v[i];
            if (v[c]==0) continue;
            else temp2*=std::pow(p[i], v[i]-1)/(maxPV[i]- minPV[i]); // Jacobian factor: 'd map_prange / dx' = 1./(b-a)
          }
          else {
            temp2*=      std::pow(p[i], v[i] );
          }
        }
        part +=temp2;
      }
      rtn.push_back(part);
    }

    return rtn;
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


  /// TODO: How do we want to read in the MinMaxValues here?
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
    _structure = mkStructure(dim(), order());
  }


  double Ipol::value(const vector<double>& params) const {
    if (params.size() != dim()) {
      stringstream ss;
      ss << "Incorrect number of parameters passed to Ipol::value ("
         << dim() << " params required, " << params.size() << " supplied)";
      throw IpolError(ss.str());
    }

    // Param scaling into [0,1] ranges defined by sampling limits (if set)
    vector<double> sparams = params;
    if (!_minPV.empty() && !_maxPV.empty()) {
      for (size_t i = 0; i < dim(); ++i) {
        sparams[i] = map_prange(params[i], _minPV[i], _maxPV[i]);
      }
    }

    // Dot product for value
    const vector<double> lv = mkLongVector(sparams, order(), _structure);
    assert(lv.size() == coeffs().size());
    double v = 0.0;
    for (size_t i = 0; i < lv.size(); ++i) {
      v += lv[i] * coeff(i);
    }
    return v;
  }

  double Ipol::derivative(const vector<double>& params) const {
    if (params.size() != dim()) {
      stringstream ss;
      ss << "Incorrect number of parameters passed to Ipol::derivative ("
         << dim() << " params required, " << params.size() << " supplied)";
      throw IpolError(ss.str());
    }

    // Param scaling into [0,1] ranges defined by sampling limits (if set)
    vector<double> sparams = params;
    if (!_minPV.empty() && !_maxPV.empty()) {
      for (size_t i = 0; i < dim(); ++i) {
        sparams[i] = map_prange(params[i], _minPV[i], _maxPV[i]);
      }
    }

    // Dot product for value
    const vector<double> lv = mkLongVectorDerivative(sparams, order(), _minPV, _maxPV, _structure);
    assert(lv.size() == coeffs().size());
    double v = 0.0;
    for (size_t i = 1; i < lv.size(); ++i) {
      v += lv[i] * coeff(i);
    }
    return v;
  }


}
